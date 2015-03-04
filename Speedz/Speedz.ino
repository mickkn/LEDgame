#include <StopWatch.h>
#include <stdio.h>
//#include <LiquidCrystal.h>
#include "LedHC138.h"
#include <LiquidCrystal_SR_LCD3.h>
#include <Wire.h>

#define BUT_NUM 3  // Number of Buttons and Leds
#define SPEAKER 11 // Speakerport PWM

#define BUT_RED 0
#define BUT_BLU 1
#define BUT_WHI 2
#define BUT_GRE 3
#define BUT_YEL 4

#define LED_RED 1
#define LED_BLU 2
#define LED_WHI 3
#define LED_GRE 4
#define LED_YEL 5
#define LED_OFF 0  // LED-OFF HACK

const int LED_A0 = 5;  // 74HC138 PIN A0
const int LED_A1 = 6;  // 74HC138 PIN A1
const int LED_A2 = 7;  // 74HC138 PIN A2
LedHC138 led(LED_A0, LED_A1, LED_A2);

// LCD
//#define RS  0
//#define RW  1
//#define E   2
//#define D4  3
//#define D5  4
//#define D6  5
//#define D7  6
//LiquidCrystal lcd(RS, RW, E, D4, D5, D6, D7);

// LCD3
#define SRDATA   8
#define STROBE   9
#define SRCLOCK 10
LiquidCrystal_SR_LCD3 lcd(SRDATA, SRCLOCK, STROBE);

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

void setup() {

  pinMode(LED_A0, OUTPUT);
  pinMode(LED_A1, OUTPUT);
  pinMode(LED_A2, OUTPUT);

  pinMode(SPEAKER, OUTPUT);

  pinMode(BUT_RED, INPUT);
  pinMode(BUT_BLU, INPUT);
  pinMode(BUT_WHI, INPUT);
  //  pinMode(BUT_GRE, INPUT);
  //  pinMode(BUT_YEL, INPUT);

  led.on(LED_OFF);

  lcd.begin(16, 2);

  lcd.setCursor(0, 0);
  lcd.print("SPEEDZ by");
  lcd.setCursor(0, 1);
  lcd.print("Mick Kirkegaard");
  delay(2000);
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("SPEEDZ");
}

void loop() {
  int pressed = B11111111;

  // Button presses
  if (digitalRead(BUT_RED))
    pressed = (pressed & B11111110);
  if (digitalRead(BUT_BLU))
    pressed = (pressed & B11111101);
  if (digitalRead(BUT_WHI))
    pressed = (pressed & B11111011);
  //  if (digitalRead(BUT_GRE))
  //    pressed = (pressed & B11110111);
  //  if (digitalRead(BUT_YEL))
  //    pressed = (pressed & B11101111);

  // Change gameStatus
  if (pressed == gameStatus) {
    lastGameStatus = gameStatus;
    while (lastGameStatus == gameStatus)
      gameStatus = (B11111111 &  ~(B00000001 << random(BUT_NUM)));
    lengthCounter--;
    if (lengthCounter == 0) {
      recordScore = 1;
      gameStatus = B00000000;
    }
    tone(SPEAKER, random(800, 1700), 50);
  }

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

    if (digitalRead(BUT_RED) && digitalRead(BUT_BLU)) {
      // START SEQUENCE
      tone(SPEAKER, 700, 500);
      led.on(LED_RED);
      delay(50);
      led.on(LED_BLU);
      delay(50);
      led.on(LED_WHI);
      delay(50);
      led.on(LED_GRE);
      delay(50);
      led.on(LED_YEL);
      delay(50);
      led.on(LED_YEL);
      delay(50);
      led.on(LED_GRE);
      delay(50);
      led.on(LED_WHI);
      delay(50);
      led.on(LED_BLU);
      delay(50);
      led.on(LED_RED);
      delay(50);
      led.on(LED_OFF);
      delay(500);
      
      tone(SPEAKER, 700, 500);
      led.on(LED_YEL);
      delay(50);
      led.on(LED_GRE);
      delay(50);
      led.on(LED_WHI);
      delay(50);
      led.on(LED_BLU);
      delay(50);
      led.on(LED_RED);
      delay(50);
      led.on(LED_RED);
      delay(50);
      led.on(LED_BLU);
      delay(50);
      led.on(LED_WHI);
      delay(50);
      led.on(LED_GRE);
      delay(50);
      led.on(LED_YEL);
      delay(50);
      led.on(LED_OFF);
      delay(500);
      
      tone(SPEAKER, 700, 500);
      led.on(LED_RED);
      delay(50);
      led.on(LED_BLU);
      delay(50);
      led.on(LED_WHI);
      delay(50);
      led.on(LED_GRE);
      delay(50);
      led.on(LED_YEL);
      delay(50);
      led.on(LED_YEL);
      delay(50);
      led.on(LED_GRE);
      delay(50);
      led.on(LED_WHI);
      delay(50);
      led.on(LED_BLU);
      delay(50);
      led.on(LED_RED);
      delay(50);
      led.on(LED_OFF);
      delay(500);
      
      tone(SPEAKER, 1400, 500);
      delay(500);
      swMilisecs.reset();
      swSeconds.reset();
      swMilisecs.start();
      swSeconds.start();
      lengthCounter = gameLength;
      recordScore = 0;
      gameStatus = (B11111111 &  ~(B00000001 << random(BUT_NUM)));
    }
  }

  if ((recordScore == 1) && (lengthCounter == 0)) {
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
  int minOut;
  lcd.setCursor(9, 0);
  minOut = ((swSeconds.elapsed() / 60) % 60);
  lcd.print(minOut);
  if(minOut < 10) {
    lcd.setCursor(8,0);
    lcd.print("0");
  }
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

