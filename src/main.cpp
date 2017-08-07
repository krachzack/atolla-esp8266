#include <Arduino.h>

#include "config.h"
#include "canvas.h"

Canvas& canvas = Canvas::instance;

void setup() {
  pinMode(PIN_GPIO_0, OUTPUT);
  pinMode(PIN_GPIO_2, OUTPUT);
  pinMode(PIN_TX, OUTPUT);
}

void loop() {
  unsigned long time = (millis() / 10) % (3*255);

  std::vector<uint8_t> colors = {0, 0, 0};
  if(time < 255) {
    colors[0] = 255-time;
  } else if(time < (255*2)) {
    colors[1] = 255-(time % 255);
  } else {
    colors[2] = 255-(time % 255);
  }

  canvas.paint(colors);

  //canvas.update();

  /*ShiftregState black;
  ShiftregState white;
  white.set();

  ShiftregState red;
  red[0] = 1;
  red[3] = 1;
  ShiftregState green;
  green[1] = 1;
  green[4] = 1;
  ShiftregState blue;
  blue[2] = 1;
  blue[5] = 1;

  shiftreg_set(red);
  delay(500);
  shiftreg_set(green);
  delay(500);
  shiftreg_set(blue);
  delay(500);
  shiftreg_set(black);
  delay(2000);*/


  /*set_shiftreg_output(true, false, false);
  delay(1000);
  set_shiftreg_output(false, true, false);
  delay(1000);
  set_shiftreg_output(false, false, true);
  delay(1000);
  set_shiftreg_output(true, true, true);
  delay(1000);
  set_shiftreg_output(false, false, false);
  delay(1000);*/
}
