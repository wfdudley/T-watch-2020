// connect to wifi (stored in "file" acc_pts.txt (in SPIFF)), and then
// get the time via ntp.

#include "config.h"
#include "DudleyWatch.h"
#include <time.h>
#include <WiFi.h>
#include "my_tz.h"

#define __WIFIMAIN__
#include "my_WiFi.h"      // define "my" wifi stuff

#include "FS.h"		// this breaks if config.h is included.
#include "SPIFFS.h"	// supposed to include <FS.h>

#define DBGCLK 1

uint8_t Hh, Mm, Ss, wday, Day, Month;
uint16_t Year;
#define USE_SPIFFS_CPP 1
#define WE_HAVE_SPIFFS 0
#if WE_HAVE_SPIFFS
File ofile;
File ifile;
#endif

boolean initial = true;
char ssid[50];

//Are we currently connected?
char get[1024];
int getptr, pwptr, newtz;
char newpw[100], encnewpw[120], newtzstr[10];

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
  if(bestAP->channel && bestAP->macAddr) {
    WiFi.begin(bestAP->ssid, bestAP->pass, bestAP->channel, bestAP->macAddr);
  }
  else {
    WiFi.begin(bestAP->ssid, bestAP->pass);
  }

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

    tzindex = BestAP.tzone;
    if(general_config.local_tzindex != tzindex) {
      general_config.local_tzindex = tzindex;
      EEPROM_writeAnything(0, general_config);
      EEPROM.commit();
    }

    //Connect to the WiFi network
    connectToWiFi(&BestAP);
    return 0;
}

static uint8_t event_result;
static int16_t event_value;
static uint8_t button_num;
static uint8_t dropdown_num;
static lv_style_t style_box;	// what good is this?
static lv_obj_t * tv;
static lv_obj_t * ta4;	// WiFi password text area
static lv_obj_t * kb;
static void kb_event_cb4(lv_obj_t * ta, lv_event_t e);

static void button_handler(lv_obj_t *obj, lv_event_t event) {
    // Serial.printf("button_handler() event = %d\n", (int)event);
    if (event == LV_EVENT_CLICKED && !event_result) {
        Serial.println(F("Clicked"));
	event_result = 1;
	event_value = 1;
	lv_obj_t * label = lv_obj_get_child(obj, NULL);
	char * txt = lv_label_get_text(label);
	// Serial.printf("button label is %s\n", txt);
	button_num = (strcmp(txt, "Done")) ? 2 : 1 ;
	Serial.printf("handler label = %s, button_num = %d\n", txt, button_num);
	// lv_label_set_text_fmt(label, "Button: %d", cnt);
	return;
    }
    else if (event == LV_EVENT_VALUE_CHANGED && !event_result) {
        Serial.println(F("Toggled"));
	event_result = 1;
	lv_obj_t * label = lv_obj_get_child(obj, NULL);
	char * txt = lv_label_get_text(label);
	// Serial.printf("button label is %s\n", txt);
	button_num = (strcmp(txt, "Done")) ? 2 : 1 ;
	Serial.printf("handler label = %s, button_num = %d\n", txt, button_num);
	// state of toggled button
	event_value = lv_obj_get_state(obj, LV_BTN_PART_MAIN) & LV_STATE_CHECKED;
	Serial.printf("state = %d -> %s\n", event_value, event_value ? "true" : "false");
	return;
    }
}

static void kb_event_cb4(lv_obj_t * _kb, lv_event_t e) {
    lv_keyboard_def_event_cb(kb, e);

    if(e == LV_EVENT_CANCEL) {
        if(kb) {
            // lv_obj_set_height(tv, LV_VER_RES);
            lv_obj_del(kb);	// delete the keyboard if done.
            kb = NULL;
        }
    }
    if(e == LV_EVENT_APPLY) {
      // announce that the user is finished with the box.
      event_result = 3;
      event_value = 4;
    }
}

static void ta_event_cb4(lv_obj_t * ta, lv_event_t e) {
    if(e == LV_EVENT_RELEASED) {
        if(kb == NULL) {	// create a keyboard if there isn't one.
	    kb = lv_keyboard_create(lv_scr_act(), NULL);
	    lv_obj_set_size(kb,  LV_HOR_RES, LV_VER_RES / 2);
	    lv_obj_align(kb, NULL, LV_ALIGN_CENTER,  0, -60);
	    lv_keyboard_set_mode(kb, LV_KEYBOARD_MODE_TEXT_UPPER);
	    lv_keyboard_set_textarea(kb, ta);
            lv_obj_set_event_cb(kb, kb_event_cb4);
            lv_indev_wait_release(lv_indev_get_act());
        }
        lv_textarea_set_cursor_hidden(ta, false);
        // lv_page_focus(t1, lv_textarea_get_label(ta), LV_ANIM_ON);
        lv_keyboard_set_textarea(kb, ta);
    } else if(e == LV_EVENT_DEFOCUSED) {
        lv_textarea_set_cursor_hidden(ta, true);
    }
}

static void dd_event_cb1(lv_obj_t * obj, lv_event_t event)
{
  if(event == LV_EVENT_VALUE_CHANGED) {
    char buf[32];
    lv_dropdown_get_selected_str(obj, buf, sizeof(buf));
    printf("Option: %s\n", buf);
    event_result = 4;
    event_value = lv_dropdown_get_selected(obj);
    dropdown_num = 1;
  }
}

static void dd_event_cb3(lv_obj_t * obj, lv_event_t event)
{
  if(event == LV_EVENT_VALUE_CHANGED) {
    char buf[32];
    lv_dropdown_get_selected_str(obj, buf, sizeof(buf));
    Serial.printf("Option: %s\n", buf);
    event_result = 4;
    event_value = lv_dropdown_get_selected(obj);
    dropdown_num = 3;
  }
}

#if HAS_TABS
#define PARENT parent
#else
#define PARENT lv_scr_act()
#endif

static void page_create(lv_obj_t * parent) {
char buff[512];
static char buf[66];
int selected;
  lv_obj_t *label;
  // lv_page_set_scrl_layout(PARENT, LV_LAYOUT_PRETTY_TOP);
  lv_obj_t * h = lv_cont_create(PARENT, NULL);
  // lv_obj_set_width(h, LV_DPI * 2);
  lv_obj_set_width(h, 240);
  // lv_obj_set_height(h, 200);
  lv_obj_set_height(h, 240);

  lv_obj_t *btn1 = lv_btn_create(h, NULL);
  lv_obj_set_event_cb(btn1, button_handler);
  lv_obj_align(btn1, NULL, LV_ALIGN_CENTER, -20, 90);

  label = lv_label_create(btn1, NULL);
  lv_obj_set_size(btn1, 100, 50); //set the button size
  lv_label_set_text(label, "Done");

  /* Create the dropdown for timezone */
  lv_obj_t * dd1 = lv_dropdown_create(h, NULL);
  lv_obj_add_style(dd1, LV_CONT_PART_MAIN, &style_box);
  lv_obj_set_style_local_value_str(dd1, LV_CONT_PART_MAIN, LV_STATE_DEFAULT, "  Time Zone");
  // lv_obj_set_width(dd1, lv_obj_get_width_grid(h, disp_size <= LV_DISP_SIZE_SMALL ? 1 : 2, 1));
  lv_obj_align(dd1, NULL, LV_ALIGN_CENTER, 0, -5);
  lv_obj_set_width(dd1, 110);
  buff[0] = '\0';
  // Serial.printf("sizeof_tz_opts = %d, local_tzindex = %d\n", sizeof_tz_opts, general_config.local_tzindex);
  for(int i = 0 ; i < sizeof_tz_opts ; i++) {
    strcat(buff, tz_opts[i].tzname);
    if(i < sizeof_tz_opts - 1) {
      strcat(buff, "\n");
    }
    if(tz_opts[i].tzone == general_config.local_tzindex) {
      Serial.printf("home_tzindex, found match i = %d, value %s\n", i, tz_opts[i].tzname);
      selected = i;
    }
  }
  lv_dropdown_set_options(dd1, buff);
  lv_dropdown_set_selected(dd1, selected);
  lv_obj_set_event_cb(dd1, dd_event_cb1);

  /* Create the dropdown for ssid */
  lv_obj_t * dd3 = lv_dropdown_create(h, NULL);
  lv_obj_add_style(dd3, LV_CONT_PART_MAIN, &style_box);
  lv_obj_set_style_local_value_str(dd3, LV_CONT_PART_MAIN, LV_STATE_DEFAULT, "  SSID");
  // lv_obj_set_width(dd3, lv_obj_get_width_grid(h, disp_size <= LV_DISP_SIZE_SMALL ? 1 : 2, 1));
  lv_obj_align(dd3, NULL, LV_ALIGN_CENTER,   0, -70);
  lv_obj_set_width(dd3, 110);
  buff[0] = '\0';
  for(int i = 0 ; i < number_of_networks ; i++) {
    strncpy(buf, WiFi.SSID(i).c_str(), sizeof(ssid));
    strcat(buf, "\n");
    strcat(buff, buf);
  }
  buff[strlen(buff)-1] = '\0';	// erase final newline

  lv_dropdown_set_options(dd3, buff);
  // lv_dropdown_set_selected(dd3, selected);
  lv_obj_set_event_cb(dd3, dd_event_cb3);

  /* Create the text area for WiFi password */
  ta4 = lv_textarea_create(h, NULL);
  lv_obj_set_event_cb(ta4, ta_event_cb4);
  lv_obj_set_width(ta4, 150);
  lv_obj_align(ta4, NULL, LV_ALIGN_CENTER, 0, 80);	// order of operations matters!
  lv_textarea_set_max_length(ta4, 20);
  lv_textarea_set_one_line(ta4, true);
  memset(buff, '\0', sizeof(buff));
  lv_textarea_set_text(ta4, buff);
  lv_obj_t *ta4_name = lv_label_create(h, NULL);
  lv_label_set_text(ta4_name, "WiFi password");
  lv_obj_set_auto_realign(ta4_name, true);
  lv_obj_align(ta4_name, ta4, LV_ALIGN_OUT_BOTTOM_MID, 0,  0);
}

int get_wifi_credentials_from_user (void) {
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
#else
uint32_t utzidx = 0;
char ussid[66];
char upass[66];
memset(ussid, '\0', sizeof(ussid));
memset(upass, '\0', sizeof(upass));
  // use LVGL to make a settings screen for new wifi access point.
#if 0
  tv = lv_tabview_create(lv_scr_act(), NULL);
#endif
  lv_style_init(&style_box);
  lv_style_set_value_align(&style_box, LV_STATE_DEFAULT, LV_ALIGN_OUT_TOP_LEFT);
  lv_style_set_value_ofs_y(&style_box, LV_STATE_DEFAULT, - LV_DPX(10));
  lv_style_set_margin_top(&style_box, LV_STATE_DEFAULT, LV_DPX(30));
  page_create(lv_scr_act());
  while(1) {
    my_idle();
    lv_task_handler();
    delay(5);
    if(event_result) {
      switch(event_result) {
	case 1 :	// button
	  Serial.printf("eloop: button %d, value = %d\n", button_num, event_value);
	  switch (button_num) {
	    case 1 :
	      goto Exit;
	  }
	  break;
	case 2 :	// slider
	  break;
	case 3 :	// text box
	  if(event_value == 4) {	// user is done, save wifi password
	    const char * txt = lv_textarea_get_text(ta4);
	    Serial.printf("set WiFi password = %s\n", txt);
	    strncpy(upass, txt, sizeof(upass));
	  }
	  break;
	case 4 :	// dropdown
	  if(dropdown_num == 1) {
	    Serial.printf("set timezone to index %d -> %d\n", event_value, tz_opts[event_value].tzone);
	    utzidx = tz_opts[event_value].tzone;
	    // general_config.local_tzindex = tz_opts[event_value].tzone;
	    // tzindex = general_config.local_tzindex;
	  }
	  else if(dropdown_num == 3) {
	    Serial.printf("chosen ssid is %s\n", WiFi.SSID(event_value).c_str());
	    strncpy(ussid, WiFi.SSID(event_value).c_str(), sizeof(ussid));
	  }
	  break;
      }
      event_result = 0;
    }
  }
Exit:
  if(utzidx && ussid[0] && upass[0]) {
    BestAP.tzone = utzidx;
    strncpy(BestAP.ssid, ussid, sizeof(BestAP.ssid));
    strncpy(BestAP.pass, upass, sizeof(BestAP.pass));
    Serial.printf("get_wifi_credentials_from_user: ssid %s, pass %s, tzone %u\n", ussid, upass, utzidx);
    append_new_access_point(ussid, upass, utzidx);
    ttgo->tft->fillScreen(TFT_BLACK);
    return(1);
  }
#endif
  return(-1);
}

int connect_to_wifi_and_get_time (boolean verbose) {
int err, ecnt, this_wifi;
    if(connect_to_wifi(verbose, &BestAP, true) && verbose) {
      Serial.println(F("trying get wifi credentials from user"));
      // prompt_user_config1();
#if WEB_WIFI_SETUP
      get_wifi_credentials_from_user();
      do {
        this_wifi = APserver_loop();
        delay(1);
        // prompt_user_config2();
      } while(this_wifi < 0);
#else
      this_wifi = get_wifi_credentials_from_user();
#endif
      if(this_wifi >= 0) {
        tzindex = BestAP.tzone;
        Serial.printf("got wifi credentials, this_wifi = %d\n", this_wifi);
	Serial.printf("before connectToWiFi: ssid %s, pass %s, tzone %u\n", BestAP.ssid, BestAP.pass, BestAP.tzone);
        connectToWiFi(&BestAP);
      }
      else {
        Serial.printf("didn't get wifi credentials, this_wifi = %d\n", this_wifi);
      }
    }
    ecnt = 0;
    while(!connected && ecnt < 30) {
#if DBGCLK
      Serial.println(F("waiting 2 seconds for wifi connection"));
#endif
      ecnt++;
      delay(2000);
    }
    if(ecnt == 30) {
#if DBGCLK
      Serial.println(F("couldn't get wifi connection in 60 seconds!"));
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

