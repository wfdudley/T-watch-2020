// Turn off the watch
// W.F.Dudley Jr.
//

#include "config.h"
#include "DudleyWatch.h"

void appShutdown(void) {
  ttgo->power->shutdown();
}
