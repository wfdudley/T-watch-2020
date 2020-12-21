// connect to wifi (stored in "file" acc_pts.txt (in SPIFF)), and then
// get the weather

#include "config.h"
#include "DudleyWatch.h"
#include <time.h>
#include <string.h>
// #include "esp_wifi.h"
#include <WiFi.h>

#include <Wire.h>
#include <Ethernet.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>
#include <AceTime.h>

#include "HTTPClient.h"
#include <OpenWeatherOneCall.h>
#include <NTPClient.h>
#include <WiFiUdp.h>

#include "my_WiFi.h"
#include "appMQTT.h"

extern WiFiClient espClient;
#include "icons.h"

#include "personal_info.h"

extern char str_latitude[];
extern char str_longitude[];
// 0 result means success
// -1 result means default to home IP
int get_lat_lon (void);
float my_latitude, my_longitude;

#define DBGWEATHER 0
#define verbose true

OpenWeatherOneCall OWOC;

#define ICONSIZEFIX 0
#if ICONSIZEFIX
#define ICFIX(n) do{x+=(w-n)/2;y+=(w-n)/2;w=n;h=n;}while(0);
#else
#define ICFIX(n)
#endif

// accepts icon name ("01d", "01n", etc) and pushes that image to TFT
void putIcon(int x, int y, int w, int h, const char * iconname) {
const short unsigned int *imagefile;
  tft->setSwapBytes(true);
  switch(iconname[0]) {
    case '0' :
      switch(iconname[1]) {
	case '1' :
	  switch(iconname[2]) {
	    case 'd' :
	      imagefile = i01d;
	      break;
	    case 'n' :
	      imagefile = i01n;
	      break;
	  }
	  break;
	case '2' :
	  switch(iconname[2]) {
	    case 'd' :
	      imagefile = i02d;
	      break;
	    case 'n' :
	      imagefile = i02n;
	      break;
	  }
	  break;
	case '3' :
	  switch(iconname[2]) {
	    case 'd' :
	      imagefile = i03d;
	      break;
	    case 'n' :
	      imagefile = i03n;
	      break;
	  }
	  break;
	case '4' :
	  switch(iconname[2]) {
	    case 'd' :
	      imagefile = i04d;
	      break;
	    case 'n' :
	      imagefile = i04n;
	      break;
	  }
	  break;
	case '9' :
	  switch(iconname[2]) {
	    case 'd' :
	      imagefile = i09d;
	      break;
	    case 'n' :
	      imagefile = i09n;
	      break;
	  }
	  break;
      }
      break;
    case '1' :
      switch(iconname[1]) {
	case '0' :
	  switch(iconname[2]) {
	    case 'd' :
	      imagefile = i10d;
	      break;
	    case 'n' :
	      imagefile = i10n;
	      break;
	  }
	  break;
	case '1' :
	  switch(iconname[2]) {
	    case 'd' :
	      imagefile = i11d;
	      break;
	    case 'n' :
	      imagefile = i11n;
	      break;
	  }
	  break;
	case '3' :
	  switch(iconname[2]) {
	    case 'd' :
	      imagefile = i13d;
	      break;
	    case 'n' :
	      imagefile = i13n;
	      break;
	  }
	  break;
      }
      break;
    case '5' :
      switch(iconname[2]) {
	case 'd' :
	  imagefile = i50d;
	  break;
	case 'n' :
	  imagefile = i50n;
	  break;
      }
      break;
  }
  tft->pushImage(x, y, w, h, imagefile);
}

#define PAGE1 1
#define PAGE2 2
#define PAGE3 3
#define PAGE4 4
#define PAGE5 5
#define ICONTEST 27

int get_wifi_credentials_from_user (void);
void connectToWiFi(struct WiFiAp *);

void appWeather(void) {
uint32_t this_sec;
int err, ecnt, lastmode, mode, mSelect, this_wifi, alert_pages;
int last_char_index, alert_summary_length, page2sumidx[10], owcres;
int16_t x, y;
char temp_unit = (general_config.metric_units) ? 'C' : 'F';
const char *velocity_unit = (general_config.metric_units) ? "km/hr" : "mi/hr";
  if(connect_to_wifi(verbose, &BestAP, true, true) && verbose) {
      Serial.printf("connect to wifi failed\n");
      tft->setTextColor(TFT_YELLOW, TFT_BLACK);
      tft->setTextSize(1);
      tft->setCursor(0, 50 + (15 * 7));
      tft->print(F("Connect to WiFi Failed!"));
      tft->setCursor(0, 50 + (15 * 8));
      tft->setTextColor(TFT_GREEN, TFT_BLACK);
      tft->print(F("Opening New WiFi screen"));
      tft->setCursor(0, 50 + (15 * 9));
      tft->setTextColor(TFT_RED, TFT_BLACK);
      tft->print(F("remember to hit the checkmark on"));
      tft->setCursor(0, 50 + (15 * 10));
      tft->print(F("the keyboard when finished entering"));
      tft->setCursor(0, 50 + (15 * 11));
      tft->print(F("the password!"));
      delay(5000);
      Serial.println(F("trying get wifi credentials from user"));
      this_wifi = get_wifi_credentials_from_user();
      if(this_wifi >= 0) {
        Serial.printf("got wifi credentials, this_wifi = %d\n", this_wifi);
	Serial.printf("before connectToWiFi: ssid %s, pass %s, tzone %u\n", BestAP.ssid, BestAP.pass, BestAP.tzone);
        connectToWiFi(&BestAP);
      }
      else {
        Serial.printf("didn't get wifi credentials, this_wifi = %d\n", this_wifi);
	return;
      }
  }
  ecnt = 0;
  while(!connected && ecnt < 400) {
#if DBGWEATHER
    Serial.println(F("waiting 0.2 seconds for wifi connection"));
#endif
    ecnt++;
    delay(200);
  }
  if(ecnt == 400) {
#if DBGWEATHER
    Serial.println(F("couldn't get wifi connection in 80 seconds!"));
#endif
    tft->setTextColor(TFT_YELLOW, TFT_BLACK);
    tft->drawString("Connect to WiFi failed in 80 seconds!",  0, 5, 2);
    delay(3000);
    WiFi.mode(WIFI_OFF);
    connected = false;
    return;
  }
  if(verbose) {
    tft->setTextColor(TFT_GREEN, TFT_BLACK);
    tft->setTextSize(1);
    tft->setCursor(0, 50 + (15 * 9));
    tft->printf("Connected to %s channel %d", BestAP.ssid, BestAP.channel);
    tft->setCursor(0, 50 + (15 * 10));
    tft->print(F("Trying to connect to "));
    tft->print(F("openweathermap.com"));
    tft->setCursor(0, 50 + (15 * 11));
    tft->print(F("my ip is: "));
    tft->print(WiFi.localIP());
  }
  delay(1000);
#if DBGWEATHER
  Serial.println(F("Trying to get latitude and longitude from IP address"));
#endif
  // first, get our location:
  int locr = get_lat_lon();	// populate str_latitude & str_longitude
#if DBGWEATHER
  if(locr) {
    Serial.printf("at home OR couldn't get latitude and longitude from IP address\n");
  }
  else {
    Serial.printf("IP address -> str_latitude %s str_longitude %s\n", str_latitude, str_longitude);
  }
#endif
  if(str_latitude[0]) {
    my_latitude = atof(str_latitude);
  }
  if(str_longitude[0]) {
    my_longitude = atof(str_longitude);
  }
#define ALERT_TEST_LOCATION 0
#if ALERT_TEST_LOCATION
    // my_latitude = 47.1848;
    // my_longitude = -123.615;
    // my_latitude = 31.8354411
    // my_longitude = -93.2885038;
    // my_latitude = 13.421129;	// Guam
    // my_longitude = 144.73972;
    my_latitude = 29.9038;	// India
    my_longitude = -73.877190;
#endif
#if DBGWEATHER
  Serial.printf("IP address -> my_latitude %f my_longitude %f\n", my_latitude, my_longitude);

  Serial.println(F("Trying to connect to openweathermap.com"));
#endif
  //=================================
  // Get the Weather Forecast
  //=================================

  /* EXCLUDES ARE:
     EXCL_C(urrent) EXCL_D(aily) EXCL_H(ourly) EXCL_M(inutely) EXCL_A(lerts)
     In the form EXCL_C+EXCL_D+EXCL_H etc
     NULL == EXCLUDE NONE (Send ALL Info)
  */

  owcres = OWOC.setOpenWeatherKey(general_config.owm_api_key);
  if(owcres) {
    Serial.printf("OWOC error: %s\n", OWOC.getErrorMsgs(owcres));
    goto ExitWeather;
  }
  owcres = OWOC.setLatLon(my_latitude, my_longitude);
  if(owcres) {
    Serial.printf("OWOC error: %s\n", OWOC.getErrorMsgs(owcres));
    goto ExitWeather;
  }
  OWOC.setExcl(EXCL_H + EXCL_M);
  OWOC.setUnits(general_config.metric_units ? METRIC : IMPERIAL);

#if OWM_LANGUAGE_FEATURE
  OWOC.setLanguage(general_config.language);
#endif

  my_idle();
  owcres = OWOC.parseWeather();
  if(owcres) {
    Serial.printf("OWOC error: %s\n", OWOC.getErrorMsgs(owcres));
  }
  else {
    memset(page2sumidx, 0, sizeof(page2sumidx)/sizeof(char));
#if ERRORTEST
#if DBGWEATHER
    for(owcres = 1 ; owcres < 30 ; owcres++) {
      Serial.printf("error %d -> %s\n", owcres, OWOC.getErrorMsgs(owcres));
    }
#endif
#endif
    mode = PAGE1;
    lastmode = 0;
    alert_pages = 0;
    if(OWOC.alert) {
	if(OWOC.alert->summary && OWOC.alert->summary[0]) {
	    alert_pages = (419 + strlen(OWOC.alert->summary)) / 420;
#if DBGWEATHER
	    Serial.printf("alert_pages = %d\n", alert_pages);
#endif
	}
    }
    while(1) {
      if(mode == PAGE1 && mode != lastmode) {
	lastmode = mode;
	tft->fillScreen(TFT_BLACK);
	tft->setTextColor(TFT_YELLOW, TFT_BLACK);
	tft->drawCentreString("Weather Pg 1",  half_width, 0, 2);
	if(OWOC.alert) {
	    if(OWOC.alert->summary && OWOC.alert->summary[0]) {
		tft->setTextColor(TFT_RED, TFT_BLACK);
		tft->drawString("Alert ->", 185, 0, 2);
	    }
	}
	tft->setTextColor(TFT_GREEN, TFT_BLACK);
	tft->setTextFont(2);
	if(OWOC.current->summary && OWOC.current->summary[0]) {
	  tft->setCursor(0, 15);
	  tft->printf("%s", OWOC.current->summary);
#if DBGWEATHER
	  Serial.printf("current summary = %s\n", OWOC.current->summary);
#endif
	}
	tft->setCursor(0, 30);
	tft->printf("%.0f %c", OWOC.current->temperature, temp_unit);
	tft->setCursor(0, 45);
	tft->printf("%.0f %%RH", OWOC.current->humidity);
	tft->setCursor(0, 60);
	tft->printf("%.0f wind %s", OWOC.current->windSpeed, velocity_unit);
	tft->setCursor(0, 75);
	tft->printf("%.0f wind gust", OWOC.current->windGust);
	tft->setCursor(85, 30);
	tft->printf("%.0f %c dewpt", OWOC.current->dewPoint, temp_unit);
	tft->setCursor(85, 45);
	tft->printf("%.0f mbar", OWOC.current->pressure);
	tft->setCursor(85, 60);
	tft->printf("%.0f%% prob precip", 100.0 * OWOC.forecast[0].pop);
	tft->setCursor(85, 75);
	tft->printf("%.0f %c hi %.0f lo",
	  OWOC.forecast[0].temperatureHigh,
	  temp_unit,
	  OWOC.forecast[0].temperatureLow
	  );
	if(OWOC.current->icon && OWOC.current->icon[0]) {
	  putIcon(191, 31, 48, 48, OWOC.current->icon);
#if DBGWEATHER
	  Serial.printf("current icon = %s\n", OWOC.current->icon);
#endif
	}
#if DBGWEATHER
	if(OWOC.current->main && OWOC.current->main[0]) {
	  Serial.printf("current main = %s\n", OWOC.current->main);
	}
	Serial.printf("current temp = %.1f\n",      OWOC.current->temperature);
	Serial.printf("current humidity = %.1f\n",  OWOC.current->humidity);
	Serial.printf("current pressure = %.1f\n",  OWOC.current->pressure);
	Serial.printf("current windSpeed = %.1f\n", OWOC.current->windSpeed);
	Serial.printf("current windGust = %.1f\n",  OWOC.current->windGust);
	// Serial.printf("aW: line %d\n", __LINE__);
	Serial.printf("current precipProbability = %.0f %%\n", 100.0 * OWOC.forecast[0].pop);
	// Print 4 day forecast to Serial Monitor
	Serial.println("");
	Serial.println("4 Day Forecast:");
#endif

	// for (int fo = 0; fo < (sizeof(OWOC.forecast) / sizeof(OWOC.forecast[0])) - 1; fo++)
	for (int fo = 1; fo < 5 ; fo++)
	{
	  //Date from epoch forecast[fo].dayTime
	  // long DoW = OWOC.forecast[fo].dayTime / 86400L;
	  // int day_of_week = (DoW + 4) % 7;

#if DBGWEATHER
	  Serial.printf("%s: high = %.0f %c, low %.0f %c\n",
	      OWOC.forecast[fo].weekDayName,
	      // OWOC.short_names[day_of_week],
	      OWOC.forecast[fo].temperatureHigh, temp_unit,
	      OWOC.forecast[fo].temperatureLow, temp_unit);
	  Serial.printf("%.0f %% precip probability\n", 100.0 * OWOC.forecast[fo].pop);
	  if(OWOC.forecast[fo].summary && OWOC.forecast[fo].summary[0]) {
	    Serial.printf("summary[%d] =  %s\n", fo, OWOC.forecast[fo].summary);
	  }
	  if(OWOC.forecast[fo].main && OWOC.forecast[fo].main[0]) {
	    Serial.printf("main[%d] =  %s\n", fo, OWOC.forecast[fo].main);
	  }
	  if(OWOC.forecast[fo].icon && OWOC.forecast[fo].icon[0]) {
	    Serial.printf("icon[%d] =  %s\n", fo, OWOC.forecast[fo].icon);
	  }
#endif
	  int offset = 95;
	  tft->setCursor(60 * (fo-1) + 20, offset);
	  // tft->print(OWOC.short_names[day_of_week]);
	  tft->print(OWOC.forecast[fo].weekDayName);
	  offset += 15;
	  tft->setCursor(60 * (fo-1), offset);
	  tft->printf("%.0f F hi", OWOC.forecast[fo].temperatureHigh);
	  offset += 15;
	  tft->setCursor(60 * (fo-1), offset);
	  tft->printf("%.0f F lo", OWOC.forecast[fo].temperatureLow);
	  offset += 15;
	  tft->setCursor(60 * (fo-1), offset);
	  tft->printf("%.0f %%RH", OWOC.forecast[fo].humidity);
	  offset += 15;
	  tft->setCursor(60 * (fo-1), offset);
	  float pp = 100.0 * OWOC.forecast[fo].pop;
	  tft->printf("%.0f%% %s", pp, (pp < 100.0) ? "pop" : "pp");
	  offset += 15;
	  tft->setCursor(60 * (fo-1), offset);
	  if(OWOC.forecast[fo].main && OWOC.forecast[fo].main[0]) {
	    tft->printf("%s", OWOC.forecast[fo].main);
	  }
	  // ycoord = 185
	  if(OWOC.forecast[fo].icon && OWOC.forecast[fo].icon[0]) {
	    putIcon(60 * (fo-1) + 1, 191, 48, 48, OWOC.forecast[fo].icon);
	  }
	}
      }
      else if(mode == PAGE2 && mode != lastmode) {
	lastmode = mode;
	tft->fillScreen(TFT_BLACK);
	tft->setTextColor(TFT_YELLOW, TFT_BLACK);
	tft->drawCentreString("Weather Pg 2",  half_width, 0, 2);
	if(OWOC.alert) {
	    last_char_index = 0;
	    if(OWOC.alert->summary && OWOC.alert->summary[0]) {
		tft->setTextColor(TFT_RED, TFT_BLACK);
		tft->drawString("More ->", 190, 75, 2);
		alert_summary_length = strlen(OWOC.alert->summary);
	    }
	    if(OWOC.alert->event && OWOC.alert->event[0]) {
#if DBGWEATHER
		Serial.printf("alert.event = %s\n", OWOC.alert->event);
#endif
		tft->setCursor(0, 15);
		tft->printf("%s", OWOC.alert->event);
	    }
	    if(OWOC.alert->senderName && OWOC.alert->senderName[0]) {
#if DBGWEATHER
		Serial.printf("alert.senderName = %s\n", OWOC.alert->senderName);
#endif
		tft->setCursor(0, 30);
		tft->printf("%s", OWOC.alert->senderName);
		tft->setCursor(0, 60);
		tft->printf("from: %s", OWOC.alert->startInfo);
		tft->setCursor(0, 75);
		tft->printf("until: %s", OWOC.alert->endInfo);
	    }
	}
	tft->setTextColor(TFT_GREEN, TFT_BLACK);
	tft->setTextFont(2);

	// Print last 3 days forecast to Serial Monitor
#if DBGWEATHER
	Serial.println("\nlast 3 Days Forecast:");
#endif

	for (int fo = 5; fo < 8 ; fo++) {
#if DBGWEATHER
	  Serial.printf("%s: high = %.0f %c, low %.0f %c\n",
	      OWOC.forecast[fo].weekDayName,
	      OWOC.forecast[fo].temperatureHigh, temp_unit,
	      OWOC.forecast[fo].temperatureLow, temp_unit);

	  Serial.printf("%.0f %% precip probability\n", 100.0 * OWOC.forecast[fo].pop);
	  if(OWOC.forecast[fo].summary && OWOC.forecast[fo].summary[0]) {
	    Serial.printf("summary =     %s\n", OWOC.forecast[fo].summary);
	  }
	  if(OWOC.forecast[fo].main && OWOC.forecast[fo].main[0]) {
	    Serial.printf("main =     %s\n", OWOC.forecast[fo].main);
	  }
	  if(OWOC.forecast[fo].icon && OWOC.forecast[fo].icon[0]) {
	    Serial.printf("icon =     %s\n", OWOC.forecast[fo].icon);
	  }
#endif
	  int offset = 95;
	  tft->setCursor(60 * (fo-5) + 20, offset);
	  tft->print(OWOC.forecast[fo].weekDayName);
	  offset += 15;
	  tft->setCursor(60 * (fo-5), offset);
	  tft->printf("%.0f F hi", OWOC.forecast[fo].temperatureHigh);
	  offset += 15;
	  tft->setCursor(60 * (fo-5), offset);
	  tft->printf("%.0f F lo", OWOC.forecast[fo].temperatureLow);
	  offset += 15;
	  tft->setCursor(60 * (fo-5), offset);
	  tft->printf("%.0f %%RH", OWOC.forecast[fo].humidity);
	  offset += 15;
	  tft->setCursor(60 * (fo-5), offset);
	  float pp = 100.0 * OWOC.forecast[fo].pop;
	  tft->printf("%.0f%% %s", pp, (pp < 100.0) ? "pop" : "pp");
	  offset += 15;
	  tft->setCursor(60 * (fo-5), offset);
	  if(OWOC.forecast[fo].main && OWOC.forecast[fo].main[0]) {
	    tft->printf("%s", OWOC.forecast[fo].main);
	  }
	  // ycoord = 185
	  if(OWOC.forecast[fo].icon && OWOC.forecast[fo].icon[0]) {
	    putIcon(60 * (fo-5) + 1, 191, 48, 48, OWOC.forecast[fo].icon);
	  }
	}
      }
      else if(alert_pages
      && mode >= PAGE3
      && mode < ICONTEST
      && mode != lastmode) {
	int sx, sy, pagelen, eop;
	char oc;
	lastmode = mode;
	tft->fillScreen(TFT_BLACK);
	tft->setTextColor(TFT_YELLOW, TFT_BLACK);
	sprintf(buff, "Weather Alert Page %d", mode - 1);
	tft->drawCentreString(buff, half_width, 0, 2);
	tft->setTextColor(TFT_GREEN, TFT_BLACK);
	tft->setTextFont(2);
	pagelen = 30 * 12;
	if(!page2sumidx[mode - PAGE3] && mode > PAGE3) {
	  page2sumidx[mode - PAGE3] = last_char_index;
	}
	last_char_index = page2sumidx[mode - PAGE3];
	char * pp = &OWOC.alert->summary[last_char_index];
	char * pe;
	if(last_char_index + pagelen > strlen(OWOC.alert->summary)) {
	  pe = &OWOC.alert->summary[alert_summary_length - 1];
	}
	else {
	  pe = &OWOC.alert->summary[last_char_index + pagelen];
	}
	oc = *pe;
#if DBGWEATHER
	Serial.printf("alert.summary part %d = %s\n", mode - PAGE3, pp);
#endif
	if(last_char_index == 0) {
	  for(char *cp = pp ; *cp ; cp++) {
	      if((*cp == '\n' || *cp == '\r')
	      && !(*(cp + 1) == '*' || *(cp + 1) == '.')
	      && !(*(cp + 2) >= 'A' && *(cp + 2) <= 'Z')) {
		  *cp = ' ';
	      }
	  }
	}
	*pe = '\0';
	tft->setCursor(0, 15);
	tft->printf("%s", pp);
	sx = tft->getCursorX();
	sy = tft->getCursorY();
#if DBGWEATHER
	Serial.printf("after big chunk, x = %d, y = %d\n", sx, sy);
	Serial.printf("line %d, alert_summary_length = %d, last_char_index = %d\n", __LINE__, alert_summary_length, last_char_index);
#endif
	last_char_index += pe - pp;
#if DBGWEATHER
	Serial.printf("line %d, last_char_index = %d\n", __LINE__, last_char_index);
#endif
	*pe = oc;
#if DBGWEATHER
	Serial.printf("REMAINDER = %s\n", pe);
#endif
	eop = 0;
	if(alert_summary_length <= last_char_index) {
	  eop = 1;
	  alert_pages = mode - 2;
#if DBGWEATHER
	  Serial.printf("line %d, alert_summary_length = %d, last_char_index = %d, eop = %d, alert_pages = %d\n", __LINE__, alert_summary_length, last_char_index, eop, alert_pages);
#endif
	}
	while((sx < 180 || sy < 220) && !eop && OWOC.alert->summary[last_char_index]) {
	  tft->printf("%c", OWOC.alert->summary[last_char_index++]);
	  sx = tft->getCursorX();
	  sy = tft->getCursorY();
	  if(sy > 220 && sx > 120 && OWOC.alert->summary[last_char_index] == ' ') {
	    eop = 1;
#if DBGWEATHER
	    Serial.printf("last line break on a space, last_char_index = %d\n", last_char_index);
#endif
	  }
	  if(!OWOC.alert->summary[last_char_index]) {
	    eop = 1;
	  }
	  if(alert_summary_length <= last_char_index) {
	    eop = 1;
	    alert_pages = mode - 2;
#if DBGWEATHER
	    Serial.printf("line %d, END OF SUMMARY, alert_summary_length = %d, last_char_index = %d, eop = %d, alert_pages = %d\n", __LINE__, alert_summary_length, last_char_index, eop, alert_pages);
#endif
	  }
	}
	if(alert_summary_length > last_char_index) {
	  tft->setTextColor(TFT_RED, TFT_BLACK);
	  tft->drawString("More ->", 190, 223, 2);
	  tft->setTextColor(TFT_GREEN, TFT_BLACK);
	}
#if DBGWEATHER
	Serial.printf("line %d, after creeping to end, x = %d, y = %d\n", __LINE__, sx, sy);
	Serial.printf("line %d, last_char_index = %d\n", __LINE__, last_char_index);
#endif
      }
      else if(mode == ICONTEST && mode != lastmode) {
	lastmode = mode;
	const char *iconnames[] = {
	  "01d", "01n", "02d", "02n", "03d", "03n", "04d", "04n", "09d",
	  "09n", "10d", "10n", "11d", "11n", "13d", "13n", "50d", "50n",
	  "01d", "01n"
	};
	tft->fillScreen(TFT_BLACK);
	for (int r = 0 ; r < 4 ; r++) {
	  for (int c = 0 ; c < 5 ; c++) {
	    putIcon(c * 48, 12 + r * 60, 48, 48, iconnames[c + (r*5)]);
	    tft->drawString(iconnames[c + (r*5)], 15 + c * 48, r * 60, 2);
	  }
	}
      }
      mSelect = poll_swipe_or_menu_press(16); // poll for touch or gesture
      switch(mSelect) {
	case LEFT :
	  if(mode == PAGE1) mode = PAGE2;
	  else if(mode < PAGE2 + alert_pages) mode++;
	  break;
	case RIGHT :
	  if(mode > PAGE1 && mode < ICONTEST) mode--;
	  else mode = PAGE1;
	  break;
	case UP :
	  break;
	case DOWN :
	  goto ExitWeather;
	case CWCIRCLE :
	  break;
	case CCWCIRCLE :
	  mode = ICONTEST;
	  break;
      }
      my_idle();
    }
  }
ExitWeather:
  while (ttgo->getTouch(x, y)) { // wait until user lifts finger to exit
    my_idle();
  }
  connected = false;
  WiFi.mode(WIFI_OFF);
  tft->fillScreen(TFT_BLACK); // Clear screen
}
