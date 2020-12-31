A beginner's guide to compiling github.com/wfdudley/T-watch-2020 using Arduino

<OL>
<LI> visit arduino.cc and download the latest arduino for your OS
</LI>
<LI> Install the lump you downloaded.
</LI>
<LI> open the Arduino IDE
</LI>
<LI> go here https://randomnerdtutorials.com/installing-the-esp32-board-in-arduino-ide-windows-instructions/
and follow the directions for your OS on how to install esp32 support
</LI>
<LI> go to Tools/Board and select "TTGO T-watch".  This should automatically
select the proper partition scheme: "Default (2x6.5 MB APP, 3.6 MB SPIFFS)"
</LI>
<LI> go to Tools/Library Manager, select Type="Updatable" and update your libraries.
</LI>
<LI> from https://github.com/wfdudley/T-watch-2020, get the list of
required libraries and install them.
</LI>
<LI> For the T-watch library, you can follow this tutorial:
https://diyprojects.io/lilygowatch-esp32-ttgo-t-watch-get-started-ide-arduino-platformio/
</LI>
<LI> For AceTime (and AceCommon), ArduinoJson, OpenweatherOneCall, TFT_eSPI, PubSubClient, NTPClient, ESP8266Audio libraries, use the Arduino Library Manager to install them.
</LI>
<LI> Download the zip from here: https://github.com/wfdudley/T-watch-2020 and
unzip (or do "git clone https://github.com/wfdudley/T-watch-2020") IN YOUR "Arduino" directory, or wherever you have decided to put your "sketches").
</LI>
<LI> RENAME the sketch directory to "DudleyWatch".
</LI>
<LI> In the DudleyWatch directory, edit my_WiFi.h and personal_info.h to suit your situation.
</LI>
<LI> In the Arduino IDE, open the DudleyWatch sketch.  Plug in your T-watch-2020 to the USB port.  Make sure the right port ("Tools/Port") is selected in the Arduino IDE.  Click the "Upload" button.  Go make a cup of coffee.
</LI>
<LI>  If your verify fails with the error "ImportError: No module named serial", then you must install python serial module.  See this:
https://stackoverflow.com/questions/61339256/arduino-nodemcu-error-compiling-for-board-esp32
<PRE>
if you run Linux, the answer is:
# For debian base linux
sudo apt-get install python-serial
# For Centos
sudo yum install python3-serial
# For Fedora
sudo dnf install python3-serial
</PRE>
<P>
If you run Winders, well, google is your friend.  I can't do everything for you.
</LI>
</BODY>
