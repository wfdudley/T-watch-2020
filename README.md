# T-watch-2020
a watch project for the TTGO T-watch-2020 version 1

I've written "new" watch code.
I took bits from Dan Geiger's watch code (https://www.instructables.com/.../Lilygo-T-Watch-2020.../),
and bits from SimpleWatch (the shipped demo), and added the following:

choice of Analog, "Basic digital", or LCARS watch face  (LCARS and Basic derived from Dan Geiger's watch).

the ability to show time in your local time zone plus your home time zone if you're travelling

step counter; MQTT monitor and control; 7 pages of Settings using the LVGL framework

the start of a "maze" game

a stopwatch

a function to set the time from NTP

storage for multiple WiFi credential sets, so the watch can connect to any SSID you regularly visit

an alarm clock function (but I don't yet know how to schedule a wake from sleep

the worlds stupidest "paint" program

Also, my code recognizes 6 gestures (left, right, up, down, cw circle, ccw circle).
(It turns out that the touch sensor chip claims to recognize gestures, but it doesn't.)
