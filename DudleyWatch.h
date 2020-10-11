/*
  change log
  2020 09 12 -- added step display to all clocks, re-worked step icon to be
  re-sizeable, made step display code take arguments.
  Added converting char * IP address to array of four bytes.  Added saving
  config changes to EEPROM in appSettings.

  2020 09 23 -- added sound and alarm values to general_config

  W.F.Dudley Jr.
*/
#ifndef DUDLEY_WATCH_H
#define DUDLEY_WATCH_H

#include <EEPROM.h>
#include "EEPROM_rw_anything.h"

#ifdef __MAIN__
#define EXTERN
#define INIT(x) = x
#else
#define EXTERN extern
#define INIT(x)
#endif

struct menu_item{
  char *name;
  char *topic;
  void* next_menu;
};

void Analog_Time(uint8_t);
void LCARS_Time(uint8_t);
void Basic_Time(uint8_t);
void LilyGo_Time(uint8_t);
boolean if_not_home_tz(void);

EXTERN uint8_t home_hh;
EXTERN uint8_t home_mm;
EXTERN uint8_t home_ss;
EXTERN RTC_Date tnow;
EXTERN uint8_t local_hour;
EXTERN uint8_t local_minute;
EXTERN boolean rtcIrq;

// this is neat, but we store chosen_skin in general_config, so . . .
// EXTERN void (*watch_fun_pointer)(uint8_t);

EXTERN struct menu_item skin_menu[]
#ifdef __MAIN__
  = {
      { "Analog", "", (void *)&Analog_Time },
      { "LCARS",  "", (void *)&LCARS_Time  },
      { "Basic",  "", (void *)&Basic_Time  },
      { "LilyGo", "", (void *)&LilyGo_Time }
//    { "Exit",  "",  NULL      }
    }
#endif
    ;

// skins:
#define LCARS 1
#define BASIC 2
#define ANALOG 3
#define LILYGO 4

uint8_t modeMenu(void);	// app selector

void appMaze(void);		// Maze game/demo
void appSetTime(void);		// set clock from keyboard
void appSettings(void);		// settings
void displayTime(uint8_t);	// watch "wrapper" app
void jSats(void);		// Jupiter satellites app
void appLevel(void);		// Bubble Level (accel test)
void appBattery(void);		// "all" battery info
void appNTPTime(void);		// set time using NTP
void appMQTT(void);		// MQTT app
void appLife(void);		// Conway's game of life
void appPaint(void);		// paint program
void skinMenu(void);		// set clock style ("skin")
void appStopWatch(void);	// stop watch
void alarmSettings(void);	// alarm clock settings
void appCalculator(void);	// calculator
void switch_menu(void);		// switch apps menu page
void appBitcoin(void);		// Bitcoin value checker
void resetStepCounter(void);

void LCARS_Time(uint8_t);
void Basic_Time(uint8_t);
void Analog_Time(uint8_t);
void charge_icon (uint16_t, uint16_t, uint16_t, uint16_t, boolean);
void battery_icon(uint16_t, uint16_t, uint16_t, uint16_t);
void steps_icon (uint16_t, uint16_t, uint16_t, uint16_t, uint8_t);
void draw_step_counter_ljust (uint16_t, uint16_t, uint16_t, uint16_t, uint8_t);
void draw_step_counter_rjust (uint16_t, uint16_t, uint16_t, uint16_t, uint8_t);

void quickBuzz(void);
void beep(int8_t);
void draw_keyboard(uint8_t, const char **, uint8_t, bool, char *);
void flash_keyboard_item (const char **, uint8_t, bool, int, int);
int get_numerical_button_press(void);
void dummyTime(void);
void bright_check (void);
void my_idle(void);
void draw_button_menu (uint8_t, struct menu_item *, uint8_t, uint8_t, bool, char *, struct mqdata *, uint8_t);
void flash_menu_item (uint8_t, struct menu_item *, uint8_t, uint8_t, bool, int, int, struct mqdata *, boolean, uint8_t);
void enable_rtc_alarm(void);
void disable_rtc_alarm(void);
void Serial_timestamp(void);
int cvt_12_hour_clock(int);

EXTERN boolean date_is_up;
EXTERN boolean steps_is_up;
EXTERN boolean initialAnalog;
EXTERN char buff[512];
EXTERN unsigned int half_width;
EXTERN unsigned int half_height;
EXTERN TFT_eSPI *tft;
EXTERN AXP20X_Class *power;
EXTERN uint8_t screen_brightness;
EXTERN boolean charge_cable_connected;
extern uint8_t number_of_sounds; // leave lower case, defined in play_sound.cpp
EXTERN boolean alarm_active;
EXTERN uint32_t next_beep;	// the next time the alarm beep should sound
EXTERN struct menu_item *app_menu_ptr; // which watch_apps menu is in current use?
EXTERN const char **app_label_ptr; // which app_labels array is in current use?

// this maps NUMBER to app.  The name field is just decorative (used for debug).
EXTERN struct menu_item watch_apps[]
#ifdef __MAIN__
= {
    { "StopWatch",   "", (void *)&appStopWatch },
    { "MQTT Ctrl",   "", (void *)&appMQTT },
    { "Battery",     "", (void *)&appBattery },
    { "Reset Steps", "", (void *)resetStepCounter },
    { "Calc",        "", (void *)&appCalculator },
    { "NTP Time",    "", (void *)&appNTPTime },
    { "Skin Select", "", (void *)&skinMenu },
    { "Level",       "", (void *)&appLevel },
    { "Alarm Set.",  "", (void *)&alarmSettings },
    { "Apps 2",      "", (void *)&switch_menu },
    { "Settings",    "", (void *)&appSettings },
    { "Clock",       "", NULL }
}
#endif
;

EXTERN const char *app_labels[]
#ifdef __MAIN__
		      = {
			  "Stopwatch", "MQTT", "Battery",
			  "ResetStep", "Calc", "NTP",
			  "Skin", "Level", "Alarm",
			  "Apps 2", "Settings", "Clock"
			}
#endif
;


EXTERN struct menu_item watch_apps2[]
#ifdef __MAIN__
= {
    { "Jupiter",     "", (void *)&jSats },
    { "",            "", NULL },
    { "Maze",        "", (void *)&appMaze },
    { "",            "", NULL },
    { "Paint",       "", (void *)&appPaint },
    { "",            "", NULL },
    { "Life",        "", (void *)&appLife },
    { "",            "", NULL },
    { "Bitcoin",     "", (void *)&appBitcoin },
    { "Apps 1",      "", (void *)&switch_menu },
    { "",            "", NULL },
    { "Clock",       "", NULL }
}
#endif
;

EXTERN const char *app_labels2[]
#ifdef __MAIN__
		      = {
			  "Jupiter", "", "Maze",
			  "", "Paint", "",
			  "Life", "", "BTC",
			  "Apps 1", "", "Clock"
			}
#endif
;

EXTERN TTGOClass *ttgo;
EXTERN uint8_t hh, mm, ss, mmonth, dday, gdow; // H, M, S variables
EXTERN uint16_t yyear; // Year is 16 bit int
EXTERN uint32_t last_activity;
EXTERN uint8_t chosen_skin;
EXTERN int step_counter;
EXTERN int last_step_counter;

#define CONFIG_REVISION 12347L
typedef struct config_t {
    long magic_number;
    uint8_t clock_skin;
    uint8_t mqtt_server[4];
    char mqtt_user[20];
    char mqtt_pass[20];
    uint16_t mqtt_port;
    uint8_t default_brightness;
    uint8_t screensaver_timeout;	// in seconds
    uint8_t stepcounter_filter;		// 0 to disable, 1,2,3,4 for filters
    uint32_t home_tzindex;
    uint32_t local_tzindex;
    boolean alarm_enable;
    uint8_t alarm_h;
    uint8_t alarm_m;
    int8_t alarm_days[7];
    int8_t alarm_sound;
    int8_t alarm_volume;
    int8_t twelve_hr_clock;
} CONFIGGEN;

EXTERN CONFIGGEN general_config;

enum LV_THING { NILEVENT=0, BUTTON, SLIDER, KEYBOARD, DROPDOWN, SWITCH };

enum SWIPE_DIR { NODIR=31, UP, DOWN, LEFT, RIGHT, CWCIRCLE, CCWCIRCLE };

EXTERN const char *swipe_names[]
#ifdef __MAIN__
= { "nodir", "up", "down", "left", "right" }
#endif
;

int poll_swipe_or_menu_press(int);

#define DL(x) Serial.print(x)
#define DLn(x) Serial.println(x)
#define DLF(x) Serial.print(F(x))
#define DLFn(x) Serial.println(F(x))
#define DV(m, v) do{Serial.printf("%s %s", m, v);}while(0)
// #define DV(m, v) do{Serial.print(m);Serial.print(v);Serial.print(" ");}while(0)
#define DVn(m, v) do{Serial.printf("%s %s\n", m, v);}while(0)
// #define DVn(m, v) do{Serial.print(m);Serial.println(v);}while(0)
#define DVF(m, v) do{Serial.print(F(m));Serial.print(v);Serial.print(F(" "));}while(0)
#define DVFn(m, v) do{Serial.print(F(m));Serial.println(v);}while(0)

#endif
