#ifndef __APP_MQTT_H__
#define __APP_MQTT_H__

EXTERN struct menu_item sensor_menu1[]
#ifdef __MQTT_MAIN__
  = {
      { "G.Out Temp", "tele/sensors/outside/temperature", NULL},
      { "N.Out Temp", "tele/light/nflood/SENSOR$.DS18S20.Temperature", NULL},
      { "Attic Temp", "tele/fan/attic/SENSOR$.DS18B20.Temperature", NULL},
      { "Gar. Humid", "tele/light/garage/SENSOR$.AM2301.Humidity", NULL},
      { "Gar. Temp", "tele/light/garage/SENSOR$.AM2301.Temperature", NULL},
      { "Out Humid", "tele/sensors/outside/humidity", NULL},
      { "Crawl Hum", "tele/sensors/crawlspace/humidity", NULL},
      { "Srvr Temp", "tele/sensors/server/temperature", NULL},
      { "2 Fl Temp", "tele/sensors/up/temperature", NULL},
      { "Sensors 2", "", NULL},
      { "Ctrl 1",    "", NULL},
      { "Exit",      "", NULL}
    }
#endif
    ;

EXTERN struct menu_item sensor_menu2[]
#ifdef __MQTT_MAIN__
  = {
      { "LivRm Temp", "tele/sensors/down/temperature", NULL},
      { "Kit. Temp",  "tele/thermostat/kitchen/temperature", NULL},
      { "Music Temp", "tele/thermostat/musicrm/temperature", NULL},
      { "Bath Hum",   "tele/pump/mbath/SENSOR$.AM2301.Humidity", NULL},
      { "Bath Temp",  "tele/pump/mbath/SENSOR$.AM2301.Temperature", NULL},
      { "Dew Point",  "computed/outside/dewpoint", NULL},
      { "Power W",    "filtered/emon/w", NULL},
      { "L1 Amps", "filtered/emon/ct1", NULL},
      { "L2 Amps", "filtered/emon/ct2", NULL},
      { "Sensors 1", "", sensor_menu1 },
      { "Ctrl 1", "", NULL},
      { "Exit", "", NULL}
    }
#endif
    ;

EXTERN struct menu_item ctrl_menu1[]
#ifdef __MQTT_MAIN__
  = {
      { "SLR lamp", "tele/light/slr/STATE$.POWER1", NULL},
      { "couch lamp", "tele/light/couch/STATE$.POWER", NULL},
      { "ELR lamp", "tele/light/elr/STATE$.POWER", NULL},
      { "NLR lamp", "tele/light/nlr/STATE$.POWER", NULL},
      { "django heater", "tele/heater/django/STATE$.POWER", NULL},
      { "art spots", "tele/light/slr/STATE$.POWER2", NULL},
      { "foyer light", "tele/light/fflood/STATE$.POWER1", NULL},
      { "front flood", "tele/light/fflood/STATE$.POWER2", NULL},
      { "crawl fan", "tele/fan/crawlspace/STATE$.POWER", NULL},
      { "Sensors 1", "", sensor_menu1 },
      { "Ctrl 2", "", NULL},
      { "Exit", "", NULL}
    }
#endif
    ;

EXTERN struct menu_item ctrl_menu2[]
#ifdef __MQTT_MAIN__
  = {
      { "west flood", "tele/light/west/STATE$.POWER", NULL},
      { "garage flood", "tele/light/garage/STATE$.POWER", NULL},
      { "bath recirc", "tele/pump/mbath/STATE$.POWER", NULL},
      { "downstairs AC", "tele/ac/down/STATE$.POWER", NULL},
      { "upstairs AC", "tele/ac/up/STATE$.POWER", NULL},
      { "upstairs fan", "tele/fan/up/STATE$.POWER", NULL},
      { "window S 2nd", "tele/window/south2nd/STATE$.POWER", NULL},
      { "window W 2nd", "tele/window/west2nd/STATE$.POWER", NULL},
      { "attic fan", "tele/fan/attic/STATE$.POWER", NULL},
      { "Ctrl 1", "", ctrl_menu1},
      { "Sensors 1", "", sensor_menu1 },
      { "Exit", "", NULL}
    }
#endif
    ;

#endif
