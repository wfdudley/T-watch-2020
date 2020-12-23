// scan for wifi signals

#include "config.h"
#include "DudleyWatch.h"
#include <string.h>
// #include "esp_wifi.h"
#include <WiFi.h>
#include <Wire.h>
#include <Ethernet.h>
#include "my_WiFi.h"

#define DBGWIFISCAN 0
#define verbose true

#define PAGE1 1
#define PAGE2 2

extern int wifi_scan(boolean);
#define SCANDELAY 8000

static uint32_t scancolors[] = { TFT_RED, TFT_ORANGE, TFT_YELLOW, TFT_GREEN, TFT_BLUE, TFT_PURPLE, TFT_LIGHTGREY, TFT_WHITE, TFT_CYAN, TFT_MAGENTA };
#define SSID_ROWS 20
#define SSID_LEN  30

void appWiFiScan(void) {
uint32_t last_scan;
int err, ecnt, lastmode, mode, mSelect, known_wifi, last_used_color;
int channel, strength;
uint8_t occupied[15];
int16_t x, y;
char ssid[50];
char ssid_array[SSID_ROWS][SSID_LEN];
uint32_t ssid_color_array[SSID_ROWS];
  tft->fillScreen(TFT_BLACK); // Clear screen
  tft->setTextSize(1);
  tft->setTextColor(TFT_YELLOW, TFT_BLACK);
  uint8_t td = tft->getTextDatum();
  tft->setTextDatum(TC_DATUM);
  tft->drawString("SCANNING . . .", tft->width()/2, 0, 2);
  tft->setTextDatum(td);
  delay(100);
  last_scan = 0;
  mode = PAGE1;
  lastmode = 0;
  while(1) {
    if(millis() > last_scan + SCANDELAY) {
#if DBGWIFISCAN
      Serial.printf("last_scan = %lu, millis() = %lu, time to do scan", last_scan, millis());
#endif
      last_scan = millis();
      memset(ssid_array, '\0', sizeof(ssid_array));
      last_used_color = 0;
      known_wifi = wifi_scan(false);
#if DBGWIFISCAN
      Serial.printf("mode = %d, known_wifi = %d\n", mode, known_wifi);
#endif
      for (int i = 0; i < number_of_networks && i < 11; ++i) {
	strncpy(ssid, WiFi.SSID(i).c_str(), sizeof(ssid));
	for(int j = 0 ; j < SSID_ROWS ; j++) {
	  if(!strcmp(ssid_array[j], ssid)) {
	    break;
	  }
	  if(!ssid_array[j][0]) {	// unused slot, "new" ssid
	    strncpy(ssid_array[j], ssid, SSID_LEN);
	    ssid_color_array[j] = scancolors[last_used_color++];
#if DBGWIFISCAN
	    Serial.printf("storing %s in slot %d, color is %x\n", ssid, j, ssid_color_array[j]);
#endif
	    break;
	  }
	}
      }
      lastmode = -1;
    }
    if(mode == PAGE1 && mode != lastmode) {
      lastmode = mode;
      tft->fillScreen(TFT_BLACK); // Clear screen
      tft->setTextSize(1);
      tft->setTextDatum(TL_DATUM);
      tft->setTextColor(TFT_YELLOW, TFT_BLACK);
      tft->drawString("WiFi Access Points",  0, 18, 2);
      tft->setTextFont(2);
      tft->setTextColor(TFT_GREEN, TFT_BLACK);
      tft->setCursor(0, 35);
      tft->print("SSID");
      tft->setCursor(150, 35);
      tft->print("chan");
      tft->setCursor(200, 35);
      tft->print("RSSI");
      for (int i = 0; i < number_of_networks && i < 11; ++i) {
	strncpy(ssid, WiFi.SSID(i).c_str(), sizeof(ssid));
	tft->setCursor(0, 50 + (15 * i));
	tft->print(ssid);
	tft->setCursor(150, 50 + (15 * i));
	tft->print(WiFi.channel(i));
	tft->setCursor(200, 50 + (15 * i));
	tft->print(WiFi.RSSI(i));
      }
#if DBGWIFISCAN
      Serial.println(F("polling"));
#endif
    }
    else if(mode == PAGE2 && mode != lastmode) {
      lastmode = mode;
      tft->fillScreen(TFT_BLACK); // Clear screen
      tft->setTextSize(1);
      for(int j = 0 ; j < SSID_ROWS ; j++) {
	if(ssid_array[j][0]) {
	  tft->setTextColor(ssid_color_array[j], TFT_BLACK);
	  if(strlen(ssid_array[j]) > 15) {
	    tft->setTextFont(1);
	    tft->setCursor((j&1)*120, 4+((0+(j/2)) * 16));
	  }
	  else {
	    tft->setTextFont(2);
	    tft->setCursor((j&1)*120, (0+(j/2)) * 16);
	  }
	  tft->printf("%-20.20s", ssid_array[j]);
#if DBGWIFISCAN
	  Serial.printf("setCursor(%d, %d), ", (j&1)*120, (0+(j/2)) * 16);
	  Serial.printf("ssid[%d] = %s\n", j, ssid_array[j]);
#endif
	}
	else {
	  break;
	}
      }
      tft->setTextColor(TFT_GREEN, TFT_BLACK);
      // HORIZONTAL axis:
      tft->setTextFont(1);
      for (int i = 0 ; i < 14 ; i++) {
	  tft->setCursor(-3+(240 * (i+1))/15, 240 - 15);
	  tft->printf("%d", i+1);
      }
      tft->drawLine(0, 240 - 17, 239, 240 - 17, TFT_DARKGREY);
      // VERTICAL axis:
      for (int s = -90 ; s < -10 ; s+= 10) {
	  tft->drawLine(0, 240 - ((110+s)*2), 240-26, 240 - ((110+s)*2), TFT_DARKGREY);
	  tft->setCursor(240 - 25, -3+240-((110+s)*2));
	  tft->printf("%4d", s);
      }
      tft->setCursor(240 - 12, 5+240-((110+-10)*2));
      tft->printf("%s", "dB");
      tft->setTextFont(2);
      // END axis drawing
      memset(occupied, '\0', sizeof(occupied));	// number of SSIDs on each chan
      for (int i = 0; i < number_of_networks && i < 11; ++i) {
	strncpy(ssid, WiFi.SSID(i).c_str(), sizeof(ssid));
	channel = WiFi.channel(i);
	strength = WiFi.RSSI(i);
	int found = 0;
	for(int j = 0 ; j < SSID_ROWS ; j++) {
	  if(!strcmp(ssid_array[j], ssid)) {
	    found = j;
	    break;
	  }
	}
	tft->drawLine(2*occupied[channel] + (240 * channel)/15, 240-16, 2*occupied[channel] + (240 * channel)/15, 240-((110+strength)*2), ssid_color_array[found]);
	occupied[channel]++;
      }
#if DBGWIFISCAN
      Serial.println(F("polling"));
#endif
    }
    mSelect = poll_swipe_or_menu_press(16); // poll for touch or gesture
    switch(mSelect) {
      case LEFT :
#if DBGWIFISCAN
	Serial.println(F("LEFT"));
#endif
	if(mode < PAGE2) mode++;
	break;
      case RIGHT :
#if DBGWIFISCAN
	Serial.println(F("RIGHT"));
#endif
	if(mode > PAGE1) mode--;
	break;
      case UP :
	break;
      case DOWN :
	goto ExitWiFiScan;
      case CWCIRCLE :
	break;
      case CCWCIRCLE :
	break;
    }
    my_idle();
  }
ExitWiFiScan:
  while (ttgo->getTouch(x, y)) { // wait until user lifts finger to exit
    my_idle();
  }
  WiFi.mode(WIFI_OFF);
  tft->fillScreen(TFT_BLACK); // Clear screen
}
