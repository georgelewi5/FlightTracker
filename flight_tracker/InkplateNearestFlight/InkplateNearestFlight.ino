#include "Config.h"
#include "Models.h"
#include "DataManager.h"
#include "Ui.h"
#include "Utils.h"

Inkplate display(INKPLATE_3BIT);
DataManager dataManager;
ScreenLayout layout(display);

void setup() {
#if DEBUG
  Serial.begin(115200);
  delay(250);
  DBG("\n=== Inkplate Nearest Flight (modular) ===\n");
#endif

  display.begin();
  display.clearDisplay(); display.display();

  if (!dataManager.begin()) {
    layout.showError("Init failed", dataManager.lastError(), "Retry in 1 min");
    goDeepSleep(REFRESH_SEC);
  }

  Flight f;
  if (!dataManager.fetchNearestFlight(f)) {
    layout.showError("No nearby flights", dataManager.lastError(), "Retry in 1 min");
    goDeepSleep(REFRESH_SEC);
  }

  // Build screen from sections
  HeaderSection header(display);
  RouteSection  route(display);
  MetaSection   meta(display);

  String title = f.callsign.length() ? f.callsign : (f.reg.length() ? f.reg : "Nearest flight");
  String from  = f.origin.length() ? f.origin : "???";
  String to    = f.destination.length() ? f.destination : "???";

  char metaBuf[96];
  if (!isnan(f.groundSpeed_kts))
    snprintf(metaBuf, sizeof(metaBuf), "%.0f kts • %s • %.1f km", f.groundSpeed_kts, f.aircraftType.c_str(), f.dist_km);
  else if (!isnan(f.altitude_ft))
    snprintf(metaBuf, sizeof(metaBuf), "%.0fft • %s • %.1f km", f.altitude_ft, f.aircraftType.c_str(), f.dist_km);
  else
    snprintf(metaBuf, sizeof(metaBuf), "%s • %.1f km", f.aircraftType.c_str(), f.dist_km);

  layout
    .add(&header, title)
    .add(&route,  from + "  \xE2\x86\x92  " + to)
    .add(&meta,   String(metaBuf))
    .render();

#if DEBUG
  DBG("[SHOW] %s | %s | %s\n", title.c_str(), (from + "->" + to).c_str(), metaBuf);
#endif

  goDeepSleep(REFRESH_SEC);
}

void loop() {}
