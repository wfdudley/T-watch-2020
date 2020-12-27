# T-watch-2020
a watch project for the TTGO T-watch-2020 version 1

This builds with Arduino IDE version 1.8.13.  It also requires:
<UL>
  <LI><A HREF="https://github.com/Xinyuan-LilyGO/TTGO_TWatch_Library" TARGET=_blank>The "official" watch library</A>
  </LI>
  <LI><A HREF="https://github.com/bxparks/AceTime" TARGET=_blank>The AceTime library</A>, which, as of 1.3.0, also requires the <A HREF="https://github.com/bxparks/AceCommon" TARGET=_blank>AceCommon library</A>
  </LI>
  <LI><A HREF="https://github.com/bblanchon/ArduinoJson" TARGET=_blank>The ArduinoJson library</A>
  </LI>
  <LI><A HREF="https://github.com/Bodmer/TFT_eSPI" TARGET=_blank>Bodmer's TFT_eSPI library</A>
  </LI>
  <LI><A HREF="https://github.com/earlephilhower/ESP8266Audio" TARGET=_blank>ESP8266Audio library</A>
  </LI>
  <LI><A HREF="https://github.com/JHershey69/OpenWeatherOneCall" TARGET=_blank>OpenWeatherOneCall library</A>
  </LI>
  <LI><A HREF="https://github.com/knolleary/pubsubclient" TARGET=_blank>PubSubClient (MQTT) library</A>
  </LI>
</UL>      

I took bits from Dan Geiger's watch code (https://www.instructables.com/Lilygo-T-Watch-2020-Arduino-Framework/),
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
  <LI> If you attempt to use WiFi with an unfamiliar access point, the watch will open a page where you can choose from the available SSIDs and then enter the password associated with that SSID.  This information is stored in a "file" in the SPIFFs part of the flash.  This is not overwritten when you re-upload the program from the Arduino IDE, so your WiFi SSID's and passwords are stored "forever".
  </LI>
  <LI> step counter (can be disabled in Settings)
  </LI>
  <LI> MQTT monitor and control (for your home automation system, for example)
  </LI>
  <LI> 9 pages of Settings in three groups (alarm, weather and "other") using the LVGL framework.  Settings are stored in EEPROM.
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
  <LI> A Mandelbrot/Julia set app, written by "FabriceA6" (sorry, I couldn't find better attribution).
  </LI>
  <LI> The WiFi access points database is now stored in a file acc_pts.txt in the SPIFFs filesystem.  If you try to use any WiFi app to connect to a new access point, the app will open up a settings page where you can enter the password and timezone for this new access point, and then it will be appended to the file acc_pts.txt.
  </LI>
  <LI> Most clocks (not LCARS) now can show either 12 or 24 hour clocks.  Setting is controlled in 4th page of Settings app.
  </LI>
  <LI> A simple calendar app.  Shows one month at a time, and you can navigate to any month using +month, -month, +year, -year buttons.  At compile time, you can define your own holidays and events like birthdays, which are highlighted different colors on the calendar display.  Touch anywhere not on a button to exit the app.
  </LI>
  <LI> An app to allow deletion of WiFi access point from the internal flash storage (SPIFF file acc_pts.txt).  This is useful if you enter the password incorrectly, or the password changes.  Simply delete the SSID and then re-enter it by using the NTP Time app or the Weather app, which will ask you for the credentials when it fails to connect to wifi.
  </LI>
  <LI> A weather app.  The app attempts to figure your location using your IP address, which works pretty well with WiFi that is not a cell phone hotspot.  The reason cell phone hotspots give a bad location is that your cell data may be routed to a city 100 miles away before it gets a routable IP address and gets to the internet.  So the weather location may be off by quite a bit when using a cell phone hotspot for connectivity.  You'll need an OpenWeatherMap.org API key, which was free at time of writing.  Also note the <b>LIBRARY REQUIREMENT</b> listed at the beginning of the file.  The weather app shows today's conditions, the conditions for the next week, and any National Weather Service alerts if there are any for "your" location.  Swipe left and right to see different pages, swipe down to exit.
  </LI>
  <LI> A weather settings app.  Allows you to set your home latitude, home longitude, the openweathermap.org API key, and metric/imperial units.  When changing a text box content, make sure to hit the check icon on the keyboard to tell the GUI that you've made a change.
  </LI>
  <LI> A WiFi scanner app.  It supplies two views: (1) list of access points, channel, RSSI; (2) graphical representation, channel vs strength.  Swipe left or right to switch views.  Swipe down to exit.
  </LI>
</UL>

<H4>Using this code on your watch</H4>

To use this code, edit personal_info.h to have the IP, port, username, password of your MQTT server (assuming you have one), plus your home latitiude, longitude, and city, and also your openweathermap.org API key.  Edit my_WiFi.h to have the SSID's, passwords, and timezones of all the WiFi access points you want your watch to connect with.  Finally, compile and upload to your watch.

<H4>warning about the LVGL User Interface</H4>

The LVGL UI is used to build the setup, WiFi, and alarm screens with all the dropdowns, buttons, etc.  The text entry boxes put up a keyboard when touched.  When you have finished entering your text into the box (WiFi password, for example), <b>make sure</b> that you hit the checkmark on the keyboard.  This ensures that the characters you have entered are seen by the UI.

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
<br>
<H4>Screen Shots</H4>
<TABLE>
  <TR>
    <TD>
      <IMG SRC="https://github.com/wfdudley/T-watch-2020/blob/master/images/2020-10-04_15.53.49.jpg"><br>"LCARS" watch face
    </TD>
    <TD>
      <IMG SRC="https://github.com/wfdudley/T-watch-2020/blob/master/images/2020-10-05_13.53.46.jpg"><br>"LilyGoGui" watch face
    </TD>
    <TD>
      <IMG SRC="https://github.com/wfdudley/T-watch-2020/blob/master/images/2020-10-08_22.01.39.jpg"><br>Analog watch face
    </TD>
  </TR>
  <TR>
    <TD>
      <IMG SRC="https://github.com/wfdudley/T-watch-2020/blob/master/images/2020-10-08_21.55.22.jpg"><br>Conway's Life
    </TD>
    <TD>
      <IMG SRC="https://github.com/wfdudley/T-watch-2020/blob/master/images/2020-10-04_15.58.25.jpg"><br>settings page 4
    </TD>
    <TD>
      <IMG SRC="https://github.com/wfdudley/T-watch-2020/blob/master/images/2020-10-04_15.59.18.jpg"><br>alarm settings page 1
    </TD>
  </TR>
  <TR>
    <TD>
      <IMG SRC="https://github.com/wfdudley/T-watch-2020/blob/master/images/2020-10-08_22.31.32.jpg"><br>MQTT client sensor page
    </TD>
    <TD>
      <IMG SRC="https://github.com/wfdudley/T-watch-2020/blob/master/images/2020-10-08_22.31.56.jpg"><br>MQTT client control page
    </TD>
    <TD>
      <IMG SRC="https://github.com/wfdudley/T-watch-2020/blob/master/images/2020-10-08_22.08.51.jpg"><br>Stop Watch
    </TD>
  </TR>
  <TR>
    <TD>
      <IMG SRC="https://github.com/wfdudley/T-watch-2020/blob/master/images/2020-10-08_22.04.42.jpg"><br>Scientific Calculator page 1
    </TD>
    <TD>
      <IMG SRC="https://github.com/wfdudley/T-watch-2020/blob/master/images/2020-10-08_22.06.03.jpg"><br>Scientific Calculator page 2
    </TD>
    <TD>
      <IMG SRC="https://github.com/wfdudley/T-watch-2020/blob/master/images/2020-10-08_21.57.48.jpg"><br>Maze game
    </TD>
  </TR>
  <TR>
    <TD>
      <IMG SRC="https://github.com/wfdudley/T-watch-2020/blob/master/images/2020-12-21_17.00.16.jpg"><br>Weather App
    </TD>
    <TD>
      <IMG SRC="https://github.com/wfdudley/T-watch-2020/blob/master/images/2020-12-22_wifi_scan.jpg"><br>WiFi scanner App
    </TD>
    <TD>
      &nbsp;
    </TD>
  </TR>
</TABLE>
