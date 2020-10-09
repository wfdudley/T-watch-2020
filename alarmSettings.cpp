// Settings for the alarm clock function
// W.F.Dudley Jr.

#include "config.h"
#include "DudleyWatch.h"
#include <math.h>
#include "my_tz.h"

static void page1_create(lv_obj_t * parent);
static void page2_create(lv_obj_t * parent);
static void page3_create(lv_obj_t * parent);
#if NEEDED
static void page4_create(lv_obj_t * parent);
#endif
static enum LV_THING event_result;
static int16_t event_value;
static uint8_t slider_num;
static uint8_t button_num;
static uint8_t dropdown_num;

static lv_obj_t *btn1, *btn2, *slider1, *slider2;
static lv_obj_t *slider1_label, *slider2_label;
static lv_obj_t *slider1_name, *slider2_name, *b2label;


void enable_rtc_alarm(void) {
struct tm timeinfo;
uint8_t hh, mm;
int8_t ahh, amm;
time_t utc_time, unix_time;
  Serial.println(F("enable_rtc_alarm()"));
  Serial_timestamp();
  ttgo->rtc->disableAlarm();
  // get alarm time from general_config.alarm_h and alarm_m
  tnow = ttgo->rtc->getDateTime();
  hh = tnow.hour;
  mm = tnow.minute;
#if SIMPLE_TEST_VERSION
  // schedule the alarm for a minute or two in the future
  amm = mm + 1 + (tnow.second > 30);
  if(amm > 59) {
    amm %= 60;
    ahh = hh + 1;
    ahh %= 24;
  }
#elsif HACKY_VERSION
  // This is hacky but I don't know a better way to convert local time
  // to UTC time.  I keep UTC in the rtc chip so that we can display
  // the time in any timezone.  If you travel, you only need to change
  // the timezone to keep the watch displaying correct local time.
  timeinfo.tm_hour = tnow.hour;
  timeinfo.tm_min  = tnow.minute;
  timeinfo.tm_sec  = tnow.second;
  timeinfo.tm_mday = tnow.day;
  timeinfo.tm_mon  = tnow.month - 1;
  timeinfo.tm_year = tnow.year - 1900;
  // now timeinfo contains GMT/UTC time
  utc_time = mktime(&timeinfo);
  unix_time = timelocal(utc_time);
  memcpy(&timeinfo, localtime(&unix_time), sizeof(struct tm)/sizeof(char));
  // now timeinfo contains local time
  Serial.print(F("NOW, LOCAL timezone: "));
  Serial.println(&timeinfo, "%A, %B %d %Y %H:%M:%S");
  int8_t hh_offset = timeinfo.tm_hour - hh;	// local - utc = hh_offset
  int8_t mm_offset = timeinfo.tm_min  - mm;	// local - utc = mm_offset
  // local - utc = mm_o
  // mm_o - local = - utc
  // local - mm_o = utc
  Serial.printf("offset to convert local to UTC: %d hours, %d minutes\n", hh_offset, mm_offset);
  // convert alarm time to UTC
  ahh = general_config.alarm_h - hh_offset;
  amm = general_config.alarm_m - mm_offset;
  if(amm < 0) {
    amm += 60;
    ahh --;
  }
  if(amm > 59) {
    amm %= 60;
    ahh ++;
  }
  if(ahh < 0) {
    ahh += 24;
  }
  Serial.printf("alarm time as UTC = %d:%02d\n", ahh, amm);
#if 0
  Serial.print(F("alarm time, LOCAL timezone: "));
  Serial.println(&timeinfo, "%A, %B %d %Y %H:%M:%S");
#endif
#else
  convert_alarm_time_to_gmt(&ahh, &amm);
#endif
  // put UTC alarm time in the rtc alarm registers
  Serial.printf("setting alarm for %d:%02d\n", ahh, amm);
  ttgo->rtc->setAlarm(ahh, amm, PCF8563_NO_ALARM, PCF8563_NO_ALARM);
  ttgo->rtc->enableAlarm();
}

void disable_rtc_alarm(void) {
    ttgo->rtc->disableAlarm();
    ttgo->rtc->resetAlarm();
    Serial.println(F("disable_rtc_alarm()"));
}

static void button_handler(lv_obj_t *obj, lv_event_t event) {
    // Serial.printf("button_handler() event = %d\n", (int)event);
    if (event == LV_EVENT_CLICKED && !event_result) {
        Serial.println(F("Clicked"));
	event_result = BUTTON;
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
	event_result = BUTTON;
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

static void slider_handler(lv_obj_t *obj, lv_event_t event) {
static char buf[4];	// max 3 bytes for number plus 1 null
    // Serial.printf("slider_handler() event = %d\n", (int)event);
    if(event == LV_EVENT_VALUE_CHANGED && !event_result) {
	event_result = SLIDER;
	event_value = lv_slider_get_value(obj);
    }
}

static void slider1_handler(lv_obj_t *obj, lv_event_t event) {
static char buf[4];	// max 3 bytes for number plus 1 null
  slider_handler(obj, event);
  if(event_result == SLIDER) {
    slider_num = 1;
    event_value = lv_slider_get_value(obj);
    snprintf(buf, 4, "%u", event_value);
    lv_label_set_text(slider1_label, buf);
    Serial.printf("handler slider_num = %d, label = %s\n", slider_num, buf);
  }
}

#if NEEDED
static void slider2_handler(lv_obj_t *obj, lv_event_t event) {
static char buf[4];	// max 3 bytes for number plus 1 null
  slider_handler(obj, event);
  if(event_result) {
    slider_num = 2;
    event_value = lv_slider_get_value(obj);
    snprintf(buf, 4, "%u", event_value);
    lv_label_set_text(slider2_label, buf);
    Serial.printf("handler slider_num = %d, label %s\n", slider_num, buf);
  }
}
#endif

#if NEEDED
static void kb_event_cb1(lv_obj_t * ta, lv_event_t e);
static void kb_event_cb2(lv_obj_t * ta, lv_event_t e);
static void kb_event_cb3(lv_obj_t * ta, lv_event_t e);
static void kb_event_cb4(lv_obj_t * ta, lv_event_t e);
static lv_obj_t * ta1;	// MQTT server IP text area
static lv_obj_t * ta2;	// MQTT port text area
static lv_obj_t * ta3;	// MQTT user text area
static lv_obj_t * ta4;	// MQTT password text area
#endif
static lv_obj_t * tv;
static lv_obj_t * t1;
static lv_obj_t * t2;
static lv_obj_t * t3;
static lv_obj_t * t4;
static lv_obj_t * kb;
static lv_style_t style_box;	// is this useful ?

#if NEEDED
static void ta_event_cb1(lv_obj_t * ta, lv_event_t e) {
    if(e == LV_EVENT_RELEASED) {
        if(kb == NULL) {	// create a keyboard if there isn't one.
	    kb = lv_keyboard_create(lv_scr_act(), NULL);
	    lv_obj_set_size(kb,  LV_HOR_RES, LV_VER_RES / 2);
	    lv_obj_align(kb, NULL, LV_ALIGN_CENTER,  0,  60);
	    lv_keyboard_set_mode(kb, LV_KEYBOARD_MODE_NUM);
	    lv_keyboard_set_textarea(kb, ta);
            lv_obj_set_event_cb(kb, kb_event_cb1);
            lv_indev_wait_release(lv_indev_get_act());
        }
        lv_textarea_set_cursor_hidden(ta, false);
        // lv_page_focus(t1, lv_textarea_get_label(ta), LV_ANIM_ON);
        lv_keyboard_set_textarea(kb, ta);
    } else if(e == LV_EVENT_DEFOCUSED) {
        lv_textarea_set_cursor_hidden(ta, true);
    }
}

static void ta_event_cb2(lv_obj_t * ta, lv_event_t e) {
    if(e == LV_EVENT_RELEASED) {
        if(kb == NULL) {	// create a keyboard if there isn't one.
	    kb = lv_keyboard_create(lv_scr_act(), NULL);
	    lv_obj_set_size(kb,  LV_HOR_RES, LV_VER_RES / 2);
	    lv_obj_align(kb, NULL, LV_ALIGN_CENTER,  0, -60);
	    lv_keyboard_set_mode(kb, LV_KEYBOARD_MODE_NUM);
	    lv_keyboard_set_textarea(kb, ta);
            lv_obj_set_event_cb(kb, kb_event_cb2);
            lv_indev_wait_release(lv_indev_get_act());
        }
        lv_textarea_set_cursor_hidden(ta, false);
        // lv_page_focus(t1, lv_textarea_get_label(ta), LV_ANIM_ON);
        lv_keyboard_set_textarea(kb, ta);
    } else if(e == LV_EVENT_DEFOCUSED) {
        lv_textarea_set_cursor_hidden(ta, true);
    }
}

static void ta_event_cb3(lv_obj_t * ta, lv_event_t e) {
    if(e == LV_EVENT_RELEASED) {
        if(kb == NULL) {	// create a keyboard if there isn't one.
	    kb = lv_keyboard_create(lv_scr_act(), NULL);
	    lv_obj_set_size(kb,  LV_HOR_RES, LV_VER_RES / 2);
	    lv_obj_align(kb, NULL, LV_ALIGN_CENTER,  0,  60);
	    lv_keyboard_set_mode(kb, LV_KEYBOARD_MODE_TEXT_UPPER);
	    lv_keyboard_set_textarea(kb, ta);
            lv_obj_set_event_cb(kb, kb_event_cb3);
            lv_indev_wait_release(lv_indev_get_act());
        }
        lv_textarea_set_cursor_hidden(ta, false);
        // lv_page_focus(t1, lv_textarea_get_label(ta), LV_ANIM_ON);
        lv_keyboard_set_textarea(kb, ta);
    } else if(e == LV_EVENT_DEFOCUSED) {
        lv_textarea_set_cursor_hidden(ta, true);
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

static void kb_event_cb1(lv_obj_t * _kb, lv_event_t e)
{
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
      event_result = KEYBOARD;
      event_value = 1;
    }
}

static void kb_event_cb2(lv_obj_t * _kb, lv_event_t e)
{
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
      event_result = KEYBOARD;
      event_value = 2;
    }
}

static void kb_event_cb3(lv_obj_t * _kb, lv_event_t e)
{
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
      event_result = KEYBOARD;
      event_value = 3;
    }
}

static void kb_event_cb4(lv_obj_t * _kb, lv_event_t e)
{
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
      event_result = KEYBOARD;
      event_value = 4;
    }
}
#endif

static void dd_event_cb1(lv_obj_t * obj, lv_event_t event)
{
  if(event == LV_EVENT_VALUE_CHANGED) {
    char buf[32];
    lv_dropdown_get_selected_str(obj, buf, sizeof(buf));
    Serial.printf("Option: %s\n", buf);
    event_result = DROPDOWN;
    event_value = lv_dropdown_get_selected(obj);
    dropdown_num = 1;
  }
}

static void dd_event_cb2(lv_obj_t * obj, lv_event_t event)
{
  if(event == LV_EVENT_VALUE_CHANGED) {
    char buf[32];
    lv_dropdown_get_selected_str(obj, buf, sizeof(buf));
    Serial.printf("Option: %s\n", buf);
    event_result = DROPDOWN;
    event_value = lv_dropdown_get_selected(obj);
    dropdown_num = 2;
  }
}

static void dd_event_cb3(lv_obj_t * obj, lv_event_t event)
{
  if(event == LV_EVENT_VALUE_CHANGED) {
    char buf[32];
    lv_dropdown_get_selected_str(obj, buf, sizeof(buf));
    Serial.printf("Option: %s\n", buf);
    event_result = DROPDOWN;
    event_value = lv_dropdown_get_selected(obj);
    dropdown_num = 3;
  }
}

/* page1:
    alarm on/off button
    alarm time general_config.alarm_h:general_config.alarm_m    dropdown(2)
    Done         button
   page2:
    alarm sound  dropdown
    alarm days   checkbox(7)
    Done         button
   page3:
    alarm volume slider
    Done         button
  SOMEWHERE ??? clock style Analog/Basic/LCARS dropdown
 */
void alarmSettings(void) {
uint32_t lasttouch, interval;
int16_t i, max_bounds, nx, ny, x, y, x0, y0, xmax, ymax, points;
  while(1) {
    max_bounds = 0;
    event_result = NILEVENT;
  char buf[10];
    tv = lv_tabview_create(lv_scr_act(), NULL);
    t1 = lv_tabview_add_tab(tv, "Alarm\ntime");
    t2 = lv_tabview_add_tab(tv, "Alarm\nsound");
    t3 = lv_tabview_add_tab(tv, "Alarm\nvolume");
#if NEEDED
    t4 = lv_tabview_add_tab(tv, "Time\nZone");
#endif

    lv_style_init(&style_box);
    lv_style_set_value_align(&style_box, LV_STATE_DEFAULT, LV_ALIGN_OUT_TOP_LEFT);
    lv_style_set_value_ofs_y(&style_box, LV_STATE_DEFAULT, - LV_DPX(10));
    lv_style_set_margin_top(&style_box, LV_STATE_DEFAULT, LV_DPX(30));

    page1_create(t1);
    page2_create(t2);
    page3_create(t3);
#if NEEDED
    page4_create(t4);
#endif
    while(1) {
      my_idle();
      lv_task_handler();
      delay(5);
      if(event_result) {
	switch(event_result) {
	  case BUTTON :	// button
	    Serial.printf("eloop: button %d, value = %d\n", button_num, event_value);
	    switch (button_num) {
	      case 1 :
		goto Exit;
	      case 2 :
		general_config.alarm_enable = event_value;
		if(general_config.alarm_enable) {
		  enable_rtc_alarm();
		}
		else {
		  disable_rtc_alarm();
		}
		lv_label_set_text(b2label, 
		  (general_config.alarm_enable) ? "alarm ON" : "alarm OFF");
		break;
	    }
	    break;
	  case SLIDER :	// slider
	    Serial.printf("eloop: slider %d, value = %d\n", slider_num, event_value);
	    switch (slider_num) {
	      case 1 :
		general_config.alarm_volume = event_value;
		beep(general_config.alarm_sound);
		break;
#if NEEDED
	      case 2 :
		general_config.mumble_gvertz = event_value;
		break;
#endif
	    }
	    break;
#if NEEDED
	  case KEYBOARD :	// text box
	    if(event_value == 1) {	// user is done, save result if legal
	      uint16_t ip[4];	// allows us to check for invalid values
	      boolean valid;
	      valid = true;
	      char fkg_buffer[20];
	      char *cp, *sp;
	      const char * txt = lv_textarea_get_text(ta1);
	      strncpy(fkg_buffer, txt, 20);
	      cp = sp = fkg_buffer;
	      Serial.printf("set MQTT server = %s\n", txt);
	      for(int i = 0 ; i < 4 ; i++) {
		while(*cp != '.' && *cp) { cp++; }
		*cp = '\0';
		ip[i] = atoi(sp);
		sp = ++cp;
		if(ip[i] > 255) { valid = false; }
	      }
	      if(valid) {
		Serial.printf("MQTT server ip = %d.%d.%d.%d\n", ip[0], ip[1], ip[2], ip[3]);
		for(int i = 0 ; i < 4 ; i++) {
		  general_config.mqtt_server[i] = ip[i];
		}
	      }
	      else {
		Serial.println(F("Invalid IP address"));
	      }
	    }
	    else if(event_value == 2) {	// user is done, save port # if legal
	      boolean valid;
	      valid = false;
	      uint32_t port;
	      const char * txt = lv_textarea_get_text(ta2);
	      Serial.printf("set MQTT port = %s\n", txt);
	      port = atoi(txt);
	      if(port <= 65535) { valid = true; }
	      if(valid) {
		Serial.printf("MQTT port = %d\n", port);
		general_config.mqtt_port = port;
	      }
	      else {
		Serial.println(F("Invalid port"));
	      }
	    }
	    else if(event_value == 3) {	// user is done, save username
	      const char * txt = lv_textarea_get_text(ta3);
	      Serial.printf("set MQTT user = %s\n", txt);
	      strncpy(general_config.mqtt_user, txt, sizeof(general_config.mqtt_user));
	    }
	    else if(event_value == 4) {	// user is done, save password
	      const char * txt = lv_textarea_get_text(ta4);
	      Serial.printf("set MQTT password = %s\n", txt);
	      strncpy(general_config.mqtt_pass, txt, sizeof(general_config.mqtt_pass));
	    }
	    break;
#endif
	  case DROPDOWN :	// dropdown
	    if(dropdown_num == 1) {
	      Serial.printf("set alarm_h to %d\n", event_value);
	      general_config.alarm_h = event_value;
	    }
	    else if(dropdown_num == 2) {
	      Serial.printf("set alarm_m index %d -> %d\n", event_value, event_value * 5);
	      general_config.alarm_m = event_value * 5;
	    }
	    else if(dropdown_num == 3) {
	      Serial.printf("set alarm_sound to %d\n", event_value);
	      general_config.alarm_sound = event_value;
	      beep(general_config.alarm_sound);
	    }
	    break;
	}
	event_result = NILEVENT;
      }
    }
  }
  do {
    // wait for a touch
    while (!ttgo->getTouch(x, y)) {
      my_idle();
    }
    // Serial.printf("y = %d\n", y);
    // wait for end of touch
    while (ttgo->getTouch(x0, y0)) {
      my_idle();
    }
  } while(y < 0 || y >= 240);
Exit:
  EEPROM_writeAnything(0, general_config);	// save the setting changes
  EEPROM.commit();
  Serial.println(F("Exit appSettings()"));
  ttgo->tft->fillScreen(TFT_BLACK);
}

static void page1_create(lv_obj_t * parent) {
static char buf[4];	// max 3 bytes for number plus 1 null
int selected;
  lv_obj_t *label;
  lv_page_set_scrl_layout(parent, LV_LAYOUT_PRETTY_TOP);
  lv_disp_size_t disp_size = lv_disp_get_size_category(NULL);
  // LV_DISP_SIZE_SMALL = disp_size = 0 !
  // lv_coord_t grid_w = lv_page_get_width_grid(parent, disp_size <= LV_DISP_SIZE_SMALL ? 1 : 2, 1);
  lv_obj_t * h = lv_cont_create(parent, NULL);
  lv_obj_set_width(h, LV_DPI * 2);
  lv_obj_set_height(h, 200);

  lv_obj_t *btn1 = lv_btn_create(h, NULL);
  lv_obj_set_event_cb(btn1, button_handler);
  lv_obj_align(btn1, NULL, LV_ALIGN_CENTER, -65, 32);

  label = lv_label_create(btn1, NULL);
  lv_obj_set_size(btn1, 100, 50); //set the button size
  lv_label_set_text(label, "Done");

  lv_obj_t *btn2 = lv_btn_create(h, NULL);
  lv_obj_set_event_cb(btn2, button_handler);
  lv_obj_align(btn2, NULL, LV_ALIGN_CENTER, 55, 32);
  lv_btn_set_checkable(btn2, true);
  lv_obj_set_size(btn2, 100, 50); //set the button size
  lv_btn_toggle(btn2);
  // lv_btn_set_fit2(btn2, LV_FIT_NONE, LV_FIT_TIGHT);
  b2label = lv_label_create(btn2, NULL);
  lv_btn_set_state(btn2, (general_config.alarm_enable) ? LV_BTN_STATE_CHECKED_PRESSED : LV_BTN_STATE_CHECKED_RELEASED);
  lv_label_set_text(b2label, 
    (general_config.alarm_enable) ? "alarm ON" : "alarm OFF");

  /* Create the dropdown for alarm hour */
  lv_obj_t * dd1 = lv_dropdown_create(h, NULL);
  lv_obj_add_style(dd1, LV_CONT_PART_MAIN, &style_box);
  lv_obj_set_style_local_value_str(dd1, LV_CONT_PART_MAIN, LV_STATE_DEFAULT, "Hour");
  // lv_obj_set_width(dd1, lv_obj_get_width_grid(h, disp_size <= LV_DISP_SIZE_SMALL ? 1 : 2, 1));
  lv_obj_align(dd1, NULL, LV_ALIGN_CENTER, -30, -40);
  lv_obj_set_width(dd1, 60);
  buff[0] = '\0';
  for(int i = 0 ; i < 24 ; i++) {
    sprintf(buf, "%02d\n", i);
    strcat(buff, buf);
    if(i == general_config.alarm_h) {
      selected = i;
    }
  }
  buff[strlen(buff)-1] = '\0';	// erase final newline
  lv_dropdown_set_options(dd1, buff);
  lv_dropdown_set_selected(dd1, selected);
  lv_obj_set_event_cb(dd1, dd_event_cb1);

  /* Create the dropdown for alarm minute */
  lv_obj_t * dd2 = lv_dropdown_create(h, NULL);
  lv_obj_add_style(dd2, LV_CONT_PART_MAIN, &style_box);
  lv_obj_set_style_local_value_str(dd2, LV_CONT_PART_MAIN, LV_STATE_DEFAULT, "Minute");
  // lv_obj_set_width(dd2, lv_obj_get_width_grid(h, disp_size <= LV_DISP_SIZE_SMALL ? 1 : 2, 1));
  lv_obj_align(dd2, NULL, LV_ALIGN_CENTER,  40, -40);
  lv_obj_set_width(dd2, 60);
  buff[0] = '\0';
  for(int i = 0 ; i < 60/5 ; i++) {
    sprintf(buf, "%02d\n", i * 5);
    strcat(buff, buf);
    if(i * 5 == general_config.alarm_m) {
      selected = i;
    }
  }
  buff[strlen(buff)-1] = '\0';	// erase final newline

  lv_dropdown_set_options(dd2, buff);
  lv_dropdown_set_selected(dd2, selected);
  lv_obj_set_event_cb(dd2, dd_event_cb2);
}

static void page2_create(lv_obj_t * parent) {
static char buf[4];	// max 3 bytes for number plus 1 null
int selected;
  lv_obj_t *label;
  lv_page_set_scrl_layout(parent, LV_LAYOUT_PRETTY_TOP);
  lv_obj_t * h = lv_cont_create(parent, NULL);
  lv_obj_set_width(h, LV_DPI * 2);
  lv_obj_set_height(h, 200);

  lv_obj_t *btn1 = lv_btn_create(h, NULL);
  lv_obj_set_event_cb(btn1, button_handler);
  lv_obj_align(btn1, NULL, LV_ALIGN_CENTER, -65, 32);

  label = lv_label_create(btn1, NULL);
  lv_obj_set_size(btn1, 100, 50); //set the button size
  lv_label_set_text(label, "Done");

  /* Create the dropdown for alarm sound */
  lv_obj_t * dd3 = lv_dropdown_create(h, NULL);
  lv_obj_add_style(dd3, LV_CONT_PART_MAIN, &style_box);
  lv_obj_set_style_local_value_str(dd3, LV_CONT_PART_MAIN, LV_STATE_DEFAULT, "Sound");
  // lv_obj_set_width(dd3, lv_obj_get_width_grid(h, disp_size <= LV_DISP_SIZE_SMALL ? 1 : 2, 1));
  lv_obj_align(dd3, NULL, LV_ALIGN_CENTER,   0, -40);
  lv_obj_set_width(dd3, 110);
  buff[0] = '\0';
  for(int i = 0 ; i < number_of_sounds ; i++) {
    sprintf(buf, "%02d\n", i + 1);
    strcat(buff, buf);
    if(i == general_config.alarm_sound) {
      selected = i;
    }
  }
  buff[strlen(buff)-1] = '\0';	// erase final newline

  lv_dropdown_set_options(dd3, buff);
  lv_dropdown_set_selected(dd3, selected);
  lv_obj_set_event_cb(dd3, dd_event_cb3);

  // MISSING: CHECKBOX for DAYS
}

static void page3_create(lv_obj_t * parent) {
static char buf[4];	// max 3 bytes for number plus 1 null
  lv_obj_t *label;
  lv_page_set_scrl_layout(parent, LV_LAYOUT_PRETTY_TOP);
  lv_obj_t * h = lv_cont_create(parent, NULL);
  lv_obj_set_width(h, LV_DPI * 2);
  lv_obj_set_height(h, 200);

  lv_obj_t *btn1 = lv_btn_create(h, NULL);
  lv_obj_set_event_cb(btn1, button_handler);
  lv_obj_align(btn1, NULL, LV_ALIGN_CENTER, -65, 32);

  label = lv_label_create(btn1, NULL);
  lv_obj_set_size(btn1, 100, 50); //set the button size
  lv_label_set_text(label, "Done");

  lv_obj_t * slider1 = lv_slider_create(h, NULL);
  lv_obj_set_width(slider1, 200);
  lv_obj_align(slider1, NULL, LV_ALIGN_CENTER, -20, -70);
  lv_obj_set_event_cb(slider1, slider1_handler);
  lv_slider_set_range(slider1, 10, 100);
  lv_slider_set_value(slider1, general_config.alarm_volume, LV_ANIM_OFF);
  /* Create a label below the slider */
  slider1_label = lv_label_create(h, NULL);
  snprintf(buf, 4, "%u", lv_slider_get_value(slider1));
  lv_label_set_text(slider1_label, buf);
  lv_obj_set_auto_realign(slider1_label, true);
  lv_obj_align(slider1_label, slider1, LV_ALIGN_OUT_BOTTOM_MID, 0, 5);
  slider1_name = lv_label_create(h, NULL);
  lv_label_set_text(slider1_name, "alarm_volume");
  lv_obj_set_auto_realign(slider1_name, true);
  lv_obj_align(slider1_name, slider1, LV_ALIGN_OUT_TOP_MID, 0, -5);
}

#if NEEDED  // sliders
static void page4_create(lv_obj_t * parent) {
char buf[32];
int selected;
  lv_obj_t *label;
  lv_page_set_scrl_layout(parent, LV_LAYOUT_PRETTY_TOP);
  lv_obj_t * h = lv_cont_create(parent, NULL);
  lv_obj_set_width(h, LV_DPI * 2);
  lv_obj_set_height(h, 200);

  lv_obj_t *btn1 = lv_btn_create(h, NULL);
  lv_obj_set_event_cb(btn1, button_handler);
  lv_obj_align(btn1, NULL, LV_ALIGN_CENTER, -65, 32);

  label = lv_label_create(btn1, NULL);
  lv_obj_set_size(btn1, 100, 50); //set the button size
  lv_label_set_text(label, "Done");

}
#endif

#if NEEDED  // sliders

  lv_obj_t * slider2 = lv_slider_create(h, NULL);
  lv_obj_set_width(slider2, 200);
  lv_obj_align(slider2, NULL, LV_ALIGN_CENTER, -20, -25);
  lv_obj_set_event_cb(slider2, slider2_handler);
  lv_slider_set_range(slider2, 5, 120);
  lv_slider_set_value(slider2, general_config.screensaver_timeout, LV_ANIM_OFF);
  /* Create a label below the slider */
  slider2_label = lv_label_create(h, NULL);
  snprintf(buf, 4, "%u", lv_slider_get_value(slider2));
  lv_label_set_text(slider2_label, buf);
  lv_obj_set_auto_realign(slider2_label, true);
  lv_obj_align(slider2_label, slider2, LV_ALIGN_OUT_BOTTOM_MID, 0, 5);
  slider2_name = lv_label_create(h, NULL);
  lv_label_set_text(slider2_name, "screenblank time");
  lv_obj_set_auto_realign(slider2_name, true);
  lv_obj_align(slider2_name, slider2, LV_ALIGN_OUT_TOP_MID, 0, -5);
#endif

#if NEEDED // TEXT AREA for digits or digits with '.'
  /* Create the text area for MQTT server IP */
  ta1 = lv_textarea_create(h, NULL);
  lv_obj_set_event_cb(ta1, ta_event_cb1);
  lv_obj_set_width(ta1, 150);
  lv_obj_align(ta1, NULL, LV_ALIGN_CENTER, -25, -30);	// order of operations matters!
  lv_textarea_set_accepted_chars(ta1, "0123456789.");
  lv_textarea_set_max_length(ta1, 15);
  lv_textarea_set_one_line(ta1, true);
  sprintf(buff, "%d.%d.%d.%d",
    general_config.mqtt_server[0], general_config.mqtt_server[1],
    general_config.mqtt_server[2], general_config.mqtt_server[3]);
  lv_textarea_set_text(ta1, buff);
  lv_obj_t *ta1_name = lv_label_create(h, NULL);
  lv_label_set_text(ta1_name, "MQTT server IP");
  lv_obj_set_auto_realign(ta1_name, true);
  lv_obj_align(ta1_name, ta1, LV_ALIGN_OUT_BOTTOM_MID, 0,  0);

  /* Create the text area for MQTT port */
  ta2 = lv_textarea_create(h, NULL);
  lv_obj_set_event_cb(ta2, ta_event_cb2);
  lv_obj_set_width(ta2, 150);
  lv_obj_align(ta2, NULL, LV_ALIGN_CENTER, -25, 20);	// order of operations matters!
  lv_textarea_set_accepted_chars(ta2, "0123456789");
  lv_textarea_set_max_length(ta2, 6);
  lv_textarea_set_one_line(ta2, true);
  sprintf(buff, "%d", general_config.mqtt_port);
  lv_textarea_set_text(ta2, buff);
  lv_obj_t *ta2_name = lv_label_create(h, NULL);
  lv_label_set_text(ta2_name, "MQTT port");
  lv_obj_set_auto_realign(ta2_name, true);
  lv_obj_align(ta2_name, ta2, LV_ALIGN_OUT_BOTTOM_MID, 0,  0);
#endif
#if NEEDED // textarea for all characters
  /* Create the text area for MQTT user */
  ta3 = lv_textarea_create(h, NULL);
  lv_obj_set_event_cb(ta3, ta_event_cb3);
  lv_obj_set_width(ta3, 150);
  lv_obj_align(ta3, NULL, LV_ALIGN_CENTER, -25, -30);	// order of operations matters!
  lv_textarea_set_max_length(ta3, 20);
  lv_textarea_set_one_line(ta3, true);
  // Serial.printf("mqtt_user was %s\n", general_config.mqtt_user);
  // strcpy(general_config.mqtt_user, "OBS27-27A");
  sprintf(buff, "%s", general_config.mqtt_user);
  lv_textarea_set_text(ta3, buff);
  lv_obj_t *ta3_name = lv_label_create(h, NULL);
  lv_label_set_text(ta3_name, "MQTT user");
  lv_obj_set_auto_realign(ta3_name, true);
  lv_obj_align(ta3_name, ta3, LV_ALIGN_OUT_BOTTOM_MID, 0,  0);

  /* Create the text area for MQTT password */
  ta4 = lv_textarea_create(h, NULL);
  lv_obj_set_event_cb(ta4, ta_event_cb4);
  lv_obj_set_width(ta4, 150);
  lv_obj_align(ta4, NULL, LV_ALIGN_CENTER, -25, 20);	// order of operations matters!
  lv_textarea_set_max_length(ta4, 20);
  lv_textarea_set_one_line(ta4, true);
  sprintf(buff, "%s", general_config.mqtt_pass);
  lv_textarea_set_text(ta4, buff);
  lv_obj_t *ta4_name = lv_label_create(h, NULL);
  lv_label_set_text(ta4_name, "MQTT password");
  lv_obj_set_auto_realign(ta4_name, true);
  lv_obj_align(ta4_name, ta4, LV_ALIGN_OUT_BOTTOM_MID, 0,  0);
#endif
