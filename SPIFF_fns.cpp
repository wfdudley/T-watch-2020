// config.h BREAKS FS.h and SPIFFS.h !!!!
// #include "config.h"
#include "FS.h"
#include "SPIFFS.h"
#include <WiFi.h>
#include "my_WiFi.h"      // define "my" wifi stuff

#define WE_HAVE_SPIFFS 1
#define NORMAL_WIFI_OPERATION 1  // disable to test the access point setup code

/* You only need to format SPIFFS the first time you run a
   test or else use the SPIFFS plugin to create a partition
   https://github.com/me-no-dev/arduino-esp32fs-plugin */
#define FORMAT_SPIFFS_IF_FAILED true

void listDir(fs::FS &fs, const char * dirname, uint8_t levels){
    Serial.printf("Listing directory: %s\r\n", dirname);

    File root = fs.open(dirname);
    if(!root){
        Serial.println("- failed to open directory");
        return;
    }
    if(!root.isDirectory()){
        Serial.println(" - not a directory");
        return;
    }

    File file = root.openNextFile();
    while(file){
        if(file.isDirectory()){
            Serial.print("  DIR : ");
            Serial.println(file.name());
            if(levels){
                listDir(fs, file.name(), levels -1);
            }
        } else {
            Serial.print("  FILE: ");
            Serial.print(file.name());
            Serial.print("\tSIZE: ");
            Serial.println(file.size());
        }
        file = root.openNextFile();
    }
}

void readFile(fs::FS &fs, const char * path){
    Serial.printf("Reading file: %s\r\n", path);

    File file = fs.open(path);
    if(!file || file.isDirectory()){
        Serial.println("- failed to open file for reading");
        return;
    }

    Serial.println("- read from file:");
    while(file.available()){
        Serial.write(file.read());
    }
    Serial.println("end of readFile ----------------");
}

void writeFile(fs::FS &fs, const char * path, const char * message){
    Serial.printf("Writing file: %s\r\n", path);

    File file = fs.open(path, FILE_WRITE);
    if(!file){
        Serial.println("- failed to open file for writing");
        return;
    }
    if(file.print(message)){
        Serial.println("- file written");
    } else {
        Serial.println("- frite failed");
    }
}

void appendFile(fs::FS &fs, const char * path, const char * message){
    Serial.printf("Appending to file: %s\r\n", path);

    File file = fs.open(path, FILE_APPEND);
    if(!file){
        Serial.println("- failed to open file for appending");
        return;
    }
    if(file.print(message)){
        Serial.println("- message appended");
    } else {
        Serial.println("- append failed");
    }
}

void renameFile(fs::FS &fs, const char * path1, const char * path2){
    Serial.printf("Renaming file %s to %s\r\n", path1, path2);
    if (fs.rename(path1, path2)) {
        Serial.println("- file renamed");
    } else {
        Serial.println("- rename failed");
    }
}

void deleteFile(fs::FS &fs, const char * path){
    Serial.printf("Deleting file: %s\r\n", path);
    if(fs.remove(path)){
        Serial.println("- file deleted");
    } else {
        Serial.println("- delete failed");
    }
}

void testFileIO(fs::FS &fs, const char * path){
    Serial.printf("Testing file I/O with %s\r\n", path);

    static uint8_t buf[512];
    size_t len = 0;
    File file = fs.open(path, FILE_WRITE);
    if(!file){
        Serial.println("- failed to open file for writing");
        return;
    }

    size_t i;
    Serial.print("- writing" );
    uint32_t start = millis();
    for(i=0; i<2048; i++){
        if ((i & 0x001F) == 0x001F){
          Serial.print(".");
        }
        file.write(buf, 512);
    }
    Serial.println("");
    uint32_t end = millis() - start;
    Serial.printf(" - %u bytes written in %u ms\r\n", 2048 * 512, end);
    file.close();

    file = fs.open(path);
    start = millis();
    end = start;
    i = 0;
    if(file && !file.isDirectory()){
        len = file.size();
        size_t flen = len;
        start = millis();
        Serial.print("- reading" );
        while(len){
            size_t toRead = len;
            if(toRead > 512){
                toRead = 512;
            }
            file.read(buf, toRead);
            if ((i++ & 0x001F) == 0x001F){
              Serial.print(".");
            }
            len -= toRead;
        }
        Serial.println("");
        end = millis() - start;
        Serial.printf("- %u bytes read in %u ms\r\n", flen, end);
        file.close();
    } else {
        Serial.println("- failed to open file for reading");
    }
}

void test_spiffs(void) {
    Serial.begin(115200);
    if(!SPIFFS.begin(FORMAT_SPIFFS_IF_FAILED)){
        Serial.println("SPIFFS Mount Failed");
        return;
    }
    
    listDir(SPIFFS, "/", 0);
    writeFile(SPIFFS, "/hello.txt", "Hello ");
    appendFile(SPIFFS, "/hello.txt", "World!\r\n");
    readFile(SPIFFS, "/hello.txt");
    renameFile(SPIFFS, "/hello.txt", "/foo.txt");
    readFile(SPIFFS, "/foo.txt");
    deleteFile(SPIFFS, "/foo.txt");
    testFileIO(SPIFFS, "/test.txt");
    deleteFile(SPIFFS, "/test.txt");
    Serial.println( "Test complete" );
}

void build_acc_pts_file (void) {
  // deleteFile(SPIFFS, "/acc_pts.txt");
  if(!SPIFFS.exists("/acc_pts.txt")) {
    Serial.println(F("can't find acc_pts.txt, creating it"));
    File ofile = SPIFFS.open("/acc_pts.txt", FILE_WRITE);
    for(int i = 0 ; i < sizeof_Access_Points ; i++) {
      ofile.printf("%s\t%s\t%lu\n", AccessPoints[i].ssid, AccessPoints[i].pass, AccessPoints[i].tzone);
    }
    ofile.close();
  }
  listDir(SPIFFS, "/", 0);
  readFile(SPIFFS, "/acc_pts.txt");
}

int read_acc_pts_file_and_compare_with_SSIDs (struct WiFiAp * bestAP) {
char filessid[33], pass[65], tzonestr[20];
int tzone, cp, phase, result;
int best_strength;
  Serial.println(F("reading acc_pts.txt file:"));
  memset(filessid, '\0', sizeof(filessid));
  memset(pass, '\0', sizeof(pass));
  memset(tzonestr, '\0', sizeof(tzonestr));
  cp = 0;
  phase = 0;
  result = 0;
  best_strength = -200;
#if WE_HAVE_SPIFFS

  build_acc_pts_file();

  File ifile = SPIFFS.open("/acc_pts.txt", "r");
  if(!ifile){
    return -1;
  }
  Serial.println(F("found /acc_pts.txt"));
  while(ifile.available()) {
    char c;
    c = ifile.read();
    Serial.write(c);
    if(c == '\t') {
      cp = 0;
      phase++;
    }
    else if(c == '\n' || c == '\r') {
      // Now we have filessid, pass, tzone from acc_pts.txt file
      // compare each WiFi object ssid with filessid from the acc_pts.txt file
      for (int i = 0; i < number_of_networks; ++i) {
        // Serial.printf("comparing %s to %s\n", WiFi.SSID(i).c_str(), filessid);
#if NORMAL_WIFI_OPERATION       // disable to test the access point setup code
        // normally, this should be enabled.
        if(!strcmp(WiFi.SSID(i).c_str(), filessid)) {
          Serial.printf("is RSSI %d > best_strength %d ?\n", WiFi.RSSI(i), best_strength);
          if(WiFi.RSSI(i) > best_strength) {
            best_strength = WiFi.RSSI(i);
            result = 1;
            strncpy(bestAP->ssid, filessid, sizeof(BestAP.ssid));
            strncpy(bestAP->pass, pass, sizeof(BestAP.pass));
            memcpy(bestAP->macAddr, WiFi.BSSID(i), 6);
            bestAP->channel = WiFi.channel(i);
	    Serial.printf("tzonestr = %s\n", tzonestr);
	    uint32_t tzid = atol(tzonestr);
            bestAP->tzone = tzid;
            Serial.printf("best strength = %d, best ssid = %s, tz = %u\n", best_strength, filessid, tzid);
            Serial.printf("best channel %d, best mac %02x:%02x:%02x:%02x:%02x:%02x\n", bestAP->channel,
              bestAP->macAddr[0], bestAP->macAddr[1], bestAP->macAddr[2],
              bestAP->macAddr[3], bestAP->macAddr[4], bestAP->macAddr[5]);
          }
        }
#endif
      }
      cp = 0;
      phase = 0;
      memset(filessid, '\0', sizeof(filessid));
      memset(pass, '\0', sizeof(pass));
      memset(tzonestr, '\0', sizeof(tzonestr));
    }
    else {
      switch(phase) {
        case 0 :
          filessid[cp++] = c;
          break;
        case 1 :
          pass[cp++] = c;
          break;
        case 2 :
          tzonestr[cp++] = c;
          break;
      }
    }
  }
  ifile.close();
#else
  for(int i = 0 ; i < sizeof(AccessPoints)/sizeof(WIFIAP) ; i++) {
    if(!strcmp(WiFi.SSID(i).c_str(), AccessPoints[i].ssid)) {
      Serial.printf("is RSSI %d > best_strength %d ?\n", WiFi.RSSI(i), best_strength);
      if(WiFi.RSSI(i) > best_strength) {
	best_strength = WiFi.RSSI(i);
	result = 1;
	strncpy(bestAP->ssid, AccessPoints[i].ssid, sizeof(BestAP.ssid));
	strncpy(bestAP->pass, AccessPoints[i].pass, sizeof(BestAP.pass));
	memcpy(bestAP->macAddr, WiFi.BSSID(i), 6);
	bestAP->channel = WiFi.channel(i);
	bestAP->tzone = AccessPoints[i].tzone;
	Serial.printf("best strength = %d, best ssid = %s\n", best_strength, filessid);
	Serial.printf("best channel %d, best mac %02x:%02x:%02x:%02x:%02x:%02x\n", bestAP->channel,
	  bestAP->macAddr[0], bestAP->macAddr[1], bestAP->macAddr[2],
	  bestAP->macAddr[3], bestAP->macAddr[4], bestAP->macAddr[5]);
      }
    }
    // AccessPoints[i].ssid, AccessPoints[i].pass, AccessPoints[i].tzone
  }
#endif
  return result;
}

void append_new_access_point (char *newssid, char *newpassword, uint32_t newtzidx) {
char buff[256];
    sprintf(buff, "%s\t%s\t%lu\n", newssid, newpassword, newtzidx);
    appendFile(SPIFFS, "/acc_pts.txt", buff);
}
