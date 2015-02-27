#include <StopWatch.h>
#include <stdio.h>
#include <LiquidCrystal.h>
#include "LedHC138.h"
//#include <LiquidCrystal_SR.h>
#include <Wire.h>

//#define _debug_

#define BUT_NUM 3 // Number of Buttons and Leds
#define SPEAKER 11 // Speakerport PWM

#define BUT_RED 7
#define BUT_BLU 8
#define BUT_WHI 9
#define BUT_GRE 10
#define BUT_YEL 12

#define LED_RED 1
#define LED_BLU 2
#define LED_WHI 3
#define LED_GRE 4
#define LED_YEL 5
#define LED_OFF 0  // LED OFF HACK

const int PIN_A0 = 7;  // 74HC138 PIN A0
const int PIN_A1 = 8;  // 74HC138 PIN A1
const int PIN_A2 = 9;  // 74HC138 PIN A2
LedHC138 led(PIN_A0, PIN_A1, PIN_A2);

// LCD
#define RS  0
#define RW  1
#define E   2
#define D4  3
#define D5  4
#define D6  5
#define D7  6
LiquidCrystal lcd(RS, RW, E, D4, D5, D6, D7);

// LCD3
//#define SRDATA  7
//#define SRCLOCK 8
//#define ENABLE  9
//LiquidCrystal_SR_LCD3 lcd(SRDATA, SRCLOCK, ENABLE);

StopWatch swMilisecs;
StopWatch swSeconds(StopWatch::SECONDS);

char minutesOutput[10];
char secondsOutput[10];
char hundredsOutput[10];

int minutesHiscore  = 99;
int secondsHiscore  = 99;
int hundredsHiscore = 99;

int gameLength = 10;
int lengthCounter = B00000000;
int gameStatus = B00000000;
int lastGameStatus = 0;
int recordScore = 0;

// myFunctions
void myDelay(int delayMS);

void setup() {

#ifdef _debug_
  Serial.begin(9600);
#endif

  lcd.begin(16, 2);

  lcd.setCursor(0, 0);
  lcd.print("SPEEDZ by");
  lcd.setCursor(0, 1);
  lcd.print("Mick Kirkegaard");
  delay(2000);
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("SPEEDZ");

  pinMode(PIN_A0, OUTPUT);
  pinMode(PIN_A1, OUTPUT);
  pinMode(PIN_A2, OUTPUT);

  pinMode(SPEAKER, OUTPUT);

  pinMode(BUT_RED, INPUT);
  pinMode(BUT_BLU, INPUT);
  pinMode(BUT_WHI, INPUT);
  pinMode(BUT_GRE, INPUT);
  pinMode(BUT_YEL, INPUT);
}

void loop() {
  int pressed = B11111111;

  if (digitalRead(BUT_RED))
    pressed = (pressed & B11111110);
  if (digitalRead(BUT_BLU))
    pressed = (pressed & B11111101);
  if (digitalRead(BUT_WHI))
    pressed = (pressed & B11111011);
  if (digitalRead(BUT_GRE))
    pressed = (pressed & B11110111);
  if (digitalRead(BUT_YEL))
    pressed = (pressed & B11101111);

  if (pressed == gameStatus) {
    // Change gameStatus to something differnt
    lastGameStatus = gameStatus;
    while (lastGameStatus == gameStatus)
      gameStatus = (B11111111 &  ~(B00000001 << random(BUT_NUM)));
    lengthCounter--;
    if (lengthCounter == 0)
      recordScore = 1;
    //myDelay(100);
  }

  // Debug
#ifdef _debug_
  if (digitalRead(BUT_WHI)) {
    Serial.print("\ngameLength= ");
    Serial.print(gameLength);
    Serial.print("\nlengthCounter= ");
    Serial.print(lengthCounter);
    Serial.print("\npressed= ");
    Serial.print(pressed, BIN);
    Serial.print("\ngameStatus= ");
    Serial.print(gameStatus, BIN);

    delay(200);
  }
#endif

  // Lights
  if (lengthCounter > 0) {
    if (gameStatus == B11111110)
      led.on(LED_RED);
    if (gameStatus == B11111101)
      led.on(LED_BLU);
    if (gameStatus == B11111011)
      led.on(LED_WHI);
    if (gameStatus == B11110111)
      led.on(LED_GRE);
    if (gameStatus == B11101111)
      led.on(LED_YEL);
  }
  else {
    lengthCounter = 0;
    led.on(LED_OFF);

    if (digitalRead(BUT_RED)) {
      // START SEQUENCE
      led.on(LED_RED);
      led.on(LED_BLU);
      led.on(LED_WHI);
      led.on(LED_GRE);
      led.on(LED_YEL);
      tone(SPEAKER, 1000, 1000);
      delay(1000);
      led.on(LED_OFF);
      delay(500);
      led.on(LED_RED);
      led.on(LED_BLU);
      led.on(LED_WHI);
      led.on(LED_GRE);
      led.on(LED_YEL);
      tone(SPEAKER, 1000, 1000);
      delay(1000);
      led.on(LED_OFF);
      delay(500);
      led.on(LED_RED);
      led.on(LED_BLU);
      led.on(LED_WHI);
      led.on(LED_GRE);
      led.on(LED_YEL);
      delay(1000);
      led.on(LED_OFF);
      delay(500);
      tone(SPEAKER, 1000, 1000);
      delay(1000);
      swMilisecs.reset();
      swSeconds.reset();
      swMilisecs.start();
      swSeconds.start();
      lengthCounter = gameLength;
      recordScore = 0;
      gameStatus = (B11111111 &  ~(B00000001 << random(BUT_NUM)));
    }
  }

  if (recordScore == 1) {
    swMilisecs.stop();
    swSeconds.stop();

    if (((swSeconds.elapsed() / 60) % 60) <= minutesHiscore) {
      if (((swSeconds.elapsed() / 60) % 60) < minutesHiscore) {
        // Print hi-score
        lcd.setCursor(8, 1);
        lcd.print(minutesOutput);
        lcd.setCursor(10, 1);
        lcd.print(":");
        lcd.setCursor(11, 1);
        lcd.print(secondsOutput);
        lcd.setCursor(13, 1);
        lcd.print(":");
        lcd.setCursor(14, 1);
        lcd.print(hundredsOutput);
        minutesHiscore = ((swSeconds.elapsed() / 60) % 60);
        secondsHiscore = (swSeconds.elapsed() % 60);
        hundredsHiscore = (swMilisecs.elapsed() % 100) + 4;
      }
      else if ((swSeconds.elapsed() % 60) <= secondsHiscore) {
        if ((swSeconds.elapsed() % 60) < secondsHiscore) {
          // Print hi-score
          lcd.setCursor(8, 1);
          lcd.print(minutesOutput);
          lcd.setCursor(10, 1);
          lcd.print(":");
          lcd.setCursor(11, 1);
          lcd.print(secondsOutput);
          lcd.setCursor(13, 1);
          lcd.print(":");
          lcd.setCursor(14, 1);
          lcd.print(hundredsOutput);
          minutesHiscore = ((swSeconds.elapsed() / 60) % 60);
          secondsHiscore = (swSeconds.elapsed() % 60);
          hundredsHiscore = (swMilisecs.elapsed() % 100) + 4;
        }
        else if ((swMilisecs.elapsed() % 100) < hundredsHiscore) {
          // Print hi-score
          lcd.setCursor(8, 1);
          lcd.print(minutesOutput);
          lcd.setCursor(10, 1);
          lcd.print(":");
          lcd.setCursor(11, 1);
          lcd.print(secondsOutput);
          lcd.setCursor(13, 1);
          lcd.print(":");
          lcd.setCursor(14, 1);
          lcd.print(hundredsOutput);
          minutesHiscore = ((swSeconds.elapsed() / 60) % 60);
          secondsHiscore = (swSeconds.elapsed() % 60);
          hundredsHiscore = (swMilisecs.elapsed() % 100) + 4;
        }
      }
    }
  }

  //LCD Stopwatch
  lcd.setCursor(8, 0);
  sprintf(minutesOutput, "%2u", ((swSeconds.elapsed() / 60) % 60));
  lcd.print(minutesOutput);
  lcd.setCursor(10, 0);
  lcd.print(":");

  lcd.setCursor(11, 0);
  sprintf(secondsOutput, "%2u", (swSeconds.elapsed() % 60));
  lcd.print(secondsOutput);
  lcd.setCursor(13, 0);
  lcd.print(":");

  lcd.setCursor(14, 0);
  sprintf(hundredsOutput, "%2u", (swMilisecs.elapsed() % 100));
  lcd.print(hundredsOutput);

  lcd.setCursor(0, 1);
  lcd.print("Hiscore:");
}

void myDelay(int delayMS) {
  int currentTime = millis();
  int breakTime = (currentTime + delayMS);
  while (breakTime > currentTime) {
    currentTime = millis();
#ifdef _debug_
    Serial.print('\n');
    Serial.print(currentTime);
#endif
  }
}

