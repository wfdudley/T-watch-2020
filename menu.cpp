// Dan Geiger's app menu code
// If you add an app, this is where you will update the
// framework code to include it in the menu.
//
// Make the following updates:
// 1) Add a line to watch_apps[]
// 2) write your app function
//
// heavily modified by W.F.Dudley Jr.

#include "config.h"
#include "DudleyWatch.h"

void dummyTime(void) {}

uint8_t modeMenu(void) {
int mSelect = 0; // The currently highlighted app
  draw_keyboard(app_labels, 1, true, "pick an app");
  while(1) {
    mSelect = poll_swipe_or_menu_press(12); // poll for touch/swipe, returns 0-15
    // mSelect = get_numerical_button_press();
    if (mSelect != -1 && mSelect < 11) {
      int row, col;
      row = mSelect / 3;
      col = mSelect % 3;
      flash_keyboard_item(app_labels, 1, true, row, col);
      Serial.printf("poll_swipe_or_menu_press -> %d, app should be %s\n",
	mSelect, watch_apps[mSelect].name);
      return mSelect;
    }
    else if (mSelect == 11) {
      flash_keyboard_item(app_labels, 1, true, 3, 2);
      return 0x1b;
    }
    my_idle();
  }
  return 0;	// default is to go back to clock
}

