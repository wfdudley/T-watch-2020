
// my_WiFi.h
//
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
    uint32_t tzone;
} WIFIAP;

EXTERN WIFIAP AccessPoints[]
#ifdef __WIFIMAIN__
= {
    { "D-link", "***REMOVED***", TZ_EST },	// 274 Jackson Pines Rd, Jackson
    { "NoSuchAgency", "83879398bb", TZ_EST },	// Dud's phone hotspot
    { "Yeldud's Bar", "***REMOVED***", TZ_PST },	// 1848 El Rey Rd, San Pedro
    { "Yeldud UP", "***REMOVED***", TZ_PST },	// 1848 El Rey Rd, San Pedro
    { "1313", "67032106", TZ_CHINA },		// Zhao & Wang's apartment
    { "WJ", "***REMOVED***", TZ_CHINA },	// Wang Jun's portable hotspot
    { "TP-LINK_A978", "10118271", TZ_CHINA },	// Wang Kun's apartment
    { "\xE9\x93\xAD\xE5\xBE\xB7\xE7\xBE\x8E\xE5\xAE\xB6" "1-2301", "mingde2301", TZ_CHINA },    // China AirBnB, keep as example of non-ASCII chars
    { "TG1672GE2", "***REMOVED***", TZ_EST },	// Geo & Eileen's house in NC
    { "NETGEAR Qin32", "***REMOVED***", TZ_EST }	// Marvin & Cindy NYC
}
#endif
;

EXTERN int sizeof_Access_Points INIT(sizeof(AccessPoints)/sizeof(WIFIAP));

typedef struct WiFiAp {
    char ssid[33];
    char pass[65];
    uint8_t macAddr[6];
    uint8_t channel;
    uint32_t tzone;
} WiFiap;

EXTERN WiFiap BestAP;
EXTERN boolean connected INIT(false);
EXTERN int number_of_networks;
EXTERN int num_saved_ap, best_ap;

int read_acc_pts_file_and_compare_with_SSIDs (struct WiFiAp *);
int connect_to_wifi(boolean, struct WiFiAp *, boolean);
int connect_to_wifi_and_get_time (boolean);
void append_new_access_point (char *, char *, uint32_t );

#endif
