// Written by FabriceA6
// https://www.instructables.com/Mandelbrot-and-Julia-Sets-on-ESP32/
// November 2019
// tweaked to run on TTGO-T-watch 2020 by W.F.Dudley October 2020

#include "config.h"
#include "DudleyWatch.h"

// choose the floating point resolution here:
typedef float FPREC;	// double or float
// float makes screen draw take up to 1 second
// double makes screen draw take between 5 and 15 seconds

#define FRENCH 0	// show English

// Dimensions
#define NUM_BUTTONS 7
const int TFT_Width  = 240;
const int TFT_Height = 240;
const int TFT_MINX   =   0;
const int TFT_MAXX   = TFT_WIDTH - (TFT_HEIGHT/NUM_BUTTONS);
const int TFT_MINY   =   0;
const int TFT_MAXY   = TFT_Height;
int boutonY;
#define NUMBER_COLORS 6
enum colors {RED, GREEN, BLUE, GRAY, PAL1, PAL2};
byte Palette = 1;
#define BUTTON_HEIGHT TFT_Height/7

// Touch
boolean wastouched = true;

// Mandel Paramaters
const FPREC Mandel_MINX = -2.1;
const FPREC Mandel_MAXX =  0.7;
const FPREC Mandel_MINY = -1.2;
const FPREC Mandel_MAXY =  1.2;
FPREC Current_MINX = Mandel_MINX;
FPREC Current_MINY = Mandel_MINY;
FPREC Current_MAXX = Mandel_MAXX;
FPREC Current_MAXY = Mandel_MAXY;
FPREC Prev_MINX = Current_MINX;
FPREC Prev_MINY = Current_MINY;
FPREC Prev_MAXX = Current_MAXX;
FPREC Prev_MAXY = Current_MAXY;
FPREC Jcx = 0.3;
FPREC Jcy = 0.01;
int itermin = 100;
int itermax = 0;
boolean Mandel = true;
byte Puissance = 2;
#define Num_puiss 6
#define Max_iter 64
#define Escape 100
char texte[40];
int Calcul_Mandel (FPREC, FPREC);
uint32_t Couleur(int, int, int);

void Draw_Mandel (void) {
  unsigned long temps = millis();
  FPREC deltaX = (Current_MAXX - Current_MINX) / (TFT_MAXX - TFT_MINX - 1);
  FPREC deltaY = (Current_MAXY - Current_MINY) / (TFT_MAXY - TFT_MINY - 1);
  for (int i = TFT_MINX; i < TFT_MAXX; i++) { // Premier tracé à blanc
    FPREC cx = Current_MINX + i * deltaX;
    for (int j = TFT_MINY; j < TFT_MAXY; j++) {
      FPREC cy = Current_MINY + j * deltaY;
      int iteration = Calcul_Mandel (cx, cy);
      if (iteration < itermin && iteration > 0) itermin = iteration;
      if (iteration > itermax) itermax = iteration;
      uint32_t color = Couleur(iteration, 0, Max_iter);
      tft->drawPixel(i, j, color);
    }
  }
  temps = millis() - temps;
  if (!Mandel) {
    tft->setTextSize(1);
    tft->setTextColor(TFT_WHITE);
    sprintf (texte, "Coefficient : ( %f , %f )", Jcx, Jcy);
    tft->drawString(texte, 25, 5);
    tft->setTextSize(2);
  }
#if FRENCH
  Serial.print (F("Temps de calcul : "));
#else
  Serial.print (F("Time to calculate : "));
#endif
  Serial.print(temps);
  Serial.println(F(" ms."));
}

int Calcul_Mandel (FPREC cx, FPREC cy) {
  FPREC x1 = 0;
  FPREC y1 = 0;
  FPREC x2 = 0;
  FPREC y2 = 0;
  if (!Mandel) {
    x1 = cx;
    y1 = cy;
    x2 = cx;
    y2 = cy;
    cx = Jcx;
    cy = Jcy;
  }
  FPREC temp = 0;
  int iteration = 0;

  while (true) {
    if (iteration >= Max_iter) return Max_iter;
    if (x2 * x2 + y2 * y2 > Escape) return iteration;
    for (int i = 0; i < Puissance - 1; i++) { // Calcul z**n
      temp = x1 * x2 - y1 * y2;
      y2 = x1 * y2 + x2 * y1;
      x2 = temp;
    }
    x1 = x2 + cx;
    y1 = y2 + cy;
    x2 = x1;
    y2 = y1;
    ++iteration;
  } // Fin Mandelbrot
}

uint32_t Couleur(int i, int mini, int maxi) {
  byte red = 0;
  byte green = 0;
  byte blue = 0;
  // color picker : https://www.w3schools.com/colors/colors_picker.asp
  byte col = 255 - map(i, mini, maxi, 0, 255); // de 0 à 255
  switch (Palette) {
    case RED :
      red   = col;
      green = 0;
      blue  = 0;
      break;
    case GREEN :
      red   = 0;
      green = col % 64;
      blue  = 0;
      break;
    case BLUE :
      red   = 0;
      green = 0;
      blue  = col % 32;
      break;
    case GRAY :
      red   = col % 50;
      green = red * 2;
      blue  = red;
      break;
    case PAL1 :
      red   = col % 60;
      green = col % 40;
      blue  = col % 10;
      break;
    case PAL2 : // passage bleu - vert - rouge
      red   = 0;
      green = 0;
      blue  = 0;
      if (col < 33) {
        red  = col;
      } else if (col < 200) {
        green  = col % 64; // / 4;
      } else {
        blue  = col - 200;
      }
      break;
  }
  return red << 11 | green << 5 | blue; //tft->color565(red, green, blue);
}

// Init_Screen
void Init_Ecran(void) {
  int delta = boutonY / 2 - 5;
  tft->fillRect(TFT_MAXX,  0, TFT_Width - TFT_MAXX, TFT_Height, TFT_WHITE);
  tft->setTextColor(TFT_BLACK);
  tft->setTextSize(2);
  tft->drawString("R", TFT_MAXX + delta, delta);
  tft->drawString("U", TFT_MAXX + delta, boutonY + delta);
  tft->drawNumber(2 + (Puissance - 1) % Num_puiss, TFT_MAXX + delta, 4 * boutonY + delta);
  if (Mandel) { // Tracé Mandelbrot
    tft->drawString("J", TFT_MAXX + delta, 3 * boutonY + delta);
  } else { // Tracé Julia
    tft->drawString("M", TFT_MAXX + delta, 3 * boutonY + delta);
    tft->drawString("C", TFT_MAXX + delta, 5 * boutonY + delta);
  }
#if DEBUG_BUTTONS
  Serial.printf("put R at %d, %d\n", TFT_MAXX + delta, delta);
  Serial.printf("put U at %d, %d\n", TFT_MAXX + delta, boutonY + delta);
  Serial.printf("put J at %d, %d\n", TFT_MAXX + delta, 3 * boutonY + delta);
  Serial.printf("put %d at %d, %d\n", 2 + (Puissance - 1) % Num_puiss, TFT_MAXX + delta, 4 * boutonY + delta);
  Serial.printf("put C at %d, %d\n", TFT_MAXX + delta, 5 * boutonY + delta);
  Serial.printf("put E at %d, %d\n", TFT_MAXX + delta, 6 * boutonY + delta);
#endif
  tft->drawString("E", TFT_MAXX + delta, 6 * boutonY + delta);
  switch (Palette) {
    case RED :
      tft->fillRect(TFT_MAXX, 2 * boutonY, boutonY, boutonY, TFT_GREEN);
      break;
    case GREEN :
      tft->fillRect(TFT_MAXX, 2 * boutonY, boutonY, boutonY, TFT_BLUE);
      break;
    case BLUE :
      tft->fillRect(TFT_MAXX, 2 * boutonY, boutonY, boutonY, TFT_DARKGREY);
      break;
    case GRAY :
      tft->fillRect(TFT_MAXX, 2 * boutonY, boutonY, boutonY, TFT_YELLOW);
      tft->drawNumber(1, TFT_MAXX + delta, 2 * boutonY + delta);
      break;
    case PAL1 :
      tft->fillRect(TFT_MAXX, 2 * boutonY, boutonY, boutonY, TFT_YELLOW);
      tft->drawNumber(2, TFT_MAXX + delta, 2 * boutonY + delta);
      break;
    case PAL2 :
      tft->fillRect(TFT_MAXX, 2 * boutonY, boutonY, boutonY, TFT_RED);
      break;
  }
  for (int i = 0; i < NUM_BUTTONS; i++) {
    tft->drawRect(TFT_MAXX, i * boutonY, boutonY, boutonY, TFT_BLACK);
  }
}

void VisuZone (int i, int j) {
  tft->drawCircle(i, j, 4, TFT_WHITE);
  tft->drawRect(i - (TFT_MAXX - TFT_MINX) / 10, j - (TFT_MAXY - TFT_MINY) / 10,
               (TFT_MAXX - TFT_MINX) / 5, (TFT_MAXY - TFT_MINY) / 5, TFT_WHITE);
}

void CalculZone (int xtft, int ytft) {
  FPREC coefX = (Current_MAXX - Current_MINX) / (TFT_MAXX - TFT_MINX);
  FPREC Mandel_centerX = Current_MINX + (xtft - TFT_MINX) * coefX;
  FPREC coefY = (Current_MAXY - Current_MINY) / (TFT_MAXY - TFT_MINY);
  FPREC Mandel_centerY = Current_MINY + (ytft - TFT_MINY) * coefY;
  FPREC DeltaX = (Current_MAXX - Current_MINX) / 10.;
  FPREC DeltaY = (Current_MAXY - Current_MINY) / 10.;
  Prev_MINX = Current_MINX;
  Prev_MINY = Current_MINY;
  Prev_MAXX = Current_MAXX;
  Prev_MAXY = Current_MAXY;
  Current_MINX = Mandel_centerX - DeltaX;
  Current_MAXX = Mandel_centerX + DeltaX;
  Current_MINY = Mandel_centerY - DeltaY;
  Current_MAXY = Mandel_centerY + DeltaY;
}

void ChoixJuliaC(void) {
int16_t x, y;
  tft->fillRect(0,  0, TFT_MAXX, TFT_MAXY, TFT_WHITE);
  tft->setTextSize(1);
  tft->setTextColor(TFT_BLACK);
  tft->drawString("Choisir le coefficient C", 75, 30);
  for (int i = 0; i < 4; i++) {
    tft->drawFastVLine(i * TFT_MAXX / 4, 0, TFT_MAXY, TFT_BLACK);
    tft->drawFastHLine(0, i * TFT_MAXY / 4, TFT_MAXX, TFT_BLACK);
  }
  tft->drawNumber(-2, 5, TFT_MAXY / 2 + 5);
  tft->drawNumber(-1, TFT_MAXX / 5 + 15, TFT_MAXY / 2 + 5);
  tft->drawNumber( 1, 4 * TFT_MAXX / 5, TFT_MAXY / 2 + 5);
  tft->drawNumber( 2, 5 * TFT_MAXX / 5 - 10, TFT_MAXY / 2 + 5);
  tft->drawNumber(-1, TFT_MAXX / 2 + 5, TFT_MAXY - 15);
  tft->drawNumber( 0, TFT_MAXX / 2 + 5, TFT_MAXY / 2 + 5);
  tft->drawNumber( 1, TFT_MAXX / 2 + 5, 5);

  while(!ttgo->getTouch(x, y)) { true; }
  delay(200);
  while(!ttgo->getTouch(x, y)) { true; }
  FPREC coefX = 4.0 / (TFT_MAXX - TFT_MINX);
  FPREC coefY = 2.0 / (TFT_MAXY - TFT_MINY);
  Jcx = -2.0 + x * coefX;
  Jcy =  1.0 - y * coefY;
  tft->fillCircle(x, y, 3, TFT_BLUE);
  sprintf (texte, "Coefficient : ( %f , %f )", Jcx, Jcy);
  tft->drawString(texte, 25, 50);
  delay(3000);
  FPREC dx = 0.7;
  Current_MINX = Mandel_MINX + dx;
  Current_MINY = Mandel_MINY;
  Current_MAXX = Mandel_MAXX + dx;
  Current_MAXY = Mandel_MAXY;
}

int Gestion_bouton (int y) {
int res;
  res = 0;
  // Boutons BUTTON_HEIGHT x BUTTON_HEIGHT pixels
  if (y < BUTTON_HEIGHT) { // Bouton RAZ
    Serial.println ("Remise a zero");
    FPREC dx = 0.0;
    if (!Mandel) dx = 0.7;
    if (Mandel && Puissance != 2) dx = 0.65;
    Current_MINX = Mandel_MINX + dx;
    Current_MINY = Mandel_MINY;
    Current_MAXX = Mandel_MAXX + dx;
    Current_MAXY = Mandel_MAXY;
  } else if (y < BUTTON_HEIGHT * 2) { // Bouton Undo
    Serial.println ("Precedent Zoom");
    Current_MINX = Prev_MINX;
    Current_MINY = Prev_MINY;
    Current_MAXX = Prev_MAXX;
    Current_MAXY = Prev_MAXY;
  } else if (y < BUTTON_HEIGHT * 3) { // Bouton couleur
    Palette = (Palette + 1) % NUMBER_COLORS;
  } else if (y < BUTTON_HEIGHT * 4) { // Choix Mandebrot / Julia
    Mandel = !Mandel;
    FPREC dx = 0;
    if (!Mandel) dx = 0.7;
    Current_MINX = Mandel_MINX + dx;
    Current_MINY = Mandel_MINY;
    Current_MAXX = Mandel_MAXX + dx;
    Current_MAXY = Mandel_MAXY;
  } else if (y < BUTTON_HEIGHT * 5) { // Paramètres
    Puissance = 2 + (Puissance - 1) % Num_puiss;
    FPREC dx = 0.0;
    if (Puissance != 2) dx = 0.65;
    Current_MINX = Mandel_MINX + dx;
    Current_MINY = Mandel_MINY;
    Current_MAXX = Mandel_MAXX + dx;
    Current_MAXY = Mandel_MAXY;
  } else if ( y < BUTTON_HEIGHT * 6 ) { // Si Julia : choix paramètre
    ChoixJuliaC();
  } else { // exit
    res = 1;
  }
  if(!res) {
    Init_Ecran();
    Draw_Mandel();
  }
  return res;
}

#define HELP_INDENT 10
void Infos(void) {
  tft->fillScreen(TFT_WHITE);
  tft->setTextColor(TFT_BLACK, TFT_WHITE);
  tft->setTextSize(2);
  tft->drawString("Mandelbrot - Julia", HELP_INDENT, 15);
  tft->setTextSize(1);
  byte y0 = 40;
  byte dy = 20;
#if FRENCH
  tft->drawString("Boutons de droite :", HELP_INDENT, y0 + 2 * dy);
  tft->drawString("R : Reset (full scale)", HELP_INDENT, y0 + 3 * dy);
  tft->drawString("U : Undo dernier zoom", HELP_INDENT, y0 + 4 * dy);
  tft->drawString("M/J : Toggle Mandelbrot <--> Julia", HELP_INDENT, y0 + 5 * dy);
  tft->drawString("Couleur : Diverses palettes", HELP_INDENT, y0 + 6 * dy);
  tft->drawString("Nombre : Exposant des calculs", HELP_INDENT, y0 + 7 * dy);
  tft->drawString("C : Choix du coefficient pour Julia", HELP_INDENT, y0 + 8 * dy);
#else
  // English instructions
  tft->drawString("Buttons on the right :", HELP_INDENT, y0 + 2 * dy);
  tft->drawString("R : Reset (full scale)", HELP_INDENT, y0 + 3 * dy);
  tft->drawString("U : Undo last zoom", HELP_INDENT, y0 + 4 * dy);
  tft->drawString("M/J : Toggle Mandelbrot <--> Julia", HELP_INDENT, y0 + 5 * dy);
  tft->drawString("Color : Various palettes", HELP_INDENT, y0 + 6 * dy);
  tft->drawString("2-7: Exponent for the calculations", HELP_INDENT, y0 + 7 * dy);
  tft->drawString("C : Choose coefficient for Julia set", HELP_INDENT, y0 + 8 * dy);
  tft->drawString("E : Exit app", HELP_INDENT, y0 + 9 * dy);
#endif
  delay (6000);
}

void Mandel_setup(void) {
  // boutonY = 10 * (TFT_MAXY / 60);
  boutonY = TFT_MAXY / NUM_BUTTONS;
  // Setup the LCD
  // tft->setRotation(3);
  // Init touchscreen
  //  uint16_t calData[5] = { 179, 3522, 335, 3434, 1 }; // Rotation 1
  // uint16_t calData[5] = { 195, 3500, 335, 3410, 7 }; // Rotation 3
  // tft->setTouch(calData);
  tft->fillScreen(TFT_BLACK);
  Infos();
  Init_Ecran();
  Draw_Mandel();
}

int Mandel_loop(void) {
int16_t x, y;
int res;
boolean istouched;
  res = 0;
  istouched = false;
  if (ttgo->getTouch(x, y)) {
    istouched = true;
    if (!wastouched) {
      if (x > TFT_MAXX) {  // touched in the button area
        res = Gestion_bouton (y);
      } else {
        VisuZone (x, y);
        CalculZone (x, y);
        Draw_Mandel();
      }
    }
  }
  wastouched = istouched;
  return res;
}

void appMandelbrot(void) {
  Mandel_setup();
  while(1) {
    my_idle();
    if(Mandel_loop()) {
	break;
    }
  }
  tft->fillScreen(TFT_BLACK);
  tft->setTextSize(1);
}
