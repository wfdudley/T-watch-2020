// connect to wifi (stored in "file" acc_pts.txt (in SPIFF)), and then
// get the time via ntp.

#include "config.h"
#include "DudleyWatch.h"
#include <time.h>
#include <FS.h>		// why is File is undefined!!!
#include <SPIFFS.h>	// supposed to include <FS.h> but File is undefined!!!
// #include "esp_wifi.h"
#include <WiFi.h>
#include <WiFiUdp.h>
#include <Wire.h>
#include "my_tz.h"

#define __WIFIMAIN__

#include "my_WiFi.h"      // define "my" wifi stuff

#define DBGCLK 1

//The udp library class
WiFiUDP udp;

uint8_t Hh, Mm, Ss, wday, Day, Month;
uint16_t Year;
#define WE_HAVE_SPIFFS 0
#if WE_HAVE_SPIFFS
File ofile;
File ifile;
#endif

boolean initial = true;
char ssid[50];

//Are we currently connected?
int num_saved_ap, best_ap;
unsigned char is_dim;
char get[1024];
int getptr, pwptr, newtz;
char newpw[100], encnewpw[120], newtzstr[10];
int number_of_networks;
int read_acc_pts_file_and_compare_with_SSIDs (struct WiFiAp *);
int connect_to_wifi_and_get_time (boolean);
#if WEB_WIFI_SETUP
const char* ap_ssid = "TTGO-AP";
// Set web server port number to 80
WiFiServer APserver(80);
#endif

void appNTPTime(void) {
  if(!connect_to_wifi_and_get_time(true)) {
    // Get the current date/time
    RTC_Date tnow = ttgo->rtc->getDateTime();

    hh = tnow.hour;
    mm = tnow.minute;
    ss = tnow.second;
    dday = tnow.day;
    mmonth = tnow.month;
    yyear = tnow.year;

    tft->setTextSize(2);
    tft->setCursor(5, 210);
    tft->printf("%02d:%02d:%02d %02d/%02d/%4d", hh, mm, ss, mmonth, dday, yyear);
    // my vars are: Hh, Mm, Ss, Year, Month, Day);
    // put a delay() here eventually.
    int16_t x, y;
    while (!ttgo->getTouch(x, y)) {	// Wait for touch
      my_idle();
    }
    while (ttgo->getTouch(x, y)) {	// Wait for release
      my_idle();
    }
    //Clear screen 
    ttgo->tft->fillScreen(TFT_BLACK);
  }
  else {
    appSetTime();
  }
}

// stub; this was a web server to get access point information
int APserver_loop(void) {
    Serial.println("inside APserver_loop()");
    delay(100);
}

boolean is_known_tzone (int8_t tzind) {
  for(int k = 0 ; k < sizeof_tz_opts ; k++) {
    if(tzind == tz_opts[k].tzone) {
      return true;
    }
  }
  return false;
}

// returns index of an SSID we know, or -1
int wifi_scan(boolean verbose) {
int ap_result;
    ap_result = -1;
int best_strength;
int best_scan_result;
char ssid[50];
    if(verbose) {
      tft->fillScreen(TFT_BLACK);
      ttgo->tft->setTextColor(TFT_YELLOW, TFT_BLACK);
      tft->setTextDatum(MC_DATUM);
      tft->setTextSize(1);
      tft->drawString("Scan Network", half_width, 5, 2);
      tft->setTextDatum(TL_DATUM);
    }

    WiFi.mode(WIFI_STA);
    WiFi.disconnect();
    delay(100);

    number_of_networks = WiFi.scanNetworks();
    if(number_of_networks == 0) {
      if(verbose) {
	ttgo->tft->setTextColor(TFT_ORANGE, TFT_BLACK);
	tft->setTextDatum(MC_DATUM);
        ttgo->tft->drawString("no SSIDs found", half_width, half_height);
	tft->setTextDatum(TL_DATUM);
      }
    }
    else {
      if(verbose) {
	tft->setTextDatum(TL_DATUM);
	tft->setTextColor(TFT_YELLOW, TFT_BLACK);
	tft->drawString("WiFi Access Points",  0, 18, 2);
	tft->setTextColor(TFT_GREEN, TFT_BLACK);
	tft->setCursor(0, 35);
	tft->print("SSID");
	tft->setCursor(150, 35);
	tft->print("chan");
	tft->setCursor(200, 35);
	tft->print("RSSI");
	if(number_of_networks) {
	  for (int i = 0; i < number_of_networks && i < 8; ++i) {
	    strncpy(ssid, WiFi.SSID(i).c_str(), sizeof(ssid));
	    tft->setCursor(0, 50 + (15 * i));
	    tft->print(ssid);
	    tft->setCursor(150, 50 + (15 * i));
            tft->print(WiFi.channel(i));
	    tft->setCursor(200, 50 + (15 * i));
	    tft->print(WiFi.RSSI(i));
	  }
	}
      }
#if DBGCLK
      Serial.printf("Found %d SSIDs:\n", number_of_networks);
      Serial.printf("%32.32s %4s\n", "SSID", "RSSI", "CHANNEL");
      for (int i = 0; i < number_of_networks; ++i) {
        Serial.printf("%32.32s %4d %4d\n", WiFi.SSID(i).c_str(),
            WiFi.RSSI(i),
            WiFi.channel(i));
      }
#endif
      /////////////////////////////////////////////////////////////
      ap_result = read_acc_pts_file_and_compare_with_SSIDs(&BestAP);
      if(ap_result < 0) {
	Serial.println(F("couldn't open acc_pts.txt file"));
	return -1;
      }
      else if(ap_result == 0) {
	Serial.println(F("I don't recognize any SSIDs"));
	return -1;
      }
      else {
#if DBGCLK
	Serial.printf("found SSID we know: %s\n", BestAP.ssid);
#endif
	return ap_result;
      }
    }
    if(verbose) {
      tft->setCursor(0, 0);
    }
    tft->setTextDatum(TL_DATUM);
    WiFi.mode(WIFI_OFF);
    return ap_result;
}

// wifi event handler
void WiFiEvent(WiFiEvent_t event) {
    switch(event) {
      case SYSTEM_EVENT_STA_GOT_IP:
	  //When connected set
#if DBGCLK
	  Serial.print(F("WiFi connected! IP address: "));
	  Serial.println(WiFi.localIP());
#endif
	  //initializes the UDP state
	  //This initializes the transfer buffer
	  // udp.begin(WiFi.localIP(),udpPort);
	  connected = true;
	  break;
      case SYSTEM_EVENT_STA_DISCONNECTED:
#if DBGCLK
	  Serial.println(F("WiFi lost connection"));
#endif
	  connected = false;
	  break;
    }
}

void connectToWiFi(struct WiFiAp * bestAP) {
#if DBGCLK
  // Serial.println("Connecting to WiFi network: " + String(bestAP->ssid));
  Serial.printf("Connecting to WiFi network: %s\n  password %s channel %d mac %02x:%02x:%02x:%02x:%02x:%02x\n", bestAP->ssid, bestAP->pass, bestAP->channel,
    bestAP->macAddr[0], bestAP->macAddr[1], bestAP->macAddr[2],
    bestAP->macAddr[3], bestAP->macAddr[4], bestAP->macAddr[5]);
#endif

  // delete old config
  WiFi.disconnect(true);
  //register event handler
  WiFi.onEvent(WiFiEvent);

  //Initiate connection
  WiFi.mode(WIFI_STA);
  WiFi.begin(bestAP->ssid, bestAP->pass, bestAP->channel, bestAP->macAddr);

#if DBGCLK
  Serial.println(F("Waiting for WIFI connection..."));
#endif
}

const char* ntpServer1 = "pool.ntp.org";
const char* ntpServer2 = "time.nis.gov";
time_t utc_time;
time_t unix_time;
// don't use this, Timezone.h handles the offset:
unsigned long gmtOffset_sec = 0L;
// const int daylightOffset_sec = 3600;
const int daylightOffset_sec = 0;

int printLocalTime(void) {
struct tm timeinfo;
  if(!getLocalTime(&timeinfo)) {
#if DBGCLK
    Serial.println(F("printLocalTime(): Failed to obtain NTP time."));
#endif
    return -1;
  }
#if DBGCLK
  Serial.println(&timeinfo, "%A, %B %d %Y %H:%M:%S");
#endif
  utc_time = mktime(&timeinfo);
  Hh = timeinfo.tm_hour;
  Mm = timeinfo.tm_min;
  Ss = timeinfo.tm_sec;
  wday = timeinfo.tm_wday;
  Day = timeinfo.tm_mday;
  Month = 1 + timeinfo.tm_mon;
  Year = 1900 + timeinfo.tm_year;
  // hack in correct from EDT to GMT
  // Day++;
  // Hh += 4; Hh %= 24;
  // this SHOULD be GMT at this point, however . . . it isn't.  WTF?
  // now that I've saved GMT to the rtc, it remembers no time offset.
  ttgo->rtc->setDateTime(Year, Month, Day, Hh, Mm, Ss);
  Serial.printf("printLocalTime(): UTC = %02d:%02d:%02d %4d/%02d/%02d\n", Hh, Mm, Ss, Year, Month, Day);
  return 0;
}

// returns 1 if no known AP found
int connect_to_wifi(boolean verbose, struct WiFiAp * bestAP, boolean do_scan) {
int err, ecnt, this_wifi;
    if(do_scan) {
      best_ap = this_wifi = wifi_scan(verbose);
    }
    else {
      this_wifi = best_ap;
    }
    if(this_wifi < 0) {
      return 1;
    }
    //Connect to the WiFi network
    // ESP32: don't use this, I think it fights with Timezone.h stuff:
    // gmtOffset_sec = BestAP.tzone * 3600;
    tzindex = BestAP.tzone;
    general_config.local_tzindex = tzindex;
    EEPROM_writeAnything(0, general_config);
    EEPROM.commit();
    connectToWiFi(&BestAP);
    return 0;
}

void get_wifi_credentials_via_AP (void) {
#if WEB_WIFI_SETUP
  // Connect to Wi-Fi network with SSID and password
  Serial.print("Setting AP (Access Point)…");
  // Remove the password parameter, if you want the AP (Access Point) to be open
  // WiFi.softAP(ap_ssid, ap_pass);
  WiFi.softAP(ap_ssid);

  IPAddress IP = WiFi.softAPIP();
  Serial.print(F("AP IP address: "));
  Serial.println(IP);

  APserver.begin();
#endif
}

int connect_to_wifi_and_get_time (boolean verbose) {
int err, ecnt, this_wifi;
    if(connect_to_wifi(verbose, &BestAP, true) && verbose) {
      Serial.println(F("opening AP to get wifi credentials from user"));
      // prompt_user_config1();
      get_wifi_credentials_via_AP();
      do {
        this_wifi = APserver_loop();
        delay(1);
        // prompt_user_config2();
      } while(this_wifi < 0);
      if(this_wifi >= 0) {
        tzindex = BestAP.tzone;
        connectToWiFi(&BestAP);
        Serial.printf("got wifi credentials, this_wifi = %d\n", this_wifi);
      }
      else {
        Serial.printf("didn't get wifi credentials, this_wifi = %d\n", this_wifi);
      }
    }
    ecnt = 0;
    while(!connected && ecnt < 40) {
#if DBGCLK
      Serial.println(F("waiting 2 seconds for wifi connection"));
#endif
      ecnt++;
      delay(2000);
    }
    if(ecnt == 40) {
#if DBGCLK
      Serial.println(F("couldn't get wifi connection in 80 seconds!"));
#endif
      return 1;
    }
    if(verbose) {
      tft->setTextColor(TFT_YELLOW, TFT_BLACK);
      tft->setCursor(0, 50 + (15 * 8));
      tft->printf("Connected to %s channel %d", BestAP.ssid, BestAP.channel);
    }

    //init and get the time
    configTime(gmtOffset_sec, daylightOffset_sec, ntpServer1, ntpServer2);
    ecnt = 0;
    do {
      err = printLocalTime();   // this sets utc_time
#if DBGCLK
      if(err) Serial.println(F("re-trying printLocalTime()"));
#endif
      ecnt++;
      delay(100);
    } while(err && ecnt < 10);
    connected = false;
    WiFi.mode(WIFI_OFF);
    return 0;
}

int read_acc_pts_file_and_compare_with_SSIDs (struct WiFiAp * bestAP) {
char filessid[33], pass[65], tzonestr[8];
int tzone, cp, phase, result;
int best_strength;
  Serial.println(F("reading acc_pts.txt file:"));
  memset(filessid, '\0', sizeof(filessid));
  memset(pass, '\0', sizeof(pass));
  memset(tzonestr, '\0', sizeof(tzonestr));
  cp = 0;
  phase = 0;
  result = 0;
  best_strength = -200;
#if WE_HAVE_SPIFFS
  Serial.printf("EEPROM size = %d\n", EEPROM_SIZE);
  if(!SPIFFS.exists("/acc_pts.txt")) {
    Serial.println(F("can't find acc_pts.txt, creating it"));
    ofile = SPIFFS.open("/acc_pts.txt", FILE_WRITE);
    for(int i = 0 ; i < sizeof(AccessPoints)/sizeof(WIFIAP) ; i++) {
      ofile.printf("%s\t%s\t%d\n", AccessPoints[i].ssid, AccessPoints[i].pass, AccessPoints[i].tzone);
    }
    ofile.close();
  }

  ifile = SPIFFS.open("/acc_pts.txt", "r");
  if(!ifile){
    return -1;
  }
  Serial.println(F("found /acc_pts.txt"));
  while(ifile.available()) {
    char c;
    c = ifile.read();
    Serial.write(c);
    if(c == '\t') {
      cp = 0;
      phase++;
    }
    else if(c == '\n') {
      // Now we have filessid, pass, tzone from acc_pts.txt file
      // compare each WiFi object ssid with filessid from the acc_pts.txt file
      for (int i = 0; i < number_of_networks; ++i) {
        // Serial.printf("comparing %s to %s\n", WiFi.SSID(i).c_str(), filessid);
#if NORMAL_WIFI_OPERATION       // disable to test the access point setup code
        // normally, this should be enabled.
        if(!strcmp(WiFi.SSID(i).c_str(), filessid)) {
          Serial.printf("is RSSI %d > best_strength %d ?\n", WiFi.RSSI(i), best_strength);
          if(WiFi.RSSI(i) > best_strength) {
            best_strength = WiFi.RSSI(i);
            result = 1;
            strncpy(bestAP->ssid, filessid, sizeof(BestAP.ssid));
            strncpy(bestAP->pass, pass, sizeof(BestAP.pass));
            memcpy(bestAP->macAddr, WiFi.BSSID(i), 6);
            bestAP->channel = WiFi.channel(i);
            bestAP->tzone = atoi(tzonestr);
            Serial.printf("best strength = %d, best ssid = %s\n", best_strength, filessid);
            Serial.printf("best channel %d, best mac %02x:%02x:%02x:%02x:%02x:%02x\n", bestAP->channel,
              bestAP->macAddr[0], bestAP->macAddr[1], bestAP->macAddr[2],
              bestAP->macAddr[3], bestAP->macAddr[4], bestAP->macAddr[5]);
          }
        }
#endif
      }
      cp = 0;
      phase = 0;
      memset(filessid, '\0', sizeof(filessid));
      memset(pass, '\0', sizeof(pass));
      memset(tzonestr, '\0', sizeof(tzonestr));
    }
    else {
      switch(phase) {
        case 0 :
          filessid[cp++] = c;
          break;
        case 1 :
          pass[cp++] = c;
          break;
        case 2 :
          tzonestr[cp++] = c;
          break;
      }
    }
  }
  ifile.close();
#else
  for(int i = 0 ; i < sizeof(AccessPoints)/sizeof(WIFIAP) ; i++) {
    if(!strcmp(WiFi.SSID(i).c_str(), AccessPoints[i].ssid)) {
      Serial.printf("is RSSI %d > best_strength %d ?\n", WiFi.RSSI(i), best_strength);
      if(WiFi.RSSI(i) > best_strength) {
	best_strength = WiFi.RSSI(i);
	result = 1;
	strncpy(bestAP->ssid, AccessPoints[i].ssid, sizeof(BestAP.ssid));
	strncpy(bestAP->pass, AccessPoints[i].pass, sizeof(BestAP.pass));
	memcpy(bestAP->macAddr, WiFi.BSSID(i), 6);
	bestAP->channel = WiFi.channel(i);
	bestAP->tzone = AccessPoints[i].tzone;
	Serial.printf("best strength = %d, best ssid = %s\n", best_strength, filessid);
	Serial.printf("best channel %d, best mac %02x:%02x:%02x:%02x:%02x:%02x\n", bestAP->channel,
	  bestAP->macAddr[0], bestAP->macAddr[1], bestAP->macAddr[2],
	  bestAP->macAddr[3], bestAP->macAddr[4], bestAP->macAddr[5]);
      }
    }
    // AccessPoints[i].ssid, AccessPoints[i].pass, AccessPoints[i].tzone
  }
#endif
  return result;
}

