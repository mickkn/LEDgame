#include "LedHC138.h"

// LED output 74HC138 3-to-8 Line Decoder (inverting)
#define LED_RED 1
#define LED_YEL 2
#define LED_BLU 3
#define LED_GRE 4
#define LED_WHI 5

const int PORT_ = 7;
const int PORT__ = 8;
const int PORT___ = 9;

LedHC138 leds(PORT_, PORT__, PORT___);

void setup() {
  pinMode(PORT_, OUTPUT);
  pinMode(PORT__, OUTPUT);
  pinMode(PORT___, OUTPUT);
}

void loop() {
  leds.on(LED_RED);
  delay(500);
  leds.on(LED_YEL);
  delay(500);
  leds.on(LED_BLU);
  delay(500);
  leds.on(LED_GRE);
  delay(500);
  leds.on(LED_WHI);
  delay(500);
}

