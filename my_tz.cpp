#include "config.h"
#include "DudleyWatch.h"
#include <AceTime.h>
#define __TZMAIN__ 1
#include "my_tz.h"

using namespace ace_time;

// Create a BasicZoneManager with the entire TZ Database.
static const int CACHE_SIZE = 3;
static BasicZoneManager<CACHE_SIZE> manager(
    zonedb::kZoneRegistrySize, zonedb::kZoneRegistry);

void init_timezones(void) {
  auto GMT_tz = manager.createForZoneId(TZ_GMT);
  auto homeTz = manager.createForZoneId(general_config.home_tzindex);
  auto localTz = manager.createForZoneId(general_config.local_tzindex);
}

// get time, convert to a "local" tz
void get_time_in_tz(uint32_t tzoneId) {
  auto GMT_tz = manager.createForZoneId(TZ_GMT);
  tnow = ttgo->rtc->getDateTime();
  auto GMT_Time = ZonedDateTime::forComponents(
    tnow.year, tnow.month, tnow.day,
    tnow.hour, tnow.minute, tnow.second, GMT_tz);
  // systemClock.setNow(GMT_Time.toEpochSeconds());
  auto localTz = manager.createForZoneId(tzoneId);
  auto localTime = GMT_Time.convertToTimeZone(localTz);
  hh     = localTime.hour();
  mm     = localTime.minute();
  ss     = localTime.second();
  dday   = localTime.day();
  mmonth = localTime.month();
  yyear  = localTime.year();
}

void convert_alarm_time_to_gmt(int8_t *ahhp, int8_t *ammp) {
  tnow = ttgo->rtc->getDateTime();
  tnow.hour = general_config.alarm_h;
  tnow.minute = general_config.alarm_m;
  auto local_tz = manager.createForZoneId(general_config.local_tzindex);
  auto Alarm_Time = ZonedDateTime::forComponents(
    tnow.year, tnow.month, tnow.day,
    tnow.hour, tnow.minute, tnow.second, local_tz);
  // Serial.printf("local alarm time is %02d:%02d\n", tnow.hour, tnow.minute);
  acetime_t epochSeconds = Alarm_Time.toEpochSeconds();
  // Serial.print("Epoch seconds: ");
  // Serial.println(epochSeconds);
  auto alarmTz = manager.createForZoneId(TZ_GMT);
  auto alarmTime = ZonedDateTime::forEpochSeconds(epochSeconds, alarmTz);
  // Serial.print(F("Alarm Time (GMT): "));
  // alarmTime.printTo(Serial);
  // Serial.println();
  *ahhp = alarmTime.hour();
  *ammp = alarmTime.minute();
  // Serial.printf("convert_alarm_time_to_gmt: %d:%02d\n", *ahhp, *ammp);
}
