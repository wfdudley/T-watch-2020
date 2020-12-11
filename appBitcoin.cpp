#include "config.h"
#include "DudleyWatch.h"
#include <ArduinoJson.h>
#include <WiFi.h>
#include <WiFiClientSecure.h>
#include "my_WiFi.h"

void appBitcoin() {
int verbose=1;
int ecnt;
  if(1) {	// this connects to wifi, MQTT server, and runs MQTTcallback
    if(connect_to_wifi(verbose, &BestAP, true, true) && verbose) {
	Serial.printf("connect to wifi failed\n");
	tft->setTextColor(TFT_YELLOW, TFT_BLACK);
	tft->drawString("Connect to WiFi Failed!",  0, 5, 2);
	delay(5000);
	return;
    }
    ecnt = 0;
    while(!connected && ecnt < 400) {
#if DBGMQTT
      Serial.println(F("waiting 0.2 seconds for wifi connection"));
#endif
      ecnt++;
      delay(200);
    }
    if(ecnt == 40) {
#if DBGMQTT
      Serial.println(F("couldn't get wifi connection in 80 seconds!"));
#endif
      tft->setTextColor(TFT_YELLOW, TFT_BLACK);
      tft->drawString("Connect to WiFi failed in 80 seconds!",  0, 5, 2);
      delay(3000);
      WiFi.mode(WIFI_OFF);
      connected = false;
      return;
    }
    if(verbose) {
      tft->setTextSize(1);
      tft->setTextColor(TFT_YELLOW, TFT_BLACK);
      tft->setCursor(0, 50 + (15 * 8));
      tft->printf("Connected to %s channel %d", BestAP.ssid, BestAP.channel);
      tft->setCursor(0, 50 + (15 * 9));
      tft->print(F("Trying to connect to api.coindesk.com"));
      tft->setCursor(0, 50 + (15 * 10));
      tft->print(F("my ip is: "));
      tft->print(WiFi.localIP());
    }
  }
  // API server
  const char* host = "api.coindesk.com";

  int16_t x, y;

  while (ttgo->getTouch(x, y)) {}

  WiFiClient client;
  const int httpPort = 80;
  client.connect(host, httpPort);

  // We now create a URI for the request
  String url = "/v1/bpi/currentprice.json";

  // This will send the request to the server
  client.print(String("GET ") + url + " HTTP/1.1\r\n" +
               "Host: " + host + "\r\n" +
               "Connection: close\r\n\r\n");
  delay(100);

  // Read all the lines of the reply from server and print them to Serial
  String answer;
  while (!client.available()) {}
  while (client.available()) {
    String line = client.readStringUntil('\r');
    answer += line;
  }

  client.stop();

  // Convert to JSON
  String jsonAnswer;
  int jsonIndex;

  for (int i = 0; i < answer.length(); i++) {
    if (answer[i] == '{') {	// }
      jsonIndex = i;
      break;
    }
  }

  // Get JSON data
  jsonAnswer = answer.substring(jsonIndex);
  jsonAnswer.trim();

  // Get rate as float
  int rateIndex = jsonAnswer.indexOf("rate_float");
  String priceString = jsonAnswer.substring(rateIndex + 12, rateIndex + 19);
  priceString.trim();
  float price = priceString.toFloat();

  tft->fillScreen(TFT_PURPLE);
  tft->setTextSize(2);
  tft->setTextColor(TFT_GREEN);
  tft->setCursor(40, 90);

  tft->println("Bitcoin Price");
  tft->setCursor(70, 130);
  tft->print("$");
  tft->println(priceString);
  tft->setTextSize(1);

  client.stop();
  WiFi.disconnect(true);
  WiFi.mode(WIFI_OFF);

  while (!ttgo->getTouch(x, y)) {} // wait until you touching
  while (ttgo->getTouch(x, y)) {}
  ttgo->tft->fillScreen(TFT_BLACK);
  connected = false;
  WiFi.mode(WIFI_OFF);
}
