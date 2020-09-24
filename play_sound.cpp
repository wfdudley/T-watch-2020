#pragma mark - Depend ESP8266Audio and ESP8266_Spiram libraries
/*
cd ~/Arduino/libraries
git clone https://github.com/earlephilhower/ESP8266Audio
git clone https://github.com/Gianbacchio/ESP8266_Spiram
*/
// derived from the PlayMP3fromPROGMEM demo code
// W.F.Dudley Jr.

#include "config.h"
#include "DudleyWatch.h"
#include <WiFi.h>
#include <HTTPClient.h>         // Remove Audio Lib error
#include <AudioFileSourcePROGMEM.h>
#include <AudioFileSourceID3.h>
#include <AudioGeneratorMP3.h>
#include <AudioOutputI2S.h>
#include "mp3files.h"

AudioGeneratorMP3 *mp3;
AudioFileSourcePROGMEM *file;
AudioOutputI2S *out;
AudioFileSourceID3 *id3;

uint8_t number_of_sounds = sizeof(beep_sizes)/sizeof(uint16_t);

#define MP3_DEBUG 0
void beep(int8_t which_sound) {
#if 0
    WiFi.mode(WIFI_OFF);
    tft->setTextFont(2);
    tft->setCursor(0, 0);
    tft->fillScreen(TFT_BLACK);
    tft->println("Sample MP3 playback begins");
#endif
#if MP3_DEBUG
    Serial.println(F("Sample MP3 playback begins"));
#endif

    //!Turn on the audio power
    ttgo->enableLDO3();
#if MP3_DEBUG
    Serial.println(F("after power on"));
#endif

    file = new AudioFileSourcePROGMEM(beep_array[which_sound], beep_sizes[which_sound]);
#if MP3_DEBUG
    Serial.println(F("after AudioFileSourcePROGMEM"));
    Serial.printf("sizeof sound %d is %u\n", which_sound, beep_sizes[which_sound]);
#endif

    id3 = new AudioFileSourceID3(file);
#if MP3_DEBUG
    Serial.println(F("after new AudioFileSourceID3"));
#endif

#if defined(STANDARD_BACKPLANE)
    Serial.println(F("STANDARD_BACKPLANE"));
    out = new AudioOutputI2S(0, 1);
#elif defined(EXTERNAL_DAC_BACKPLANE)
#if MP3_DEBUG
    Serial.println(F("EXTERNAL_DAC_BACKPLANE"));
#endif

    out = new AudioOutputI2S();
#if MP3_DEBUG
    Serial.println(F("after AudioOutputI2S"));
#endif

    //External DAC decoding
    out->SetPinout(TWATCH_DAC_IIS_BCK, TWATCH_DAC_IIS_WS, TWATCH_DAC_IIS_DOUT);
#if MP3_DEBUG
    Serial.printf("after SetPinout(%d, %d, %d)\n", TWATCH_DAC_IIS_BCK, TWATCH_DAC_IIS_WS, TWATCH_DAC_IIS_DOUT);
#endif
#endif
    out->SetGain(4.0 * ((float)general_config.alarm_volume)/100.0);
#if MP3_DEBUG
    Serial.println(F("after SetGain(4)"));
#endif

    mp3 = new AudioGeneratorMP3();
#if MP3_DEBUG
    Serial.println(F("after new AudioGeneratorMP3()"));
#endif

    mp3->begin(id3, out);
#if MP3_DEBUG
    Serial.println(F("after begin(id3, out)"));
#endif

    while(mp3->isRunning()) {
        if (!mp3->loop()) mp3->stop();
    }
#if MP3_DEBUG
    Serial.printf("MP3 done\n");
#endif
#if 0
    tft->println("Sample MP3 playback done.");
#endif
#if MP3_DEBUG
    delay(500);
#endif
}
