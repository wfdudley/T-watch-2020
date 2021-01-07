// edit this to have the IP, username, password of your MQTT server
// also edit MY_TIMEZONE to show your "home" timezone

#define MQTT_IP0 192
#define MQTT_IP1 168
#define MQTT_IP2 1
#define MQTT_IP3 50
#define MQTT_PORT 1883
#define MQTT_USER "user-o-roony"
#define MQTT_PASS "dumbpassword"

#define MY_TIMEZONE TZ_EST

// language for weather information.  Feature not supported yet.
#define MY_LANGUAGE "en"

// your personal keys
// get this one from openweathermap.org
#define ONECALLKEY "00000000001111111122222222333333"
// get this free API key from locationiq.com
#define LOCATIONIQ_KEY "kkkkkkkkkkkkkkkkkkkkkkkkkkkkkk"

// if you have a home IP address, put it here (in quotes).  It will
// save some time getting your location for weather when you're at home.
// your home IP should be a routable IP, NOT 10.something or 192.168.something
// One way to find your home ip is to visit "whatismyip.com" from your
// home computer.  Even if you have a dynamic IP, it'll be close enough
// for purposes of getting the local weather.
// #define HOME_IP "198.72.14.218" // typical home IP (this, in Washington, DC)
#define HOME_IP ""

// Get your home latitude and longitude from google maps (right click on your
// This is the latitude and longitude of the Washington Post newspaper.
// #define HOME_LATITUDE "38.90284"
// #define HOME_LONGITUDE "-77.03082"
#define HOME_LATITUDE ""
#define HOME_LONGITUDE ""

// #define HOME_CITY "Washington, DC"
#define HOME_CITY ""
