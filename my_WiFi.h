// my_WiFi.h
// this is the "generic" version.
// edit to add all the WiFi credentials you'd like to connect to

#ifndef __MYWIFI_H__
#define __MYWIFI_H__

#ifdef __WIFIMAIN__
#define EXTERN
#define INIT(x) = x
#else
#define EXTERN extern
#define INIT(x)
#endif

typedef struct WiFiAP {
    char *ssid;
    char *pass;
    int tzone;
} WIFIAP;

EXTERN WIFIAP AccessPoints[]
#ifdef __WIFIMAIN__
= {
    { "SSID-1", "password#1", TZ_EST },		// Newark, NJ
    { "Phone-ssid", "#2password", TZ_EST },	// my phone hotspot
    { "work-ssid", "password4job", TZ_EST },	// my job's WiFi
    { "friend #1 ssid", "pass#2word", TZ_PST },	// friend one's WiFi
    { "2nd friend SSID", "passwo#4rd", TZ_CST }, // friend two's WiFi
    { "dentist-ssid", "pa#5ssword", TZ_EST },	// my dentist's WiFi
    { "\xE9\x93\xAD\xE5\xBE\xB7\xE7\xBE\x8E\xE5\xAE\xB6" "1-2301", "mingde2301", TZ_CHINA },    // an AirBnB in China, kept as example of non-ASCII chars
    { "friend 3 SSID", "passwor#6d", TZ_EST }	// friend three's WiFi
}
#endif
;

typedef struct WiFiAp {
    char ssid[33];
    char pass[65];
    uint8_t macAddr[6];
    uint8_t channel;
    int tzone;
} WiFiap;

EXTERN WiFiap BestAP;
EXTERN boolean connected INIT(false);

int connect_to_wifi(boolean, struct WiFiAp *, boolean);

#endif
