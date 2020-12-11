// connect to wifi (stored in "file" acc_pts.txt (in SPIFF)), and then
// do MQTT stuff

#include "config.h"
#include "DudleyWatch.h"
#include <time.h>
// #include "esp_wifi.h"
#include <WiFi.h>
// #include <WiFiUdp.h>
#include <Wire.h>
#include <Ethernet.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>
#include <AceTime.h>

#include "my_WiFi.h"
#define __MQTT_MAIN__
#include "appMQTT.h"

WiFiClient espClient;
PubSubClient mqttClient(espClient);
void MQTTcallback(char*, byte*, unsigned int);
void mqtt_reconnect (void);

IPAddress MQTTserver;

#define DBGMQTT 1
#define verbose true

#define MAX_MON_TOPICS 20
#define MAX_TOPIC_WIDTH 250
static char topics_to_monitor[MAX_MON_TOPICS][MAX_TOPIC_WIDTH];
static int next_topic_insert;
static struct menu_item *current_menu;
#define MAX_DATA_WIDTH 20
struct mqdata {
  char value[MAX_DATA_WIDTH];
};
static struct mqdata sensor_menu1_data[12];
static struct mqdata sensor_menu2_data[12];
static struct mqdata ctrl_menu1_data[12];
static struct mqdata ctrl_menu2_data[12];

void flash_menu_item (uint8_t ncols, struct menu_item *mqmenu_p, uint8_t font, uint8_t bigfont, bool leave_room_for_label, int row, int col, struct mqdata *menu_data_p, boolean flash, uint8_t ptr_null_grey) {
uint8_t yvals[4], yh;
uint16_t icolor;
  if(leave_room_for_label) {
    yvals[0] = 35;
    yvals[1] = 85;
    yvals[2] = 135;
    yvals[3] = 185;
    yh = 50;
  }
  else {
    yvals[0] =   0;
    yvals[1] =  60;
    yvals[2] = 120;
    yvals[3] = 180;
    yh = 60;
  }
  int16_t bwidth;
  int16_t xvals[3] = { 0, 83, 164 };
  int16_t xtvals[3] = { 41, 120, 200 };
  if(ncols > 3) { ncols = 3; }
  if(ncols == 3) {
    bwidth = 75;
  }
  else if(ncols == 2) {
    xvals[1] = 123;
    xtvals[0] = 61;
    xtvals[1] = 184;
    bwidth = 115;
  }
  else if(ncols == 1) {
    xtvals[0] = 120;
    bwidth = 239;
  }
  // note: space at the top do display what is typed
  // was tft->fillRect(0, 35, 80, 50, TFT_BLUE);
  // number keys are 80 x 50, four rows of three
  // x=0, 81, 161, y=35, 85, 135, 185
  tft->setTextColor(TFT_GREEN);
  int ino = col + (row * ncols);
  if(flash) {
    icolor = TFT_LIGHTGREY;
    tft->fillRoundRect(xvals[col], yvals[row], bwidth, yh-5, 6, icolor);
    tft->drawCentreString( mqmenu_p[ino].name, xtvals[col], yvals[row]+5, font);
    delay(100);
  }
  icolor = (!strlen(mqmenu_p[ino].name)) ? TFT_BLACK
      : ((ptr_null_grey ^ (mqmenu_p[ino].next_menu != NULL))
      ||!strcmp(mqmenu_p[ino].name, "Done")
      ||!strcmp(mqmenu_p[ino].name, "Exit")) ? TFT_DARKGREY
	  : TFT_BLUE ;
  tft->fillRoundRect(xvals[col], yvals[row], bwidth, yh-5, 6, icolor);
  tft->drawCentreString( mqmenu_p[ino].name, xtvals[col], yvals[row]+5, font);
  if(menu_data_p) {
    tft->drawCentreString( menu_data_p[ino].value, xtvals[col], yvals[row]+20, (strlen(menu_data_p[ino].value) < 10) ? bigfont : font);
  }
}

void draw_button_menu (uint8_t ncols, struct menu_item *mqmenu_p, uint8_t font, uint8_t bigfont, bool leave_room_for_label, char *top_label, struct mqdata *menu_data_p, uint8_t ptr_null_grey) {
uint8_t yvals[4], yh, row, col;
uint16_t icolor;
  tft->fillScreen(TFT_BLACK);
  if(leave_room_for_label) {
    yvals[0] = 35;
    yvals[1] = 85;
    yvals[2] = 135;
    yvals[3] = 185;
    yh = 50;
    if(top_label) {
      tft->setTextColor(TFT_GREEN);
      tft->drawCentreString( top_label, half_width, 5, font);
    }
  }
  else {
    yvals[0] =   0;
    yvals[1] =  60;
    yvals[2] = 120;
    yvals[3] = 180;
    yh = 60;
  }
  int16_t bwidth;
  int16_t xvals[3] = { 0, 83, 164 };
  int16_t xtvals[3] = { 41, 120, 200 };
  if(ncols > 3) { ncols = 3; }
  if(ncols == 3) {
    bwidth = 75;
  }
  else if(ncols == 2) {
    xvals[1] = 123;
    xtvals[0] = 61;
    xtvals[1] = 184;
    bwidth = 115;
  }
  else if(ncols == 1) {
    xtvals[0] = 120;
    bwidth = 239;
  }
  // note: space at the top do display what is typed
  // was tft->fillRect(0, 35, 80, 50, TFT_BLUE);
  // number keys are 80 x 50, four rows of three
  // x=0, 81, 161, y=35, 85, 135, 185
  tft->setTextColor(TFT_GREEN);
  if(ncols > 3) { ncols = 3; }
  for(row = 0 ; row < 4 ; row++) {
    for(col = 0 ; col < ncols ; col++) {
      int ino = col + (row * ncols);
      icolor = (!strlen(mqmenu_p[ino].name)) ? TFT_BLACK
	  : ((ptr_null_grey ^ (mqmenu_p[ino].next_menu != NULL))
	  ||!strcmp(mqmenu_p[ino].name, "Done")
	  ||!strcmp(mqmenu_p[ino].name, "Exit")) ? TFT_DARKGREY
	      : TFT_BLUE ;
      tft->fillRoundRect(xvals[col], yvals[row], bwidth, yh-5, 6, icolor);
      tft->drawCentreString( mqmenu_p[ino].name, xtvals[col], yvals[row]+5, font);
      if(menu_data_p) {
	  tft->drawCentreString( menu_data_p[ino].value, xtvals[col], yvals[row]+20, (strlen(menu_data_p[ino].value) < 10) ? bigfont : font);
      }
    }
  }
}

#define DBG_GESTURES 0
int poll_swipe_or_menu_press(int num_choices) {
uint32_t lasttouch, interval;
int16_t nx, ny, x, y, x0, y0, amax, points;
enum SWIPE_DIR resdir;
  resdir = NODIR;
  if(num_choices < 14) { num_choices = 12; }
  else { num_choices = 16; }
  int xdir = 0;
  int ydir = 0;
  int xdif = 0;
  int ydif = 0;
  x0 = -1;
  y0 = -1;
  char dir;
  int16_t max_lrud[4] = {1000, -1, 1000, -1};
  int16_t pts_lrud[4] = {-1, -1, -1, -1};
  points = 0;
  lasttouch = millis();
  do {
    interval = millis() - lasttouch;
    // Serial.printf("(1) interval = %lu\n", interval);
    if(ttgo->getTouch(nx, ny)) {
      // Serial.print(F("T"));
      if((nx > 0) && (ny > 0) && (nx < 240) && (ny < 240)) {
	// Serial.println(F("G"));
	x = nx; y = ny;
	// remember the first touch:
	if(x0 < 0) { x0 = x; }
	if(y0 < 0) { y0 = y; }
	int i = points;
	if(pts_lrud[0] < 0 || x < max_lrud[0]) { max_lrud[0] = x; pts_lrud[0] = i++; }
	if(pts_lrud[1] < 0 || x > max_lrud[1]) { max_lrud[1] = x; pts_lrud[1] = i++; }
	if(pts_lrud[2] < 0 || y < max_lrud[2]) { max_lrud[2] = y; pts_lrud[2] = i++; }
	if(pts_lrud[3] < 0 || y > max_lrud[3]) { max_lrud[3] = y; pts_lrud[3] = i++; }
	points++;
      }
      lasttouch = millis();
    }
  } while(interval < 100);
#if DBG_GESTURES
  if(points) {
    Serial.printf("i = %d, p = %d\n", interval, points);
    Serial.printf("max_lrud[] = %d, %d, %d, %d\n", max_lrud[0], max_lrud[1], max_lrud[2], max_lrud[3]);
    Serial.printf("pts_lrud[] = %d, %d, %d, %d\n", pts_lrud[0], pts_lrud[1], pts_lrud[2], pts_lrud[3]);
  }
#endif
  if(x0 >= 0 && y0 >= 0) {
    // Serial.printf("(2) interval = %lu\n", interval);
    xdir = x - x0;	// x extent of swipe.  near zero if a circle gesture
    ydir = y - y0;	// y extent of swipe.  near zero if a circle gesture
    xdif = max_lrud[1] - max_lrud[0];	// always a positive difference
    ydif = max_lrud[3] - max_lrud[2];	// always a positive difference
    amax = (xdif > ydif) ? xdif : ydif ;
#if DBG_GESTURES
    Serial.printf("x0 = %d, x = %d, xdif = %d\n", x0, x, xdif);
    Serial.printf("y0 = %d, y = %d, ydif = %d, amax = %d\n", y0, y, ydif, amax);
#endif
    if(points > 100 && xdif > 60 && ydif > 60) {	// is gesture a circle ?
#if DBG_GESTURES
      int8_t max_order[4] = {-1, -1, -1, -1};
#endif
      int16_t min_value[4] = {1000, 1000, 1000, 1000};
      char lut[4] = { 'L', 'R', 'U', 'D' };
      char dir_order[9];
      dir_order[8] = '\0';
#if DBG_GESTURES
      Serial.printf("maybe a circle\n");
#endif
      // CW  circle -> u, r, d, l (starting at any point in the list)
      // CCW circle -> u, l, d, r (starting at any point in the list)
      // so if right after up and left after down, then CW
      // so if left after up and right after down, then CCW
      // else not a circle
      // order the max_lrud points
      int8_t min_index;
      for(int8_t j = 0 ; j < 4 ; j++) {
	for(int8_t i = 0 ; i < 4 ; i++) {
	  if(pts_lrud[i] < min_value[j] && (j == 0 || pts_lrud[i] > min_value[j-1])) {
	    min_value[j] = pts_lrud[i];
	    min_index = i;
	  }
	}
#if DBG_GESTURES
	max_order[j] = min_index;
#endif
	dir_order[j] = lut[min_index];
	dir_order[j+4] = lut[min_index];
      }
#if DBG_GESTURES
      Serial.printf("max_order[4] = %d, %d, %d, %d\n", max_order[0], max_order[1], max_order[2], max_order[3]);
      Serial.printf("dir_order = %s\n", dir_order);
#endif
      if(strstr(dir_order, "URDL")) {
#if DBG_GESTURES
	Serial.println(F("found CW CIRCLE"));
#endif
	return(CWCIRCLE);
      }
      if(strstr(dir_order, "ULDR")) {
#if DBG_GESTURES
	Serial.println(F("found CCW CIRCLE"));
#endif
	return(CCWCIRCLE);
      }
    }
    else if(amax > 60) { // moved across 1/4 of the screen, so probably a swipe
      dir = (ydif > xdif) ? 'y' : 'x' ;
#if DBG_GESTURES
      Serial.printf("amax = %d, points = %d, probably a swipe\n", amax, points);
      Serial.printf("xdir = %d, ydir = %d\n", xdir, ydir);
      Serial.printf("xdif = %d, ydif = %d, dir = %c\n", xdif, ydif, dir);
#endif
      resdir = 
	(dir == 'x') ? ((xdir > 0) ? RIGHT : LEFT ) :
			(ydir > 0) ? DOWN : UP;
      return resdir;
    }
    else {	// must be button press
      if(num_choices == 12) {
	if (y < 85) {
	  if (x < 80) return 0;
	  else if (x > 160) return 2;
	  else return 1;
	}
	else if (y < 135) {
	  if (x < 80) return 3;
	  else if (x > 160) return 5;
	  else return 4;
	}
	else if (y < 185) {
	  if (x < 80) return 6;
	  else if (x > 160) return 8;
	  else return 7;
	}
	else if (x < 80) return 9;
	else if (x > 160) return 11;
	else return 10;
      }
      else {	// must be 16 choices
	if (y < 85) {
	  if (x < 60) return 0;
	  else if (x < 120) return 1;
	  else if (x < 180) return 2;
	  else return 3;
	}
	else if (y < 135) {
	  if (x < 60) return 4;
	  else if (x < 120) return 5;
	  else if (x < 180) return 6;
	  else return 7;
	}
	else if (y < 185) {
	  if (x < 60) return 8;
	  else if (x < 120) return 9;
	  else if (x < 180) return 10;
	  else return 11;
	}
	if (x < 60) return 12;
	else if (x < 120) return 13;
	else if (x < 180) return 14;
	else return 15;
      }
    }
  }
  return -1;
}

void store_mqtt_data (char * Stopic, char * suffix, const char * svalue) {
int row, col;
char stopic[MAX_TOPIC_WIDTH];

  if(!svalue[0]) { return; }
  if(suffix[0]) {
    sprintf(stopic, "%s$.%s", Stopic, suffix);
    // Serial.printf("store_mqtt_data(%s, %30s) line %d\n", stopic, svalue, __LINE__);
  }
  else {
    strncpy(stopic, Stopic, MAX_TOPIC_WIDTH);
  }
  for(int j = 0 ; j < 12 ; j++) {
    row = j / 3 ; col = j % 3 ;
  // Serial.printf("store_mqtt_data(%s, %30s) line %d, j = %d\n", stopic, svalue, __LINE__, j);
    if(sensor_menu1[j].topic && !strncmp(sensor_menu1[j].topic, stopic, MAX_TOPIC_WIDTH)) {
      strncpy(sensor_menu1_data[j].value, svalue, MAX_DATA_WIDTH);
      if(current_menu == sensor_menu1) {
	// Serial.printf("%s %s\n", stopic, svalue);
	flash_menu_item(3, current_menu, 1, 2, true, row, col, sensor_menu1_data, false, 0);
      }
    }
    else if(sensor_menu2[j].topic && !strncmp(sensor_menu2[j].topic, stopic, MAX_TOPIC_WIDTH)) {
      strncpy(sensor_menu2_data[j].value, svalue, MAX_DATA_WIDTH);
      if(current_menu == sensor_menu2) {
	// Serial.printf("%s %s\n", stopic, svalue);
	flash_menu_item(3, current_menu, 1, 2, true, row, col, sensor_menu2_data, false, 0);
      }
    }
    else if(ctrl_menu1[j].topic && !strncmp(ctrl_menu1[j].topic, stopic, MAX_TOPIC_WIDTH)) {
      strncpy(ctrl_menu1_data[j].value, svalue, MAX_DATA_WIDTH);
      if(current_menu == ctrl_menu1) {
	// Serial.printf("%s %s\n", stopic, svalue);
	flash_menu_item(3, current_menu, 1, 2, true, row, col, ctrl_menu1_data, false, 0);
      }
    }
    else if(ctrl_menu2[j].topic && !strncmp(ctrl_menu2[j].topic, stopic, MAX_TOPIC_WIDTH)) {
      strncpy(ctrl_menu2_data[j].value, svalue, MAX_DATA_WIDTH);
      if(current_menu == ctrl_menu2) {
	// Serial.printf("%s %s\n", stopic, svalue);
	flash_menu_item(3, current_menu, 1, 2, true, row, col, ctrl_menu2_data, false, 0);
      }
    }
  }
  // Serial.println(F("leaving store_mqtt_data()"));
}

void appMQTT(void) {
uint32_t this_sec;
int err, ecnt;
int16_t x, y;
struct mqdata *menu_data_p;
struct menu_item *last_current_menu;
    // patch up the menus, since I can't get forward references to work
    sensor_menu1[9].next_menu = (struct menu_item *)sensor_menu2;
    sensor_menu1[10].next_menu = (struct menu_item *)ctrl_menu1;

    sensor_menu2[10].next_menu = (struct menu_item *)ctrl_menu1;

    ctrl_menu1[10].next_menu = (struct menu_item *)ctrl_menu2;

    next_topic_insert = 0;
    memset(topics_to_monitor, '\0', sizeof(topics_to_monitor)/sizeof(char));
    MQTTserver[0] = general_config.mqtt_server[0];
    MQTTserver[1] = general_config.mqtt_server[1];
    MQTTserver[2] = general_config.mqtt_server[2];
    MQTTserver[3] = general_config.mqtt_server[3];

    // pick what to monitor/control:
struct menu_item *this_pick;
char *menu_label;
int mSelect;
enum SWIPE_DIR swipe;
    if(1) {	// this connects to wifi, MQTT server, and runs MQTTcallback
      if(connect_to_wifi(verbose, &BestAP, true, true) && verbose) {
	  Serial.printf("connect to wifi failed\n");
	  tft->setTextColor(TFT_YELLOW, TFT_BLACK);
	  tft->drawString("Connect to WiFi Failed!",  0, 5, 2);
	  delay(5000);
	  return;
      }
      ecnt = 0;
      while(!connected && ecnt < 400) {
#if DBGMQTT
	Serial.println(F("waiting 0.2 seconds for wifi connection"));
#endif
	ecnt++;
	delay(200);
      }
      if(ecnt == 40) {
#if DBGMQTT
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
	tft->setCursor(0, 50 + (15 * 8));
	tft->printf("Connected to %s channel %d", BestAP.ssid, BestAP.channel);
	tft->setCursor(0, 50 + (15 * 9));
	tft->print(F("Trying to connect to "));
	tft->print(MQTTserver);
	tft->setCursor(0, 50 + (15 * 10));
	tft->print(F("my ip is: "));
	tft->print(WiFi.localIP());
      }
      Serial.print(F("Trying to connect to "));
      Serial.print(MQTTserver);
      Serial.printf(" on port %d.\n", general_config.mqtt_port);
      Serial.printf("mqtt user = %s, passwd = %s\n",
	general_config.mqtt_user,
	general_config.mqtt_pass);
      mqttClient.setServer(MQTTserver, general_config.mqtt_port);
      mqttClient.setCallback(MQTTcallback);

      mqtt_reconnect();
      if(verbose) {
	delay(2000);
      }
    }
    current_menu = sensor_menu1;
    this_sec = millis();
    while(this_sec + 30000 > millis()) {
Top:  if(current_menu == sensor_menu1) {
	menu_label = "sens 2 <-- sensors page 1 ^ ctrl 1";
	menu_data_p = sensor_menu1_data;
      }
      if(current_menu == sensor_menu2) {
	menu_label = "ctrl 1 ^ sensors page 2 --> sens 1";
	menu_data_p = sensor_menu2_data;
      }
      if(current_menu == ctrl_menu1) {
	menu_label = "ctrl 2 V control page 1 --> sens 1";
	menu_data_p = ctrl_menu1_data;
      }
      if(current_menu == ctrl_menu2) {
	menu_label = "ctrl 1 ^ control page 2 --> sens 1";
	menu_data_p = ctrl_menu2_data;
      }
      last_current_menu = current_menu;
      // Serial.println(F("before draw_button_menu()"));
      draw_button_menu(3, current_menu, 1, 2, true, menu_label, menu_data_p, 0);
      while(1) {
	// Serial.println(F("before poll_swipe_or_menu_press(12)"));
	mSelect = poll_swipe_or_menu_press(12);	// poll for touch or gesture
	if (mSelect > -1 && mSelect < NODIR) {	// if user touched something
	  int col = mSelect % 3;
	  int row = mSelect / 3;
	  flash_menu_item(3, current_menu, 1, 2, true, row, col, menu_data_p, true, 0);
	  if(current_menu == sensor_menu1) { this_pick = &sensor_menu1[mSelect]; }
	  if(current_menu == sensor_menu2) { this_pick = &sensor_menu2[mSelect]; }
	  if(current_menu == ctrl_menu1) { this_pick = &ctrl_menu1[mSelect]; }
	  if(current_menu == ctrl_menu2) { this_pick = &ctrl_menu2[mSelect]; }
	  if(!strcmp(this_pick->name, "Exit")) {
	    Serial.printf("user chose %s\n", this_pick->name);
	    goto Exit;
	  }
	  if(this_pick->next_menu) {
	    current_menu = (struct menu_item *)this_pick->next_menu;
	    Serial.printf("user chose menu %s\n", this_pick->name);
	    goto Top;
	  }
	  else if(this_pick->topic) {	// if cmnd/foo then do foo
	    // so if it's in a "ctrl" menu, then send MQTT command
	    if(current_menu == ctrl_menu1
	    || current_menu == ctrl_menu2) {
	      Serial.printf("user touched %s\n", this_pick->topic);
	      strcpy(buff, this_pick->topic);
	      memcpy(buff, "cmnd", 4);
	      char *cp;
	      char sfx[2];
	      char newvalue[5];
	      sfx[1] = '\0';
	      cp = strstr(buff, "STATE$.POWER4");
	      sfx[0] = '4';
	      if(!cp) {
		cp = strstr(buff, "STATE$.POWER3");
		sfx[0] = '3';
	      }
	      if(!cp) {
		cp = strstr(buff, "STATE$.POWER2");
		sfx[0] = '2';
	      }
	      if(!cp) {
		cp = strstr(buff, "STATE$.POWER1");
		sfx[0] = '1';
	      }
	      if(!cp) {
		cp = strstr(buff, "STATE$.POWER");
		sfx[0] = '\0';	// "erase" the numerical suffix
	      }
	      strcpy(cp, "power");
	      if(sfx[0]) { strcat(buff, sfx); }
	      Serial.printf("command topic %s, mSelect = %d, value was %s\n",
		buff, mSelect,
		(menu_data_p[mSelect].value[0]) ? menu_data_p[mSelect].value : "nil");
	      strcpy(newvalue, (menu_data_p[mSelect].value[1] == 'N'
	      || menu_data_p[mSelect].value[1] == 'n') ? "off" : "on");
	      Serial.printf("command: topic %s = %s\n", buff, newvalue);
	      mqttClient.publish(buff, newvalue);
	      strcpy(menu_data_p[mSelect].value, newvalue);
	      flash_menu_item(3, current_menu, 1, 2, true, row, col, menu_data_p, false, 1);
	    }
	  }
	}
	else if(mSelect > NODIR) {
	  Serial.printf("swipe %s, ", swipe_names[mSelect - (int)NODIR]);
	  char *mname;
	  switch(mSelect) {
	    case UP :
	      current_menu = ctrl_menu1;
	      mname = "ctrl_menu1";
	      break;
	    case DOWN :
	      current_menu = ctrl_menu2;
	      mname = "ctrl_menu2";
	      break;
	    case LEFT :	// show the screen to the right
	      current_menu = sensor_menu2;
	      mname = "sensor_menu2";
	      break;
	    case RIGHT :	// show the screen to the left
	      current_menu = sensor_menu1;
	      mname = "sensor_menu1";
	      break;
	  }
	  Serial.printf("user chose menu %s\n", mname);
	  if(last_current_menu != current_menu) {
	      goto Top;
	  }
	  break;
	}
	my_idle();
	if (mqttClient.connected()) {
	  mqttClient.loop();	// check for messages
	}
	else {
	  mqtt_reconnect();
	}
      }
    }
Exit:
    while (ttgo->getTouch(x, y)) {	// Wait for release
      my_idle();
    }
    Serial.printf("exit appMQTT()\n");
    tft->fillScreen(TFT_BLACK);
    connected = false;
    WiFi.mode(WIFI_OFF);
}

void mqtt_reconnect() {
uint8_t errcnt;
    // Loop until we're reconnected
    errcnt = 0;
    while (!mqttClient.connected() && errcnt < 10) {
	DLF("Connect MQTT ... ");
	// Attempt to connect
	if (mqttClient.connect("wristwatch", general_config.mqtt_user, general_config.mqtt_pass))
	{
	    DLFn("OK");
	    // Once connected, publish an announcement...
	    // ... and resubscribe
	    mqttClient.subscribe("#", 1);
	} else {
	    DVFn("failed :", mqttClient.state());
	    DLFn(" retry in 5 seconds");
	    // Wait 2 seconds before retrying
	    delay(2000);
	    errcnt++;
	}
    }
}

#define MAX_PAYLOAD 2001
void MQTTcallback(char* topic, byte* payload, unsigned int length) {
const size_t capacity = JSON_OBJECT_SIZE(4) + JSON_OBJECT_SIZE(8) + 160;
// const size_t capacity = 2*JSON_OBJECT_SIZE(5) + 130;
// const size_t capacity = JSON_OBJECT_SIZE(2) + JSON_OBJECT_SIZE(3) + 80;
static char json[MAX_PAYLOAD];	// static to move it off the stack
static char jsoncopy[MAX_PAYLOAD];	// static to move it off the stack
char longtopic[MAX_TOPIC_WIDTH];
int i;
char svalue[100];
boolean is_power, is_power1, is_power2, is_power3, is_power4, is_am2301;
boolean is_ds18s20, is_ds18b20;
boolean found_powerN = false;

  is_power  = false;
  is_power1 = false;
  is_power2 = false;
  is_power3 = false;
  is_power4 = false;
  is_am2301 = false;
  is_ds18b20 = false;
  is_ds18s20 = false;
  if(strstr(topic, "tele/emon/")) { return; }
  strncpy(longtopic, topic, MAX_TOPIC_WIDTH);
  // Serial.printf("MQTTcallback(%s, payload) line %d\n", longtopic, __LINE__);
  for (i=0;i<length && i < MAX_PAYLOAD - 1;i++) {
    json[i] = payload[i];
    jsoncopy[i] = payload[i];
  }
  json[i] = '\0';
  jsoncopy[i] = '\0';
  if(strstr(json, "AM2301")) {
    is_am2301 = true;
  }
  if(strstr(json, "DS18B20")) {
    is_ds18b20 = true;
  }
  if(strstr(json, "DS18S20")) {
    is_ds18s20 = true;
  }
  if(strstr(json, "POWER1")) {
    is_power1 = true;
    found_powerN = true;
  }
  if(strstr(json, "POWER2")) {
    is_power2 = true;
    found_powerN = true;
  }
  if(strstr(json, "POWER3")) {
    is_power3 = true;
    found_powerN = true;
  }
  if(strstr(json, "POWER4")) {
    is_power4 = true;
    found_powerN = true;
  }
  if(!found_powerN && strstr(json, "POWER")) {
    is_power = true;
  }
  // Serial.printf("MQTTcallback(%s, %30s) line %d\n", longtopic, json, __LINE__);

  if(strchr(json, '{')) /* } */ {	// if payload is JSON
    DynamicJsonDocument doc(capacity);
    DeserializationError err = deserializeJson(doc, json);
    if (err) {
      Serial.print(F("deserializeJson() failed with code "));
      Serial.println(err.c_str());
      return;
    }

    if(strstr(longtopic, "STATE")) {
      // if there is no data for some value, then we get null/0.0/0/"" as appropriate
#if WE_NEED_THIS
      if(strstr(jsoncopy, "Time")) {
	const char* Time = doc["Time"]; // "2020-09-06T03:16:42"
	store_mqtt_data(longtopic, "Time", Time);
      }
      if(strstr(jsoncopy, "Uptime")) {
	const char* Uptime = doc["Uptime"]; // "2T11:08:41"
	store_mqtt_data(longtopic, "Uptime", Uptime);
      }
      if(strstr(jsoncopy, "Vcc")) {
	float Vcc = doc["Vcc"]; // 3.402
	sprintf(svalue, "%.2f", Vcc);
	store_mqtt_data(longtopic, "Vcc", svalue);
      }
#endif
      if(is_power1) {
	const char* POWER1 = doc["POWER1"]; // "ON"
	store_mqtt_data(longtopic, "POWER1", POWER1);
      }
      if(is_power2) {
	const char* POWER2 = doc["POWER2"]; // "OFF"
	store_mqtt_data(longtopic, "POWER2", POWER2);
      }
      if(is_power3) {
	const char* POWER3 = doc["POWER3"]; // "OFF"
	store_mqtt_data(longtopic, "POWER3", POWER3);
      }
      if(is_power4) {
	const char* POWER4 = doc["POWER4"]; // "ON"
	store_mqtt_data(longtopic, "POWER4", POWER4);
      }
      if(is_power) {
	const char* POWER = doc["POWER"]; // "ON"
	store_mqtt_data(longtopic, "POWER", POWER);
      }

      JsonObject Wifi = doc["Wifi"];

      if(strstr(jsoncopy, "WiFi")) {
	int Wifi_AP = Wifi["AP"]; // 1
	sprintf(svalue, "%d", Wifi_AP);
	store_mqtt_data(longtopic, "Wifi.AP", svalue);
	const char* Wifi_SSId = Wifi["SSId"]; // "D-link"
	store_mqtt_data(longtopic, "Wifi.SSId", Wifi_SSId);
	const char* Wifi_BSSId = Wifi["BSSId"]; // "00:26:75:96:A8:58"
	store_mqtt_data(longtopic, "Wifi.BSSId", Wifi_BSSId);
	int Wifi_Channel = Wifi["Channel"]; // 4
	sprintf(svalue, "%d", Wifi_Channel);
	store_mqtt_data(longtopic, "Wifi.Channel", svalue);
	int Wifi_RSSI = Wifi["RSSI"]; // 100
	sprintf(svalue, "%d", Wifi_RSSI);
	store_mqtt_data(longtopic, "Wifi.RSSI", svalue);
	const char* Wifi_APMac = Wifi["APMac"]; // "74:DA:38:6D:73:88"
	store_mqtt_data(longtopic, "Wifi.APMac", Wifi_APMac);
      }
    }

    if(strstr(longtopic, "SENSOR")) {
      if(is_am2301) {
	float AM2301_Temperature = doc["AM2301"]["Temperature"]; // 75.2
	// Serial.printf("AM2301_Temperature = %.1f\n", AM2301_Temperature);
	sprintf(svalue, "%.1f", AM2301_Temperature);
	store_mqtt_data(longtopic, "AM2301.Temperature", svalue);
	float AM2301_Humidity = doc["AM2301"]["Humidity"]; // 77.7
	sprintf(svalue, "%.1f", AM2301_Humidity);
	store_mqtt_data(longtopic, "AM2301.Humidity", svalue);
	const char* TempUnit = doc["TempUnit"]; // "F"
	store_mqtt_data(longtopic, "TempUnit", TempUnit);
      }
      if(is_ds18b20) {
	float DS18B20_Temperature = doc["DS18B20"]["Temperature"]; // 75.2
	// Serial.printf("DS18B20_Temperature = %.1f\n", DS18B20_Temperature);
	sprintf(svalue, "%.1f", DS18B20_Temperature);
	store_mqtt_data(longtopic, "DS18B20.Temperature", svalue);
	const char* TempUnit = doc["TempUnit"]; // "F"
	store_mqtt_data(longtopic, "TempUnit", TempUnit);
      }
      if(is_ds18s20) {
	float DS18S20_Temperature = doc["DS18S20"]["Temperature"]; // 75.2
	// Serial.printf("DS18S20_Temperature = %.1f\n", DS18S20_Temperature);
	sprintf(svalue, "%.1f", DS18S20_Temperature);
	store_mqtt_data(longtopic, "DS18S20.Temperature", svalue);
	const char* TempUnit = doc["TempUnit"]; // "F"
	store_mqtt_data(longtopic, "TempUnit", TempUnit);
      }
    }
  }
  else {
    store_mqtt_data(longtopic, "", json);
  }
}
