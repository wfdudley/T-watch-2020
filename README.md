# T-watch-2020
a watch project for the TTGO T-watch-2020 version 1

This builds with Arduino IDE version 1.8.13.  It requires the AceTime library.

I've written "new" watch code.
I took bits from Dan Geiger's watch code (https://www.instructables.com/.../Lilygo-T-Watch-2020.../),
and bits from SimpleWatch (the shipped demo), and added the following:
<H4>Feature List</H4>
<ul>
<LI> choice of Analog, "Basic digital", or LCARS watch face  (LCARS and Basic derived from Dan Geiger's watch).

<LI> The ability to show time in your local time zone plus your home time zone if you're travelling

<LI> step counter (can be disabled in Settings)

<LI> MQTT monitor and control (for your home automation system, for example)

<LI> 7 pages of Settings in two groups (alarm and "other") using the LVGL framework.  Settings are stored in EEPROM.

<LI> A "maze" game.  Swipe or tilt watch left, right, up, down to move your cursor.  Try to move it to the red endpoint.  CW circle gesture to exit.

<LI> A simple stopwatch

<LI> A function to set the time from NTP.  If that fails, run's Dan Geiger's "SetTime" app.)

<LI> Storage for multiple WiFi credential sets, so the watch can connect to any SSID you regularly visit

<LI> An alarm clock function.  Alarm is interpreted in the current timezone, so 08:00 is always 08:00 local time.

<LI> The world's stupidest "paint" program

<LI> Polish postfix scientific calculator.

<LI> Conway's game of life.  (No user controls, "yet").  Randomly populates the "board" and runs forever.  Touch the screen to exit.
</UL>
Also, my code recognizes 6 gestures (left, right, up, down, cw circle, ccw circle).
(The FT6236 chip claims to recognize gestures, but it doesn't.)

To use this code, edit personal_info.h to have the IP, port, username, password of your MQTT server
(assuming you have one).  Edit my_WiFi.h to have the SSID's, passwords, and timezones of all the
WiFi access points you want your watch to connect with.  Finally, compile and upload to your watch.
<H4>Gestures Mapping</H4>
From the watch display, the following gestures work:
<UL>
<LI> swipe down  : go to app select screen

<LI> swipe right : run Battery app to see battery charge level, etc.

<LI> swipe left  : go to alarm settings.  If alarm is beeping, silence alarm.

<LI> CW circle   : run MQTT app

<LI> CCW circle  : run Settings app.
</UL>
In the scientific calculator, the following gestures work:
<UL>
<LI> swipe right : exit app

<LI> swipe down  : push value onto stack

<LI> swipe up    : pop stack and discard the value

<LI> swipe left  : backspace to erase most recently typed digit

<LI> CW circle   : put the value of pi on the top of the stack (3.141...)

<LI> CCW circle  : switch to second keyboard, where most of the transcendental functions live, plus "CHS" (change sign), "CLX" (clear "x" or top of stack), swap X <-> Y
  </UL>
To compute 2 + 3, do: 2, swipe down, 3, +

