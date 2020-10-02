// simple Polish Postfix four function calculator
#include <math.h>
#include "config.h"
#include "DudleyWatch.h"

const char *calc_labels1[]
		      = {
			  "1", "2", "3", "+",
			  "4", "5", "6", "-",
			  "7", "8", "9", "*",
			  ".", "0", "y^x", "/",
			}
;

const char button_values1[] = {'1', '2', '3', '+',
			       '4', '5', '6', '-',
			       '7', '8', '9', '*',
			       '.', '0', '^', '/' };

const char *calc_labels2[]
		      = {
			  "sin", "cos", "tan", "chs",
			  "asin", "acos", "atan", "sqr",
			  "ln", "log", "e^x", "mod",
			  "x<>y", "flr", "2^x", "clx",
			}
;

const char button_values2[] = {'s', 'c', 't', '_',
			       'A', 'C', 'T', 'q',
			       'l', 'L', 'e', 'm',
			       'w', 'f', 'E', 'x' };

// todo:
// second keyboard with functions like "CHS" (change sign), sin, cos, tan

void draw_keyboard_16 (const char **b_labels, uint8_t font, bool leave_room_for_label, char *top_label) {
uint8_t yvals[4], yh, row, col;
uint16_t icolor;
  ttgo->tft->fillScreen(TFT_BLACK);
  if(leave_room_for_label) {
    yvals[0] = 35;
    yvals[1] = 85;
    yvals[2] = 135;
    yvals[3] = 185;
    yh = 50;
    if(top_label) {
      ttgo->tft->setTextColor(TFT_GREEN);
      ttgo->tft->drawCentreString( top_label, half_width, 5, font);
    }
  }
  else {
    yvals[0] =   0;
    yvals[1] =  60;
    yvals[2] = 120;
    yvals[3] = 180;
    yh = 60;
  }
  int16_t xvals[4] = { 0, 62, 124, 186 };
  int16_t xtvals[4] = { 31, 91, 152, 218 };
  // note: space at the top do display what is typed
  // was ttgo->tft->fillRect(0, 35, 80, 50, TFT_BLUE);
  // number keys are 80 x 50, four rows of three
  // x=0, 81, 161, y=35, 85, 135, 185
  ttgo->tft->setTextColor(TFT_GREEN);
  for(row = 0 ; row < 4 ; row++) {
    for(col = 0 ; col < 4 ; col++) {
      int ino = col + (row * 4);
      icolor = (!strcmp(b_labels[ino], "CANCEL") || !strcmp(b_labels[ino], "DONE")) ? TFT_DARKGREY : TFT_BLUE ;
      ttgo->tft->fillRoundRect(xvals[col], yvals[row], 55, yh-5, 6, icolor);
      ttgo->tft->drawCentreString(b_labels[ino], xtvals[col], yvals[row]+5, font);
    }
  }
}

static float stack[5];

void push(float x) {
    for(int i = (sizeof(stack)/sizeof(float)) - 1 ; i > 0 ; i--) {
	stack[i] = stack[i - 1];
    }
    stack[0] = x;
}

float pop(void) {
float r;
    r = stack[0];
    for(int i = 0 ; i < (sizeof(stack)/sizeof(float)) - 1 ; i++) {
	stack[i] = stack[i + 1];
    }
    return(r);
}

void appCalculator(void) {
int mSelect = 0;
char c, *cp, ibuf[20], buf[200];
float x;
boolean needs_push;
int last_len, current_kbd;

    current_kbd = 0;
    for(int i = 0 ; i < sizeof(stack)/sizeof(float) ; i++) {
	stack[i] = 0.0;
    }
    memset(ibuf, '\0', sizeof(ibuf)/sizeof(char));
    memset(buf, '\0', sizeof(buf)/sizeof(char));
    draw_keyboard_16(current_kbd ? calc_labels2 : calc_labels1, 4, true, "");
    do {
	my_idle();
	mSelect = poll_swipe_or_menu_press(16); // poll for touch/swipe, returns 0-19
	c = 0xff;
	if(mSelect >= 0 && mSelect < 16) {
	  if(current_kbd) {
	    c = button_values2[mSelect];
	  }
	  else {
	    c = button_values1[mSelect];
	  }
	}
	else if(mSelect == 15 + LEFT) {	// swipe left == backspace
	  c = 0x08;
	}
	else if(mSelect == 15 + UP) {	// swipe up == pop
	  c = 'P';
	}
	else if(mSelect == 15 + DOWN) {	// swipe down == push
	  c = '=';
	}
	else if(mSelect == 15 + CWCIRCLE) {	// swipe cw == pi
	  c = 'p';
	}
	else if(mSelect == 15 + CCWCIRCLE) {	// swipe ccw -> switch kbd
	  current_kbd ^= 1;
	  draw_keyboard_16(current_kbd ? calc_labels2 : calc_labels1, 4, true, buf);
	  // tft->setTextColor(TFT_GREEN);
	  // tft->drawCentreString( buf, x, 5, 4);
	}
	// use CCWCIRCLE to switch to function keyboard (or back again)
	if(c != 0xff) {
	  Serial.printf("c = %02x = '%c'\n", c, c);
	  switch(c) {
	      case '0' : case '1' : case '2' : case '3' : case '4' :
	      case '5' : case '6' : case '7' : case '8' : case '9' :
	      case '.' :
		if(needs_push) {
		  memset(ibuf, '\0', sizeof(ibuf)/sizeof(char));
		  push(0.0);
		  needs_push = 0;
		  tft->fillRect(0, 5, 240, 10, TFT_BLACK);
		}
		ibuf[strlen(ibuf)] = c;
		ibuf[strlen(ibuf)+1] = '\0';
		stack[0] = atof(ibuf);
		Serial.printf("ibuf = %s\n", ibuf);
		break;
	      case 'P' :	// Pop
		pop();
		break;
	      case 'p' :	// pi
		stack[0] = 4.0 * atan(1);
		break;
	      case 0x08 :	// erase (backspace)
		if(strlen(ibuf)) {
		  ibuf[strlen(ibuf)-1] = '\0';
		  Serial.printf("backspace -> ibuf = %s\n", ibuf);
		  stack[0] = atof(ibuf);
		}
		break;
	      case '=' :	// push
		push(0.0);
		memset(ibuf, '\0', sizeof(ibuf)/sizeof(char));
		needs_push = 0;
		break;
	      case '+' :
		stack[0] += pop();
		Serial.printf("add -> %.2f\n", stack[0]);
		needs_push = 1;
		break;
	      case '-' :
		x = pop();
		stack[0] -= x;
		Serial.printf("subtract -> %.2f\n", stack[0]);
		needs_push = 1;
		break;
	      case '*' :
		stack[0] *= pop();
		Serial.printf("multiply -> %.2f\n", stack[0]);
		needs_push = 1;
		break;
	      case '/' :
		x = pop();
		stack[0] /= x;
		Serial.printf("divide -> %.2f\n", stack[0]);
		needs_push = 1;
		break;
	      case '^' :	// exponentiation
		x = pop();
		stack[0] = pow(stack[0], x);
		needs_push = 1;
		break;
	      case '_' :	// change sign
		stack[0] = 0.0 - stack[0];
		needs_push = 1;
		break;
	      case 'l' :	// ln base e
		stack[0] = log(stack[0]);
		needs_push = 1;
		break;
	      case 'L' :	// log base 10
		stack[0] = log10(stack[0]);
		needs_push = 1;
		break;
	      case 'e' :	// e ^^ x
		stack[0] = exp(stack[0]);
		needs_push = 1;
		break;
	      case 's' :	// sine
		stack[0] = sin(stack[0]);
		needs_push = 1;
		break;
	      case 'c' :	// cosine
		stack[0] = cos(stack[0]);
		needs_push = 1;
		break;
	      case 't' :	// tangent
		stack[0] = tan(stack[0]);
		needs_push = 1;
		break;
	      case 'S' :	// arcsine
		stack[0] = asin(stack[0]);
		needs_push = 1;
		break;
	      case 'C' :	// arccosine
		stack[0] = acos(stack[0]);
		needs_push = 1;
		break;
	      case 'T' :	// arctangent
		stack[0] = atan(stack[0]);
		needs_push = 1;
		break;
	      case 'f' :	// floor
		stack[0] = floor(stack[0]);
		needs_push = 1;
		break;
	      case 'E' :	// 2 ^^ x
		stack[0] = pow(2.0, stack[0]);
		needs_push = 1;
		break;
	      case 'q' :	// square root
		stack[0] = sqrt(stack[0]);
		needs_push = 1;
		break;
	      case 'x' :	// clear x
		stack[0] = 0.0;
		needs_push = 1;
		break;
	      case 'm' :	// modulo
		x = pop();
		stack[0] = fmod(stack[0], x);
		needs_push = 1;
		break;
	      case 'w' :	// swap x and y
		x = stack[0];
		stack[0] = stack[1];
		stack[1] = x;
		needs_push = 1;
		break;
	  }
#define DBG_CALC 1
#if DBG_CALC
	  Serial.print(F("stack "));
	  for(int i = 0 ; i < sizeof(stack)/sizeof(float) ; i++) {
	      Serial.printf("%.2f, ", stack[i]);
	  }
	  Serial.printf("\n");
#endif
	  sprintf(buf, "%.4f", stack[0]);
	  // if(last_len != strlen(buf)) {
	    tft->fillRect(0, 5, 240, 20, TFT_BLACK);
	  // }
	  tft->setTextColor(TFT_GREEN);
	  tft->drawCentreString( buf, x, 5, 4);
	  last_len = strlen(buf);
	}
	else if(mSelect == 15 + RIGHT) {	// exit
	    tft->fillScreen(TFT_BLACK);		// clear screen
	    return;
	}
    } while(1);
}
