#include "config.h"
#include "DudleyWatch.h"
#include "personal_info.h"
#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>

static String payload;

// do http or https request, leave result in payload, possibly copy to result
String get_thing(char *url, char *result, int rlength) {
int errcnt;
  errcnt = 0;
  do {
    HTTPClient http;
// Serial.printf("g_l: line %d\n", __LINE__);
    http.begin(url);
// Serial.printf("g_l: line %d\n", __LINE__);
    // start connection and send HTTP header
    int httpCode = http.GET();
    // httpCode will be negative on error
// Serial.printf("g_l: line %d\n", __LINE__);
    if(httpCode > 0) {
// Serial.printf("g_l: line %d\n", __LINE__);
	// HTTP header has been send and server response header has been handled
	if(httpCode == HTTP_CODE_OK
	|| httpCode == 301) {	// success!
	  payload = http.getString();
	  // Serial.println(payload);
	  int max_length = 1 + payload.length();
	  if(max_length > rlength) {
	    max_length = rlength;
	  }
	  if(result && max_length) {
// Serial.printf("g_l: line %d\n", __LINE__);
	    payload.toCharArray(result, max_length);
// Serial.printf("g_l: line %d\n", __LINE__);
	  }
	}
	else {
	  Serial.printf("http.GET problem, code: %d\n", httpCode);
// Serial.printf("g_l: line %d\n", __LINE__);
	  payload = http.getString();
// Serial.printf("g_l: line %d\n", __LINE__);
	  Serial.println(payload);
	  errcnt++;
	}
    }
    else {
	Serial.printf("http.GET failed, error: %s\n", http.errorToString(httpCode).c_str());
	errcnt++;
	delay(10);
    }
    http.end();
  } while(errcnt && errcnt < 3);
// Serial.printf("line %d\n", __LINE__);
  return payload;
}

static const char *ip_apis[] = {
//"https://helloacm.com/api/what-is-my-ip-address/",	// unusable
  "https://www.dudley.nu/printip.cgi",
  "http://bot.whatismyipaddress.com/",
//"https://api.ipify.org",	// doesn't work, no idea why
  "http://danml.com/myip/"
};

char str_latitude[20];
char str_longitude[20];
char get_loc_city[80];

boolean is_non_routable_ip (char *my_ipaddress) {
  if(!strncmp(my_ipaddress, "192.168.", 8)
  || !strncmp(my_ipaddress, "10.", 3)
  || !strncmp(my_ipaddress, "172.16.", 7)
  || !strncmp(my_ipaddress, "172.17.", 7)
  || !strncmp(my_ipaddress, "172.18.", 7)
  || !strncmp(my_ipaddress, "172.19.", 7)
  || (!strncmp(my_ipaddress, "172.2", 5) && isdigit(my_ipaddress[5]) && my_ipaddress[6] == '.')
  || !strncmp(my_ipaddress, "172.30.", 7)
  || !strncmp(my_ipaddress, "172.31.", 7)) {
    return true;
  }
  return false;
}

void remove_trailing_newline(char *s) {
  int iplastchar = strlen(s) - 1;
  while(iplastchar > 1 && s[iplastchar] < ' ') {
    s[iplastchar] = '\0';
    iplastchar = strlen(s) - 1;
  }
}

// 0 result means success
// -1 result means default to home IP
int get_lat_lon (void) {
int result;
  result = -1;
  str_latitude[0] = '\0';
  str_longitude[0] = '\0';
  int home_ip_len = strlen(HOME_IP);
  if ((WiFi.status() == WL_CONNECTED)) { //Check the current connection status
    result = 0;
    char my_ipaddress[20];
    char request_url[200];
    memset(my_ipaddress, '\0', sizeof(my_ipaddress));
    for(int ii = 0 ; ii < sizeof(ip_apis)/sizeof(char *) ; ii++) {
      strcpy(request_url, ip_apis[ii]);
      Serial.printf("trying %s\n", request_url);
      get_thing(request_url, my_ipaddress, sizeof(my_ipaddress));
      remove_trailing_newline(my_ipaddress);
      Serial.printf("after attempt %d, my ip = %s\n", ii+1, my_ipaddress);
      if(is_non_routable_ip(my_ipaddress)) {
	my_ipaddress[0] = '\0';
	Serial.println("non-routable!");
      }
      if(my_ipaddress[0]) {
	break;
      }
    }
    if(!my_ipaddress[0] || !isdigit(my_ipaddress[0]) || is_non_routable_ip(my_ipaddress)) {
      Serial.printf("last attempt got bad or non-routable IP\n");
      strcpy(my_ipaddress, HOME_IP);
      Serial.printf("can't get ip address, using home %s\n", my_ipaddress);
      result = -1;
    }
    Serial.printf("my ip is %s\n", my_ipaddress);
    // if NOT my ip AND ( !str_latitude OR !str_longitude), DO
    // if it's NOT my home IP address, look up str_latitude and str_longitude
    if(strncmp(my_ipaddress, HOME_IP, home_ip_len)
    && (!str_latitude[0] || !str_longitude[0])) {
      sprintf(request_url, "%s%s/latitude", "https://ipapi.co/", my_ipaddress);
      Serial.printf("trying %s\n", request_url);
      get_thing(request_url, str_latitude, sizeof(str_latitude));
      Serial.printf("my latitude is %s\n", str_latitude);
      sprintf(request_url, "%s%s/longitude", "https://ipapi.co/", my_ipaddress);
      Serial.printf("trying %s\n", request_url);
      get_thing(request_url, str_longitude, sizeof(str_longitude));
      Serial.printf("my longitude is %s\n", str_longitude);
      sprintf(request_url, "%s%s/city", "https://ipapi.co/", my_ipaddress);
      Serial.printf("trying %s\n", request_url);
      get_thing(request_url, get_loc_city, sizeof(get_loc_city));
      Serial.printf("my city is %s\n", get_loc_city);
    }
    if(!strncmp(my_ipaddress, HOME_IP, home_ip_len)) {
      strcpy(get_loc_city, HOME_CITY);
    }
    if(!str_latitude[0]) {
      strncpy(str_latitude, general_config.my_latitude, sizeof(str_latitude));
      Serial.printf("at home or latitude get failed, using default %s\n", str_latitude);
      result = -1;
    }
    if(!str_longitude[0]) {
      strncpy(str_longitude, general_config.my_longitude, sizeof(str_longitude));
      Serial.printf("at home or longitude get failed, using default %s\n", str_longitude);
      result = -1;
    }
  }
  return result;
}

// this is actually a full reverse geolocation lookup
int get_city_from_lat_long(char *latitude, char *longitude) {
char url[100];
char json[600];
  get_loc_city[0] = '\0';
  sprintf(url, "https://us1.locationiq.com/v1/reverse.php?key=%s&format=json&lat=%s&lon=%s", LOCATIONIQ_KEY, latitude, longitude);
  // Serial.printf("get_city... trying %s\n", url);
  get_thing(url, json, sizeof(json));
  DynamicJsonDocument doc(1024);
  deserializeJson(doc, json);

  const char* place_id = doc["place_id"]; // "332847685712"
  const char* licence = doc["licence"]; // "https://locationiq.com/attribution"
  const char* lat = doc["lat"]; // "40.098011"
  const char* lon = doc["lon"]; // "-74.26592"
  const char* display_name = doc["display_name"]; // "274, Jackson Pines Road, Jackson Township, Ocean County, New Jersey, 08527, USA"
  // Serial.println(display_name);

  JsonArray boundingbox = doc["boundingbox"];
  const char* boundingbox_0 = boundingbox[0]; // "40.098011"
  const char* boundingbox_1 = boundingbox[1]; // "40.098011"
  const char* boundingbox_2 = boundingbox[2]; // "-74.26592"
  const char* boundingbox_3 = boundingbox[3]; // "-74.26592"

  float importance = doc["importance"]; // 0.225

  JsonObject address = doc["address"];
  const char* address_house_number = address["house_number"]; // "274"
  const char* address_road = address["road"]; // "Jackson Pines Road"
  const char* address_city = address["city"]; // "Jackson Township"
  const char* address_county = address["county"]; // "Ocean County"
  const char* address_state = address["state"]; // "New Jersey"
  const char* address_postcode = address["postcode"]; // "08527"
  const char* address_country = address["country"]; // "United States of America"
  const char* address_country_code = address["country_code"]; // "us"
  if(address["city"]) {
    strncpy(get_loc_city, address["city"], sizeof(get_loc_city));
    char *tp = strstr(get_loc_city, " Township");
    if(tp) { *tp = '\0'; }
    return 0;
  }
  return -1;
}
