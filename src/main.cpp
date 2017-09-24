#include <Arduino.h>
#include <ESP8266mDNS.h>
#include <EEPROM.h>
#include <atolla/sink.h>
#include <WiFiManager.h>          //https://github.com/tzapu/WiFiManager

#include "canvas.h"
#include "shiftreg.h"

extern "C" {
  #include "user_interface.h"
}

os_timer_t repaintTimer;

#define REPAINT_TIMER_US_PERIOD 4500
#define MAX_HOSTNAME_LEN 30
#define EEPROM_MAGIC0 0xa1
#define EEPROM_MAGIC1 0xcf

Canvas& canvas = Canvas::instance;
const AtollaSinkSpec sinkSpec = {
  10042, // port
  SHIFTREG_OUTPUT_COUNT/3 // two lights
};
AtollaSink sink;
AtollaSinkState sinkState = ATOLLA_SINK_STATE_OPEN;

String selectedHostname;

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

void loadEEPROM() {
  EEPROM.begin(2+MAX_HOSTNAME_LEN);
  byte magic0 = EEPROM.read(0);
  byte magic1 = EEPROM.read(1);

  // Only read if data does not seem like garbage by checking the magic numbers
  if(magic0 == EEPROM_MAGIC0 && magic1 == EEPROM_MAGIC1) {
    char hostname[MAX_HOSTNAME_LEN+1];
    for(size_t i = 0; i < MAX_HOSTNAME_LEN; ++i) {
      hostname[i] = EEPROM.read(2+i);
    }
    hostname[MAX_HOSTNAME_LEN] = '\0';
    selectedHostname = hostname;
  }
}

void persistEEPROM() {
  EEPROM.write(0, EEPROM_MAGIC0);
  EEPROM.write(1, EEPROM_MAGIC1);

  for(int i = 0; i < selectedHostname.length(); ++i) {
    EEPROM.write(2+i, selectedHostname[i]);
  }

  EEPROM.commit();
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

  String defaultHostname("atolla-");
  char chipnumStr[32];
  itoa(ESP.getChipId(), chipnumStr, 16);
  defaultHostname += chipnumStr;

  loadEEPROM();

  WiFiManagerParameter hostnameParam(
    "hostname", // ID
    "hostname", // Placeholder
    defaultHostname.c_str(), // Default value
    MAX_HOSTNAME_LEN, // maximum length
    selectedHostname.c_str() // Will be custom value, right now same as default value
  );
  
  wifiManager.addParameter(&hostnameParam);

  //set callback that gets called when connecting to previous WiFi fails, and enters Access Point mode
  wifiManager.setAPCallback(configModeCallback);

  if(selectedHostname.length() == 0) {
    // If EEPROM was unwritten or out of date, reset
    wifiManager.resetSettings();
  }

  //fetches ssid and pass and tries to connect
  //if it does not connect it starts an access point with the specified name
  //here  "AutoConnectAP"
  //and goes into a blocking loop awaiting configuration
  if(!wifiManager.autoConnect(defaultHostname.c_str())) {
    Serial.begin(9600);
    Serial.println("failed to connect and hit timeout");
    //reset and try again, or maybe put it to deep sleep
    ESP.reset();
    delay(1000);
  }

  ShiftregState green;
  green[1] = true;
  shiftreg_set(green);

  selectedHostname = hostnameParam.getValue();

  persistEEPROM();

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
  WiFi.hostname(selectedHostname);
  MDNS.begin(selectedHostname.c_str());
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

void paintIdleAnimation() {

  const unsigned int every_ms = 15000;
  const unsigned int for_ms = 1500;
  const unsigned int green_low = 0;
  const unsigned int green_high = 80;

  unsigned int t = millis() % every_ms;

  if(t < for_ms) {
    uint8_t green;

    if(t < (for_ms/2)) {
      double a = t / (for_ms/2);
      // ken perlin smoothstep https://en.wikipedia.org/wiki/Smoothstep
      a = a*a*a*(a*(a*6 - 15) + 10);
      green = (uint8_t) (green_low + a*(green_high-green_low));
    } else {
      double a = (t-(for_ms/2)) / (for_ms/2);
      a = a*a*a*(a*(a*6 - 15) + 10);
      green = (uint8_t) (green_high - a*(green_high-green_low));
    }

    canvas.paint(0, green, 0);
  } else {
    canvas.clear();
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
    paintIdleAnimation();
  }
}

void loop() {
  receive();
}