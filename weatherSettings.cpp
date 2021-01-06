#include "config.h"
#include "DudleyWatch.h"
#include <OpenWeatherOneCall.h>
#include "personal_info.h"

extern OpenWeatherOneCall OWOC;

static uint8_t event_result;
static int16_t event_value;
static uint8_t button_num;
static uint8_t dropdown_num;
static uint8_t switch_num;
static lv_style_t style_box;	// what good is this?
static lv_obj_t * tv;
static lv_obj_t * t1;
static lv_obj_t * t2;
static lv_obj_t * ta1;	// latitude text area
static lv_obj_t * ta2;	// longitude text area
static lv_obj_t * ta4;	// OWM API key text area
static lv_obj_t * kb;	// full keyboard
static lv_obj_t * nkb;	// numeric keyboard
static lv_obj_t * sw1;
static void kb_event_cb1(lv_obj_t * ta, lv_event_t e);
static void kb_event_cb4(lv_obj_t * ta, lv_event_t e);
static void page1_create(lv_obj_t * parent);
static void page2_create(lv_obj_t * parent);

static void button_handler(lv_obj_t *obj, lv_event_t event) {
    // Serial.printf("button_handler() event = %d\n", (int)event);
    if (event == LV_EVENT_CLICKED && !event_result) {
        Serial.println(F("Clicked"));
	event_result = BUTTON;
	event_value = 1;
	lv_obj_t * label = lv_obj_get_child(obj, NULL);
	char * txt = lv_label_get_text(label);
	// Serial.printf("button label is %s\n", txt);
	button_num = (strcmp(txt, "Save")) ? 2 : 1 ;
	Serial.printf("handler label = %s, button_num = %d\n", txt, button_num);
	// lv_label_set_text_fmt(label, "Button: %d", cnt);
	return;
    }
#if NEEDED
    else if (event == LV_EVENT_VALUE_CHANGED && !event_result) {
        Serial.println(F("Toggled"));
	event_result = BUTTON;
	lv_obj_t * label = lv_obj_get_child(obj, NULL);
	char * txt = lv_label_get_text(label);
	// Serial.printf("button label is %s\n", txt);
	button_num = (strcmp(txt, "Save")) ? 2 : 1 ;
	Serial.printf("handler label = %s, button_num = %d\n", txt, button_num);
	// state of toggled button
	event_value = lv_obj_get_state(obj, LV_BTN_PART_MAIN) & LV_STATE_CHECKED;
	Serial.printf("state = %d -> %s\n", event_value, event_value ? "true" : "false");
	return;
    }
#endif
}

static void stepc_chg_event_cb(lv_obj_t *obj, lv_event_t event) {
  if (event == LV_EVENT_VALUE_CHANGED) {
    event_result = SWITCH;
    event_value = lv_switch_get_state(sw1);
    switch_num = 1;
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
      event_result = KEYBOARD;
      event_value = 4;
      if(kb) {
	lv_obj_del(kb);	// delete the keyboard if done.
	kb = NULL;
      }
    }
}

static void kb_event_cb1(lv_obj_t * _kb, lv_event_t e) {
    lv_keyboard_def_event_cb(nkb, e);

    if(e == LV_EVENT_CANCEL) {
        if(nkb) {
            // lv_obj_set_height(tv, LV_VER_RES);
	    lv_obj_del(nkb);	// delete the keyboard if done.
	    nkb = NULL;
        }
    }
    if(e == LV_EVENT_APPLY) {
      // announce that the user is finished with the box.
      event_result = KEYBOARD;
      event_value = 1;
      if(nkb) {
	lv_obj_del(nkb);	// delete the keyboard if done.
	nkb = NULL;
      }
    }
}

static void kb_event_cb2(lv_obj_t * _kb, lv_event_t e) {
    lv_keyboard_def_event_cb(nkb, e);

    if(e == LV_EVENT_CANCEL) {
        if(nkb) {
            // lv_obj_set_height(tv, LV_VER_RES);
	    lv_obj_del(nkb);	// delete the keyboard if done.
	    nkb = NULL;
        }
    }
    if(e == LV_EVENT_APPLY) {
      // announce that the user is finished with the box.
      event_result = KEYBOARD;
      event_value = 2;
      if(nkb) {
	lv_obj_del(nkb);	// delete the keyboard if done.
	nkb = NULL;
      }
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

static void ta_event_cb1(lv_obj_t * ta, lv_event_t e) {
    if(e == LV_EVENT_RELEASED) {
        if(nkb == NULL) {	// create a keyboard if there isn't one.
	    nkb = lv_keyboard_create(lv_scr_act(), NULL);
	    lv_obj_set_size(nkb,  LV_HOR_RES, LV_VER_RES / 2);
	    lv_obj_align(nkb, NULL, LV_ALIGN_CENTER,  0, +60);
	    lv_keyboard_set_mode(nkb, LV_KEYBOARD_MODE_NUM);
	    lv_keyboard_set_textarea(nkb, ta);
            lv_obj_set_event_cb(nkb, kb_event_cb1);
            lv_indev_wait_release(lv_indev_get_act());
        }
        lv_textarea_set_cursor_hidden(ta, false);
        // lv_page_focus(t1, lv_textarea_get_label(ta), LV_ANIM_ON);
        lv_keyboard_set_textarea(nkb, ta);
    } else if(e == LV_EVENT_DEFOCUSED) {
        lv_textarea_set_cursor_hidden(ta, true);
    }
}

static void ta_event_cb2(lv_obj_t * ta, lv_event_t e) {
    if(e == LV_EVENT_RELEASED) {
        if(nkb == NULL) {	// create a keyboard if there isn't one.
	    nkb = lv_keyboard_create(lv_scr_act(), NULL);
	    lv_obj_set_size(nkb,  LV_HOR_RES, LV_VER_RES / 2);
	    lv_obj_align(nkb, NULL, LV_ALIGN_CENTER,  0, -60);
	    lv_keyboard_set_mode(nkb, LV_KEYBOARD_MODE_NUM);
	    lv_keyboard_set_textarea(nkb, ta);
            lv_obj_set_event_cb(nkb, kb_event_cb2);
            lv_indev_wait_release(lv_indev_get_act());
        }
        lv_textarea_set_cursor_hidden(ta, false);
        // lv_page_focus(t1, lv_textarea_get_label(ta), LV_ANIM_ON);
        lv_keyboard_set_textarea(nkb, ta);
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
    event_result = DROPDOWN;
    event_value = lv_dropdown_get_selected(obj);
    dropdown_num = 1;
  }
}

#define HAS_TABS 1
#if HAS_TABS
#define PARENT parent
#else
#define PARENT lv_scr_act()
#endif

// GUI creates:
// Done button
// Imperial/Metric switch
// Language dropdown
// OWM API KEY textarea
//
// latitude textarea
// longitude textarea

// the openweathermap.org "language" function is hardly worth the trouble.
// owm doesn't translate most of the information into the selected language.
// the only thing that seems to get converted is the "description" field in
// "current". In particular, National Weather Service alerts are NOT translated.
#define OWM_LANGUAGE_FEATURE 0

static void gui_create(void) {
  tv = lv_tabview_create(lv_scr_act(), NULL);
  t1 = lv_tabview_add_tab(tv, "Page\n  1");
  t2 = lv_tabview_add_tab(tv, "Page\n  2");
  page1_create(t1);
  page2_create(t2);

  lv_style_init(&style_box);
  lv_style_set_value_align(&style_box, LV_STATE_DEFAULT, LV_ALIGN_OUT_TOP_LEFT);
  lv_style_set_value_ofs_y(&style_box, LV_STATE_DEFAULT, - LV_DPX(10));
  lv_style_set_margin_top(&style_box, LV_STATE_DEFAULT, LV_DPX(30));
}

static void page1_create(lv_obj_t * parent) {
char buff[512];
static char buf[66];
int selected;
  lv_obj_t *label;
  // lv_page_set_scrl_layout(PARENT, LV_LAYOUT_PRETTY_TOP);
  lv_page_set_scrlbar_mode(PARENT, LV_SCRLBAR_MODE_OFF);
  lv_obj_t * h = lv_cont_create(PARENT, NULL);
  // lv_obj_set_width(h, LV_DPI * 2);
  lv_obj_set_width(h, 240);
  // lv_obj_set_height(h, 200);
  lv_obj_set_height(h, 240);

  lv_obj_t *btn1 = lv_btn_create(h, NULL);
  lv_obj_set_event_cb(btn1, button_handler);
  lv_obj_align(btn1, NULL, LV_ALIGN_CENTER, -50, 20);

  label = lv_label_create(btn1, NULL);
  lv_obj_set_size(btn1, 100, 50); //set the button size
  lv_label_set_text(label, "Save");

  lv_obj_t *btn2 = lv_btn_create(h, NULL);
  lv_obj_set_event_cb(btn2, button_handler);
  lv_obj_align(btn2, NULL, LV_ALIGN_CENTER,  70, 20);

  label = lv_label_create(btn2, NULL);
  lv_obj_set_size(btn2, 100, 50); //set the button size
  lv_label_set_text(label, "Cancel");

#if OWM_LANGUAGE_FEATURE
  /* Create the dropdown for language */
  lv_obj_t * dd1 = lv_dropdown_create(h, NULL);
  lv_obj_add_style(dd1, LV_CONT_PART_MAIN, &style_box);
  lv_obj_set_style_local_value_str(dd1, LV_CONT_PART_MAIN, LV_STATE_DEFAULT, "  Language");
  // lv_obj_set_width(dd1, lv_obj_get_width_grid(h, disp_size <= LV_DISP_SIZE_SMALL ? 1 : 2, 1));
  lv_obj_align(dd1, NULL, LV_ALIGN_CENTER, 0, -20);
  lv_obj_set_width(dd1, 110);
  buff[0] = '\0';
  char codebuf[6];
  char *bp = buff;
  char *rp = buff;
  // Serial.printf("gen_cfg.language = %s\n", general_config.language);
  for(int i = 0 ; rp ; i++) {
    rp = OWOC.nextLanguage(codebuf, bp, i);
    // rp = nextLanguage(codebuf, bp, i);
    if(rp) {
      if(!strcmp(codebuf, general_config.language)) {
	Serial.printf("language, found match i = %d, value %s\n", i, codebuf);
	selected = i;
      }
      bp += strlen(bp);
      *bp++ = '\n';
      *bp = '\0';
    }
  }
  lv_dropdown_set_options(dd1, buff);
  lv_dropdown_set_selected(dd1, selected);
  lv_obj_set_event_cb(dd1, dd_event_cb1);
#endif

  sw1 = lv_switch_create(h, NULL);
  if(general_config.metric_units) { lv_switch_on(sw1, LV_ANIM_OFF); }
  Serial.printf("Units switch should be %s\n", (general_config.metric_units) ? "Metric" : "Imperial");
  lv_obj_set_event_cb(sw1, stepc_chg_event_cb);
  lv_obj_align(sw1, NULL, LV_ALIGN_CENTER, -2, -90);
  lv_obj_set_style_local_value_str(sw1, LV_SWITCH_PART_BG, LV_STATE_DEFAULT, "Metric");
  lv_obj_set_style_local_value_align(sw1, LV_SWITCH_PART_BG, LV_STATE_DEFAULT, LV_ALIGN_OUT_RIGHT_MID);
  lv_obj_set_style_local_value_ofs_x(sw1, LV_SWITCH_PART_BG, LV_STATE_DEFAULT, LV_DPI/35);

  /* Create the text area for OWM API key */
  ta4 = lv_textarea_create(h, NULL);
  lv_obj_set_event_cb(ta4, ta_event_cb4);
  lv_obj_set_width(ta4, 220);
  lv_obj_align(ta4, NULL, LV_ALIGN_CENTER, 0,  0);	// order of operations matters!
  lv_textarea_set_max_length(ta4, 40);
  lv_textarea_set_one_line(ta4, true);
  lv_obj_t *ta4_name = lv_label_create(h, NULL);
  Serial.printf("API key = %s\n", general_config.owm_api_key);
  if(strcmp(general_config.owm_api_key, ONECALLKEY)) {
    strcpy(general_config.owm_api_key, ONECALLKEY);
    Serial.printf("FIXING API key = %s\n", general_config.owm_api_key);
  }
  sprintf(buff, "%s", general_config.owm_api_key);
  lv_textarea_set_text(ta4, buff);
  lv_label_set_text(ta4_name, "OWM API key");
  lv_obj_set_auto_realign(ta4_name, true);
  lv_obj_align(ta4_name, ta4, LV_ALIGN_OUT_BOTTOM_MID, 0,  0);
}

static void page2_create(lv_obj_t * parent) {
char buff[512];
static char buf[66];
int selected;
  lv_obj_t *label;
  // lv_page_set_scrl_layout(PARENT, LV_LAYOUT_PRETTY_TOP);
  lv_page_set_scrlbar_mode(PARENT, LV_SCRLBAR_MODE_OFF);
  lv_obj_t * h = lv_cont_create(PARENT, NULL);
  // lv_obj_set_width(h, LV_DPI * 2);
  lv_obj_set_width(h, 240);
  // lv_obj_set_height(h, 200);
  lv_obj_set_height(h, 240);

  lv_obj_t *btn1 = lv_btn_create(h, NULL);
  lv_obj_set_event_cb(btn1, button_handler);
  lv_obj_align(btn1, NULL, LV_ALIGN_CENTER, -50, 20);

  label = lv_label_create(btn1, NULL);
  lv_obj_set_size(btn1, 100, 50); //set the button size
  lv_label_set_text(label, "Save");

  lv_obj_t *btn2 = lv_btn_create(h, NULL);
  lv_obj_set_event_cb(btn2, button_handler);
  lv_obj_align(btn2, NULL, LV_ALIGN_CENTER,  70, 20);

  label = lv_label_create(btn2, NULL);
  lv_obj_set_size(btn2, 100, 50); //set the button size
  lv_label_set_text(label, "Cancel");

  /* Create the text area for home latitude */
  ta1 = lv_textarea_create(h, NULL);
  lv_obj_set_event_cb(ta1, ta_event_cb1);
  lv_obj_set_width(ta1, 100);
  lv_obj_align(ta1, NULL, LV_ALIGN_CENTER, 0, -50);	// order of operations matters!
  lv_textarea_set_max_length(ta1, 40);
  lv_textarea_set_one_line(ta1, true);
  lv_obj_t *ta1_name = lv_label_create(h, NULL);
  Serial.printf("latitude = %s\n", general_config.my_latitude);
  sprintf(buff, "%s", general_config.my_latitude);
  lv_textarea_set_text(ta1, buff);
  lv_label_set_text(ta1_name, "home latitude");
  lv_obj_set_auto_realign(ta1_name, true);
  lv_obj_align(ta1_name, ta1, LV_ALIGN_OUT_BOTTOM_MID, 0, 0);

  /* Create the text area for home longitude */
  ta2 = lv_textarea_create(h, NULL);
  lv_obj_set_event_cb(ta2, ta_event_cb2);
  lv_obj_set_width(ta2, 100);
  lv_obj_align(ta2, NULL, LV_ALIGN_CENTER, 0, 10);	// order of operations matters!
  lv_textarea_set_max_length(ta2, 40);
  lv_textarea_set_one_line(ta2, true);
  lv_obj_t *ta2_name = lv_label_create(h, NULL);
  Serial.printf("longitude = %s\n", general_config.my_longitude);
  sprintf(buff, "%s", general_config.my_longitude);
  lv_textarea_set_text(ta2, buff);
  lv_label_set_text(ta2_name, "home longitude");
  lv_obj_set_auto_realign(ta2_name, true);
  lv_obj_align(ta2_name, ta2, LV_ALIGN_OUT_BOTTOM_MID, 0,  0);
}

void weatherSettings (void) {
int api_key_has_been_set = 0;
  // use LVGL to make a settings screen for new wifi access point.
ReStart:
  gui_create();
  event_result = NILEVENT;
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
	      Serial.print(F("User hit Done\n"));
	      button_num = 0;
	      event_result = 0;
              if(kb) {
		lv_obj_del(kb);	// delete the keyboard if done.
		kb = NULL;
	      }
	      goto Exit;
	    case 2 :
	      Serial.print(F("User hit Cancel\n"));
	      goto Leave;
	  }
	  break;
	case SLIDER :	// slider
	  break;
	case SWITCH :	// switch
	  switch(switch_num) {
	    case 1 :
	      general_config.metric_units = event_value;
	      Serial.printf("Units switch now = %s\n", (general_config.metric_units) ? "Metric" : "Imperial");
	      break;
	  }
	  break;
	case KEYBOARD :	// text box
	  const char *txt;
	  switch(event_value) {
	    case 1 :	// latitude
	      txt = lv_textarea_get_text(ta1);
	      Serial.printf("set my_latitude = %s, sizeof(my_latitude) = %d\n",
		txt, sizeof(general_config.my_latitude));
	      strncpy(general_config.my_latitude, txt, sizeof(general_config.my_latitude));
	      break;
	    case 2 :	// longitude
	      txt = lv_textarea_get_text(ta2);
	      Serial.printf("set my_longitude = %s, sizeof(my_longitude) = %d\n",
		txt, sizeof(general_config.my_longitude));
	      strncpy(general_config.my_longitude, txt, sizeof(general_config.my_longitude));
	      break;
	    case 4 :	// owm api key
	      txt = lv_textarea_get_text(ta4);
	      Serial.printf("set API key = %s, sizeof(owm_api_key) = %d\n",
		txt, sizeof(general_config.owm_api_key));
	      strncpy(general_config.owm_api_key, txt, sizeof(general_config.owm_api_key));
	      api_key_has_been_set++;
	      break;
	  }
	  break;
	case DROPDOWN :
#if OWM_LANGUAGE_FEATURE
	  if(dropdown_num == 1) {	// language dropdown
	    char buff[8];
	    char *bptr = OWOC.nextLanguage(buff, NULL, event_value);
	    // char *bptr = nextLanguage(buff, NULL, event_value);
	    if(bptr) {
	      Serial.printf("set language to index %d -> %s\n", event_value, buff);
	      strcpy(general_config.language, buff);
	    }
	  }
#endif
	  break;
      }
      event_result = NILEVENT;
    }
  }
Exit:
  ttgo->tft->fillScreen(TFT_BLACK);
  if(!api_key_has_been_set) {
    tft->setCursor(0, 50);
    tft->printf("You must hit the checkmark on the\nkeyboard after you have entered\nthe API key");
    delay(2000);
    goto ReStart;
  }
  EEPROM_writeAnything(0, general_config);	// save the setting changes
  EEPROM.commit();
Leave:
  Serial.println(F("Exit weatherSettings()"));
  ttgo->tft->fillScreen(TFT_BLACK);
}
