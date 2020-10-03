/*
 tetris on the TTGO OLED device
 */
/*
 * originally:
 * Tetris game on an Adafruit RGB LCD Shield
 * https://github.com/dmalec/RgbLcdShieldTriscalino
 *
 * Forked/ported from https://github.com/dzimboum/triscalino
 * Originally implemented on the great Deuligne shield, by Snootlab.
 *
 * Copyright 2012 dzimboum
 * Released under the WTFPL 2.0 license
 * http://sam.zoy.org/wtfpl/COPYING
 *
 * Ported by Dan Malec to the Adafruit RGB LCD Shield
 * - Changed type of lcd (and related includes) to Adafruit_RGBLCDShield
 * - Changed type of hiScores (and related includes) to RgbLcdShieldHiScores
 * - Changed button logic to work with Adafruit Shield
 * - Changed lcd.init() call to lcd.begin(16, 2)
 * - Added setting of backlight color 
 * - Added backlight color enum
 *
 * hacked to work with ttgo oled screen - W.F.Dudley 2019 10 17
 *
 * ****************************************************************************
 * Dependencies
 * ****************************************************************************
 *
 * ****************************************************************************
 */

// #include <Wire.h>
#include <inttypes.h>
#include <string.h>
// #include <TFT_eSPI.h>
// #include <SPI.h>
#include "config.h"
#include "DudleyWatch.h"

#define Black      0x0000
#define White      0xFFFF
#define Light_Gray 0xBDF7
#define Dark_Gray  0x7BEF
#define Darker_Gray  0x0841
#define Red        0xF800
#define Yellow     0xFFE0
#define Orange     0xFBE0
#define Brown      0x79E0
#define Green      0x07E0
#define Cyan       0x07FF
#define Blue       0x001F
#define Pink       0xF81F

#define TRANSPARENT White

#define DBGLIFE 0

void randomize(void);
void moveCursorUp(void);
void moveCursorDown(void);
void moveCursorLeft(void);
void moveCursorRight(void);
void invertCurrentCell(void);
void next(void);

#define BLOCKWIDTH 4
#define BLOCKHEIGHT 4

const int COLS = 60; // 240 / BLOCKWIDTH
const int ROWS = 60; // 240 / BLOCKHEIGHT

boolean current_state[ROWS][COLS]; // stores the current state of the cells

boolean play = false; // if true, application will automatically advance to the next state

int lastR; // last cursor row
int lastC; // last cursor column

extern void espDelay(int);


static uint8_t touched;

static void my_delay(unsigned int dtime) {
  do {
    my_idle();
    delay(5);
    dtime -= 5;
  } while(dtime > 0);
}

static void draw_block (int bx, int by, boolean erase, int color) {
  tft->fillRect(bx * BLOCKWIDTH, by * BLOCKHEIGHT, BLOCKWIDTH, BLOCKHEIGHT, erase ? TRANSPARENT : color);
}

void life_setup(void) {
#if DBGLIFE
  Serial.println("life_setup()");
#endif

  touched = 0;

  lastR = ROWS / 2; // last cursor row
  lastC = COLS / 2; // last cursor column

  // make the background white
  tft->fillScreen(White);
  // tft->setTextColor(TFT_GREEN, TFT_GREY);  // Adding a black background colour erases previous text automatically

// targetTime = millis() + 1000; 
  // spr.createSprite(BLOCKWIDTH, BLOCKHEIGHT);

  tft->setTextColor(Black);
  tft->fillRect(0, 130, 135, 20, White);
  tft->drawCentreString("Setup ok", half_width, 130, 4);
  delay(1000);
  tft->fillRect(0, 130, 135, 20, White);

  // create seed to use the random function
  int seed = esp_random();
#if DBGLIFE
  Serial.print(F("random seed: "));
  Serial.println(seed);
#endif
  randomSeed(seed);

  // initialize cells with random values
  randomize();
  play = true;
}

/**
 * Main application loop
 *
 * First, check if cursor needs to move in any direction.
 * If it does, move the cursor and apply a small delay, so that it
 * does not move too fast.
 *
 * Then check for each input button to see if any action must be performed.
 *
 * In case the application is in PLAY state, transition to the next
 * state automatically.
 */

void life_loop(void) {
int16_t x, y;
  my_idle();
  if(ttgo->getTouch(x, y)) {
    touched = true;
  }
  if (play) {
    next();
  }
}

void appLife(void) {
int16_t x, y;
  life_setup();
  do {
    life_loop();
  } while(!touched);
  while (ttgo->getTouch(x, y)) {	// Wait for release
    my_idle();
  }
  tft->fillScreen(TFT_BLACK);	// Clear screen 
}

/**
 * This method transitions the current state to the next state, using the following rules:
 *
 * 1. Any live cell with fewer than two live neighbours dies, as if caused by underpopulation.
 * 2. Any live cell with two or three live neighbours lives on to the next generation.
 * 3. Any live cell with more than three live neighbours dies, as if by overpopulation.
 * 4. Any dead cell with exactly three live neighbours becomes a live cell, as if by reproduction.
 *
 * For the purpose of this application, live == TRUE and dead == FALSE
 */
void next(void) {
  int x;
  int y;
  boolean highlighted;
  boolean value;
  boolean next[ROWS][COLS]; // stores the next state of the cells

  for (int r = 0; r < ROWS; r++) { // for each row
    for (int c = 0; c < COLS; c++) { // and each column
      // count how many live neighbors this cell has
      int liveNeighbors = 0;
      for (int i = -1; i < 2; i++) {
        y = r + i;
        if (y == -1) {
          y = ROWS-1;
        } else if (y == ROWS) {
          y = 0;
        }
        for (int j = -1; j < 2; j++) {
          if (i != 0 || j != 0) {
            x = c + j;
            if (x == -1) {
              x = COLS-1;
            } else if (x == COLS) {
              x = 0;
            }

            if (current_state[y][x]) {
              liveNeighbors++;
            }
          }
        }
      }

      // apply the rules
      if (current_state[r][c] && liveNeighbors >= 2 && liveNeighbors <= 3) { // live cells with 2 or 3 neighbors remain alive
        value = true;
      } else if (!current_state[r][c] && liveNeighbors == 3) { // dead cells with 3 neighbors become alive
        value = true;
      } else {
        value = false;
      }

      next[r][c] = value;

      // checks if the cursor is on top of this cell
      highlighted = (r == lastR && c == lastC);

      // draw the cell
      int color;
      if(value) {
	color = (highlighted) ? Green : Black ;
      }
      else {
	color = (highlighted) ? Orange : White ;
      }
      draw_block(c, r, 0, color);
    }
  }

  // discard the old state and keep the new one
  memcpy(current_state, next, sizeof next);
}

/**
 * Move the cursor one cell to the left.
 * If this is the last cell, start on the other side of the grid.
 */
void moveCursorLeft(void) {
int color;
  color = (current_state[lastR][lastC]) ? Black : White ;
  draw_block(lastC, lastR, 0, color);
  lastC = lastC == 0 ? COLS-1 : lastC - 1;
  color = (current_state[lastR][lastC]) ? Orange : Black ;
  draw_block(lastC, lastR, 0, color);
}

/**
 * @see moveCursorLeft
 */
void moveCursorRight(void) {
int color;
  color = (current_state[lastR][lastC]) ? Black : White ;
  draw_block(lastC, lastR, 0, color);
  lastC = lastC == COLS-1 ? 0 : lastC + 1;
  color = (current_state[lastR][lastC]) ? Orange : Black ;
  draw_block(lastC, lastR, 0, color);
}

/**
 * @see moveCursorLeft
 */
void moveCursorUp(void) {
int color;
  color = (current_state[lastR][lastC]) ? Black : White ;
  draw_block(lastC, lastR, 0, color);
  lastR = lastR == ROWS-1 ? 0 : lastR + 1;
  color = (current_state[lastR][lastC]) ? Orange : Black ;
  draw_block(lastC, lastR, 0, color);
}

/**
 * @see moveCursorLeft
 */
void moveCursorDown(void) {
int color;
  color = (current_state[lastR][lastC]) ? Black : White ;
  draw_block(lastC, lastR, 0, color);
  lastR = lastR == 0 ? ROWS-1 : lastR - 1;
  color = (current_state[lastR][lastC]) ? Orange : Black ;
  draw_block(lastC, lastR, 0, color);
}

/**
 * Inverts the state (alive, dead) of the cell in which the cursor is currently on
 */
void invertCurrentCell(void) {
  current_state[lastR][lastC] = !current_state[lastR][lastC];
int color;
  color = (current_state[lastR][lastC]) ? Orange : Black ;
  draw_block(lastC, lastR, 0, color);
}

/**
 * Creates a random state.
 * The slider can be used to control the percentage of alive and dead cells.
 * The more to the right the slider is, the more alive cells are created.
 */
void randomize(void) {
  int slider = 35;
  int num;
  boolean value;
  boolean highlighted;

  for (int r = 0; r < ROWS; r++) {
    for (int c = 0; c < COLS; c++) {
      num = random(1, 100);

      value = num >= slider;
      current_state[r][c] = value;

      highlighted = (r == lastR && c == lastC);

      int color;
      if(value) {
	color = (highlighted) ? Green : White ;
      }
      else {
	color = (highlighted) ? Orange : Black ;
      }
      draw_block(c, r, 0, color);
    }
  }
}
