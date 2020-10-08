# T-watch-2020
a watch project for the TTGO T-watch-2020 version 1

This builds with Arduino IDE version 1.8.13.  It also requires:
<UL>
  <LI><A HREF="https://github.com/Xinyuan-LilyGO/TTGO_TWatch_Library" TARGET=_blank>The "official" watch library</A>
  </LI>
  <LI><A HREF="https://github.com/bxparks/AceTime" TARGET=_blank>The AceTime library</A>
  </LI>
  <LI><A HREF="https://github.com/Bodmer/TFT_eSPI" TARGET=_blank>Bodmer's TFT_eSPI library</A>
  </LI>
</UL>      

I took bits from Dan Geiger's watch code (https://www.instructables.com/.../Lilygo-T-Watch-2020.../),
and bits from SimpleWatch (the shipped demo), and added many new features.
<H4>Existing Features</H4>
<UL>
  <LI>Dan's Jupiter's moon's calculator is mostly unchanged.
  </LI>
  <LI>Dan's accelerometer demo is now tarted up a little as a "Level"
  </LI>
  <LI>Dan's battery app is mostly unchanged.
  </LI>
  <LI>Dan's "Basic" watch face has been enhanced with step counter, battery charge icon.
  </LI>
  <LI>Dan's "LCARS" watch face has been enhanced with step counter, battery charge icon, more.
  </LI>
  <LI>Dan's Bitcoin app is mostly unchanged.
  </LI>
</UL>
<H4>New Features</H4>
<ul>
  <LI> choice of Analog, "Basic digital", "LilyGoGui", or LCARS watch face.  LCARS and Basic are derived from Dan Geiger's watch.  LilyGoGui derived from the demo code that comes with the watch.
  </LI>
  <LI> The ability to show time in your local time zone plus your home time zone if you're travelling
  </LI>
  <LI> step counter (can be disabled in Settings)
  </LI>
  <LI> MQTT monitor and control (for your home automation system, for example)
  </LI>
  <LI> 7 pages of Settings in two groups (alarm and "other") using the LVGL framework.  Settings are stored in EEPROM.
  </LI>
  <LI> A "maze" game.  Swipe or tilt watch left, right, up, down to move your cursor.  Try to move it to the red endpoint.  CW circle gesture to exit.
  </LI>
  <LI> A simple stopwatch
  </LI>
  <LI> A function to set the time from NTP.  If that fails, run's Dan Geiger's "SetTime" app.
  </LI>
  <LI> Storage for multiple WiFi credential sets, so the watch can connect to any SSID you regularly visit
  </LI>
  <LI> An alarm clock function.  Alarm is interpreted in the current timezone, so 08:00 is always 08:00 local time.
  </LI>
  <LI> The world's stupidest "paint" program
  </LI>
  <LI> Polish postfix scientific calculator.
  </LI>
  <LI> Conway's game of life.  (No user controls, "yet").  Randomly populates the "board" and runs forever.  Touch the screen to exit.
  </LI>
  <LI> Recognition of 6 gestures (left, right, up, down, cw circle, ccw circle). (The FT6236 chip claims to recognize gestures, but it doesn't.)
  </LI>
  <LI> Battery level icon, "is plugged into charger" icon, and step counter icon.
  </LI>
  <LI> The WiFi access points database is now stored in a file acc_pts.txt in the SPIFFs filesystem.  If you try to use the NTP function to connect to a new access point, appNTP will open up a settings page where you can enter the password and timezone for this new access point, and then it will be appended to the file acc_pts.txt.
  </LI>
  <LI> Most clocks (not LCARS) now can show either 12 or 24 hour clocks.  Setting is controlled in 4th page of Settings app.
  </LI>
</UL>

<H4>Using this code on your watch</H4>

To use this code, edit personal_info.h to have the IP, port, username, password of your MQTT server
(assuming you have one).  Edit my_WiFi.h to have the SSID's, passwords, and timezones of all the
WiFi access points you want your watch to connect with.  Finally, compile and upload to your watch.

<H4>Gestures Mapping</H4>

From the watch display, the following gestures work:
<UL>
  <LI> swipe down  : go to app select screen
  </LI>
  <LI> swipe right : run Battery app to see battery charge level, etc.
  </LI>
  <LI> swipe left  : go to alarm settings.  If alarm is beeping, silence alarm.
  </LI>
  <LI> CW circle   : run MQTT app
  </LI>
  <LI> CCW circle  : run Settings app.
  </LI>
</UL>
In the scientific calculator, the following gestures work:
<UL>
  <LI> swipe right : exit app
  </LI>
  <LI> swipe down  : push value onto stack
  </LI>
  <LI> swipe up    : pop stack and discard the value
  </LI>
  <LI> swipe left  : backspace to erase most recently typed digit
  </LI>
  <LI> CW circle   : put the value of pi on the top of the stack (3.141...)
  </LI>
  <LI> CCW circle  : switch to second keyboard, where most of the transcendental functions live, plus "CHS" (change sign), "CLX" (clear "x" or top of stack), swap X <-> Y
  </LI>
</UL>
To compute 2 + 3, do: 2, swipe down, 3, +

<IMG SRC="https://github.com/wfdudley/T-watch-2020/blob/master/images/2020-10-04_15.53.49.jpg">"LCARS" watch face</A><br>
