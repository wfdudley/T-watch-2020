# T-watch-2020
a watch project for the TTGO T-watch-2020 version 1

This builds with Arduino IDE version 1.8.13
Requires the AceTime library.

I've written "new" watch code.
I took bits from Dan Geiger's watch code (https://www.instructables.com/.../Lilygo-T-Watch-2020.../),
and bits from SimpleWatch (the shipped demo), and added the following:

choice of Analog, "Basic digital", or LCARS watch face  (LCARS and Basic derived from Dan Geiger's watch).

the ability to show time in your local time zone plus your home time zone if you're travelling

step counter

MQTT monitor and control

7 pages of Settings in two groups (alarm and "other") using the LVGL framework

A "maze" game.  Swipe or tilt watch left, right, up, down to move your cursor.  Try to move it to the red endpoint.

a stopwatch

a function to set the time from NTP

storage for multiple WiFi credential sets, so the watch can connect to any SSID you regularly visit

an alarm clock function

the worlds stupidest "paint" program

Polish postfix scientific calculator.

Conway's game of life.  (No user controls, "yet").

Also, my code recognizes 6 gestures (left, right, up, down, cw circle, ccw circle).
(It turns out that the touch sensor chip claims to recognize gestures, but it doesn't.)

To use this code, edit personal_info.h to have the IP, port, username, password of your MQTT server
(assuming you have one).  Edit my_WiFi.h to have the SSID's, passwords, and timezones of all the
WiFi access points you want your watch to connect with.  Finally, compile and upload to your watch.

From the watch display, the following gestures work:

swipe down  : go to app select screen

swipe right : run Battery app to see battery charge level, etc.

swipe left  : run alarm settings.  If alarm is beeping, silence alarm.

CW circle   : run MQTT app

CCW circle  : run Settings app.

In the scientific calculator, the following gestures work:

swipe right : exit app

swipe down  : push value onto stack

swipe up    : pop stack and discard the value

swipe left  : backspace to erase most recently typed digit

CW circle   : put the value of pi on the top of the stack (3.141...)

CCW circle  : switch to second keyboard, where most of the transcendental functions live, plus "CHS" (change sign), "CLX" (clear "x" or top of stack), swap X <-> Y

To compute 2 + 3, do: 2, swipe down, 3, +

