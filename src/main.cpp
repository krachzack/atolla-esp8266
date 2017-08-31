#include <Arduino.h>
#include <ESP8266mDNS.h>
#include <atolla/sink.h>
#include <WiFiManager.h>          //https://github.com/tzapu/WiFiManager

#include "canvas.h"
#include "shiftreg.h"

extern "C" {
  #include "user_interface.h"
}

os_timer_t repaintTimer;

#define REPAINT_TIMER_US_PERIOD 4500

#define HOSTNAME "brett"

Canvas& canvas = Canvas::instance;
const AtollaSinkSpec sinkSpec = {
  10042, // port
  SHIFTREG_OUTPUT_COUNT/3 // two lights
};
AtollaSink sink;
AtollaSinkState sinkState = ATOLLA_SINK_STATE_OPEN;

void initAtolla();
void initMDNS();
void initTiming();
void receive();
void repaint();

void configModeCallback (WiFiManager *myWiFiManager) {
  ShiftregState red;
  red[0] = true;
  shiftreg_set(red);

  Serial.begin(9600);
  Serial.println("Entered config mode");
  Serial.println(WiFi.softAPIP());
  //if you used auto generated SSID, print it
  Serial.println(myWiFiManager->getConfigPortalSSID());
  Serial.end();
}

void setup() {
  shiftreg_init();

  ShiftregState yellow;
  yellow[0] = true;
  yellow[1] = true;
  shiftreg_set(yellow);

  
  //WiFiManager
  //Local intialization. Once its business is done, there is no need to keep it around
  WiFiManager wifiManager;
  //reset settings - for testing
  //wifiManager.resetSettings();

  //set callback that gets called when connecting to previous WiFi fails, and enters Access Point mode
  wifiManager.setAPCallback(configModeCallback);

  //fetches ssid and pass and tries to connect
  //if it does not connect it starts an access point with the specified name
  //here  "AutoConnectAP"
  //and goes into a blocking loop awaiting configuration
  if(!wifiManager.autoConnect()) {
    Serial.begin(9600);
    Serial.println("failed to connect and hit timeout");
    //reset and try again, or maybe put it to deep sleep
    ESP.reset();
    delay(1000);
  }

  ShiftregState green;
  green[1] = true;
  shiftreg_set(green);

  //if you get here you have connected to the WiFi
  Serial.begin(9600);
  Serial.println("Connected!");

  canvas.begin();

  initAtolla();
  initMDNS();
  initTiming();
}

void initAtolla() {
  sink = atolla_sink_make(&sinkSpec);
}

void initMDNS() {
#ifdef HOSTNAME
  String hostname(HOSTNAME);
#else
  String hostname("atolla-");
  hostname += String(ESP.getChipId(), HEX);
#endif
  WiFi.hostname(hostname);
  MDNS.begin(hostname.c_str());
  MDNS.addService("atolla", "udp", (uint16_t) sinkSpec.port);
}

void initTiming() {
  //receiveTicker.attach_ms(20, receive);
  //repaintTicker.attach_ms(50, repaint);
  os_timer_setfn(&repaintTimer, (os_timer_func_t *) repaint, NULL);
  os_timer_arm_us(&repaintTimer, REPAINT_TIMER_US_PERIOD, true);
}

void receive() {
  sinkState = atolla_sink_state(sink);

  if(sinkState == ATOLLA_SINK_STATE_ERROR) {
    // If there is an error, restart the sink
    atolla_sink_free(sink);
    sink = atolla_sink_make(&sinkSpec);
  }
}

void repaint() {
  if(sinkState == ATOLLA_SINK_STATE_LENT) {
    bool ok = atolla_sink_get(sink, canvas.thresholds.data(), canvas.thresholds.size());
    if(!ok) {
      //Serial.println("Lent - Clearing");
      canvas.clear();
    }
  } else if(sinkState == ATOLLA_SINK_STATE_OPEN) {
    // idle mode, flash some green
    int t = millis() / 2;
    uint8_t val = (uint8_t) (abs((t % 512) - 256));
    canvas.paint(0, val, 0);
  }
}

void loop() {
  receive();
}