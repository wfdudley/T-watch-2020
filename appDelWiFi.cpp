// delete one wifi AP (stored in "file" acc_pts.txt (in SPIFF))

#include "config.h"
#include "DudleyWatch.h"
#include <time.h>
#include <WiFi.h>
#include <NTPClient.h>
#include <WiFiUdp.h>
#include "my_tz.h"

#include "my_WiFi.h"      // define "my" wifi stuff

#include "FS.h"		// this breaks if config.h is included.
#include "SPIFFS.h"	// supposed to include <FS.h>
#include "SPIFF_fns.h"

#define DBGDELWIFI 1

#define USE_SPIFFS_CPP 1
#define WE_HAVE_SPIFFS 0
#if WE_HAVE_SPIFFS
error: initializer-string for array of chars is too longerror: initializer-string for array of chars is too longerror: initializer-string for array of chars is too longFile ofile;
File ifile;
#endif

int get_which_ap_to_delete_from_user(void);

static uint8_t event_result;
static int16_t event_value;
static uint8_t button_num;
static uint8_t dropdown_num;
static lv_style_t style_box;	// what good is this?
static lv_obj_t * tv;
static lv_obj_t * ta4;	// WiFi password text area
static lv_obj_t * kb;
static lv_obj_t * dd1;
static void kb_event_cb4(lv_obj_t * ta, lv_event_t e);
static uint32_t utzidx;
static char ussid[66];

void appDelWiFi(void) {
int res;
  button_num = 0;
  dropdown_num = 0;
  event_result = NILEVENT;
  if(!file_exists("/acc_pts.txt")) {
    tft->setTextColor(TFT_YELLOW, TFT_BLACK);
    tft->setTextSize(1);
    tft->setCursor(0, 50 + (15 * 7));
    tft->print(F("No wifi data file (acc_pts.txt) found!"));
    delay(5000);
    tft->fillScreen(TFT_BLACK);
    return;
  }
  res = get_which_ap_to_delete_from_user();
  if(res) {
    tft->setTextColor(TFT_YELLOW, TFT_BLACK);
    tft->setTextSize(1);
    tft->setCursor(0, 50 + (15 * 7));
    tft->print(F("problems deleting SSID data!"));
    delay(5000);
    tft->fillScreen(TFT_BLACK);
  }
  return;
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
	button_num = (strcmp(txt, "Delete")) ? 2 : 1 ;
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
	button_num = (strcmp(txt, "Done")) ? 2 : 1 ;
	Serial.printf("handler label = %s, button_num = %d\n", txt, button_num);
	// state of toggled button
	event_value = lv_obj_get_state(obj, LV_BTN_PART_MAIN) & LV_STATE_CHECKED;
	Serial.printf("state = %d -> %s\n", event_value, event_value ? "true" : "false");
	return;
    }
#endif
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

#if HAS_TABS
#define PARENT parent
#else
#define PARENT lv_scr_act()
#endif

static void page_create(lv_obj_t * parent) {
char buff[4096];
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
  lv_obj_align(btn1, NULL, LV_ALIGN_CENTER, -40, 90);

  label = lv_label_create(btn1, NULL);
  lv_obj_set_size(btn1, 100, 50); //set the button size
  lv_label_set_text(label, "Delete");

  lv_obj_t *btn2 = lv_btn_create(h, NULL);
  lv_obj_set_event_cb(btn2, button_handler);
  lv_obj_align(btn2, NULL, LV_ALIGN_CENTER,  70, 90);

  label = lv_label_create(btn2, NULL);
  lv_obj_set_size(btn2, 100, 50); //set the button size
  lv_label_set_text(label, "Cancel");

  /* Create the dropdown for SSID */
  dd1 = lv_dropdown_create(h, NULL);
  lv_obj_add_style(dd1, LV_CONT_PART_MAIN, &style_box);
  lv_obj_set_style_local_value_str(dd1, LV_CONT_PART_MAIN, LV_STATE_DEFAULT, "            SSID");
  // lv_obj_set_width(dd1, lv_obj_get_width_grid(h, disp_size <= LV_DISP_SIZE_SMALL ? 1 : 2, 1));
  lv_obj_align(dd1, NULL, LV_ALIGN_CENTER, -50, -70);
  lv_obj_set_width(dd1, 200);
  memset(buff, '\0', sizeof(buff));
  strcpy(buff, "none chosen\n");
  get_list_ssid(&buff[strlen(buff)], sizeof(buff)-strlen(buff));
  strcpy(ussid, "none chosen");
  lv_dropdown_set_options(dd1, buff);
  lv_dropdown_set_selected(dd1, selected);
  lv_obj_set_event_cb(dd1, dd_event_cb1);
}

int get_which_ap_to_delete_from_user (void) {
char upass[66];
  utzidx = 0;
  memset(ussid, '\0', sizeof(ussid));
  // use LVGL to make a settings screen for new wifi access point.
#if 0
  tv = lv_tabview_create(lv_scr_act(), NULL);
#endif
ReStart:
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
	case BUTTON :	// button
	  Serial.printf("eloop: button %d, value = %d\n", button_num, event_value);
	  switch (button_num) {
	    case 1 :
	      Serial.print(F("User hit Delete\n"));
	      goto Exit;
	    case 2 :
	      Serial.print(F("User hit Cancel\n"));
	      strcpy(ussid, "none chosen");
	      goto Exit;
	  }
	  break;
	case SLIDER :	// slider
	  break;
	case KEYBOARD :	// text box
	  break;
	case DROPDOWN :	// dropdown
	  if(dropdown_num == 1) {
	    char *cp1, *cp2;
	    buff[4096];
	    strncpy(buff, lv_dropdown_get_options(dd1), 4096);
	    buff[4095] = '\0';
	    cp1 = &buff[0];
	    for(int i = 0 ; i < event_value ; i++) {
	      cp1 = strchr(cp1 + 1, '\n');
	      if(!cp1) break;
	    }
	    if(cp1) {
	      cp2 = strchr(cp1 + 1, '\n');
	      if(cp2) { *cp2 = '\0'; }
	      strncpy(ussid, cp1+1, sizeof(ussid));
	      if(cp2) { *cp2 = '\n'; }
	    }
	    Serial.printf("chosen ssid is %s\n", ussid);
	  }
	  break;
      }
      event_result = NILEVENT;
    }
  }
Exit:
  tft->fillScreen(TFT_BLACK);
  Serial.printf("user chose: %s\n", ussid);
  if(strcmp(ussid, "none chosen")) {
    Serial.printf("about to delete line containing: ssid %s\n", ussid);
    return delete_access_point(ussid);
  }
  return(0);
}

