#include "canvas.h"
#include "WiFiManager.h"          //https://github.com/tzapu/WiFiManager
#include "shiftreg.h"

Canvas& canvas = Canvas::instance;

void configModeCallback (WiFiManager *myWiFiManager) {
  ShiftregState red;
  red[0] = true;
  shiftreg_set(red);

  Serial.println("Entered config mode");
  Serial.println(WiFi.softAPIP());
  //if you used auto generated SSID, print it
  Serial.println(myWiFiManager->getConfigPortalSSID());
}

void setup() {
  Serial.begin(9600);

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
    Serial.println("failed to connect and hit timeout");
    //reset and try again, or maybe put it to deep sleep
    ESP.reset();
    delay(1000);
  }

  //if you get here you have connected to the WiFi
  Serial.println("connected...yeey :)");

  canvas.begin();
  //canvas.setAutoUpdate();
}

void loop() {
  canvas.update();
  canvas.paint(0, abs((millis() / 2) % 512 - 256), 0);

  /*ShiftregState green;
  green[1] = true;
  shiftreg_set(green);*/
}