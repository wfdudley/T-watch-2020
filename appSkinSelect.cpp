// The clock skin menu code
// If you add a skin, this is where you will update the
// framework code to include it in the menu.
// heavily modified from Dan Geiger's code by
// W.F.Dudley Jr.

#include "config.h"
#include "DudleyWatch.h"

void setSkinDisplay(int);

typedef struct skin {
    String name;
    uint8_t skindex;
} SKIN;

SKIN skins[] = {
    { "LCARS", LCARS },
    { "Basic", BASIC },
    { "Analog", ANALOG },
    { "Lilygo", LILYGO }
};

const int maxSkins = sizeof(skins)/sizeof(SKIN); // number of skins

void skinMenu(void) {
int mSelect;
int16_t x, y;

  boolean exitMenu = false; // used to stay in the menu until user selects app

  draw_button_menu(1, skin_menu, 1, 2, true, "Pick a Watch Style", NULL, 1);
  do {
    mSelect = poll_swipe_or_menu_press(12);	// poll for touch or gesture
#if 0
    if(mSelect > -1) {
      Serial.printf("poll_swipe... returns %d\n", mSelect);
    }
#endif
    if(mSelect > -1 && mSelect < NODIR) {	// if user touched something
      switch(mSelect) {
	case 0 :
	case 1 :
	case 2 :
	  // Serial.println(F("0 1 2 -> ANALOG"));
	  // this is neat, but we store chosen_skin in general_config, so . . .
	  // watch_fun_pointer = (void (*)(uint8_t))skin_menu[0].next_menu;
	  chosen_skin = ANALOG;
	  break;
	case 3 :
	case 4 :
	case 5 :
	  // Serial.println(F("3 4 5 -> LCARS"));
	  // watch_fun_pointer = (void (*)(uint8_t))skin_menu[1].next_menu;
	  chosen_skin = LCARS;
	  break;
	case 6 :
	case 7 :
	case 8 :
	  // Serial.println(F("6 7 8 -> BASIC"));
	  // watch_fun_pointer = (void (*)(uint8_t))skin_menu[2].next_menu;
	  chosen_skin = BASIC;
	  break;
	case 9 :
	case 10 :
	case 11 :
	  chosen_skin = LILYGO;
	  // watch_fun_pointer = (void (*)(uint8_t))skin_menu[3].next_menu;
	  // Serial.println(F("9 10 11 -> LilyGo"));
	  break;
      }
      goto Exit;
    }
    // swipe is 32,33,34,35 (UP, DOWN, etc)
  } while(1);

Exit:
  tft->fillScreen(TFT_BLACK);
  general_config.clock_skin = chosen_skin;
  EEPROM_writeAnything(0, general_config);
  EEPROM.commit();
  return;
}
