#include <StopWatch.h>
#include <stdio.h>
#include "LedHC138.h"
#include <LiquidCrystal_SR_LCD3.h>
#include <Wire.h>

#define BUT_NUM 5  // Number of Buttons and Leds
#define SPEAKER 6 // Speakerport PWM

#define BUT_RED 0
#define BUT_BLU 1
#define BUT_WHI 2
#define BUT_GRE 3
#define BUT_YEL 4

#define LED_RED 4
#define LED_BLU 5
#define LED_WHI 3
#define LED_GRE 2
#define LED_YEL 1
#define LED_OFF 0  // LED-OFF HACK

const int LED_A0 = 12;  // 74HC138 PIN A0
const int LED_A1 = 11;  // 74HC138 PIN A1
const int LED_A2 = 10;  // 74HC138 PIN A2
LedHC138 led(LED_A0, LED_A1, LED_A2);

// LCD3
#define SRDATA  8
#define STROBE  7
#define SRCLOCK 9
LiquidCrystal_SR_LCD3 lcd(SRDATA, SRCLOCK, STROBE);

StopWatch swMilisecs;
StopWatch swSeconds(StopWatch::SECONDS);

char minutesOutput[10];
char secondsOutput[10];
char hundredsOutput[10];

int minutesHiscore  = 99;
int secondsHiscore  = 99;
int hundredsHiscore = 99;

// Game chooser
static int menu_chooseGame = 1;
void menu(void);

// Speedz variables
int speedz_gameLength = 30;
int speedz_lengthCounter = B00000000;
int speedz_gameStatus = B00000000;
int speedz_lastGameStatus = 0;
int speedz_recordScore = 0;
void speedz(void);

// memoz variables
int memozArray[42];
const int memoz_blu = 5;
const int memoz_red = 4;
const int memoz_whi = 3;
const int memoz_gre = 2;
const int memoz_yel = 1;
void memoz(void);

void setup() {

  pinMode(LED_A0, OUTPUT);
  pinMode(LED_A1, OUTPUT);
  pinMode(LED_A2, OUTPUT);

  pinMode(SPEAKER, OUTPUT);

  pinMode(BUT_RED, INPUT);
  pinMode(BUT_BLU, INPUT);
  pinMode(BUT_WHI, INPUT);
  pinMode(BUT_GRE, INPUT);
  pinMode(BUT_YEL, INPUT);

  led.on(LED_OFF);

  lcd.begin(16, 2);

  lcd.setCursor(0, 0);
  lcd.print("LED Games by");
  lcd.setCursor(0, 1);
  lcd.print("Mick Kirkegaard");
  delay(2000);
  lcd.clear();
}

void loop() {
  
  // Game chooser
  if(!digitalRead(BUT_RED) || !digitalRead(BUT_BLU) || !digitalRead(BUT_WHI) || !digitalRead(BUT_GRE) || !digitalRead(BUT_YEL)) {
    menu_chooseGame = 0;
    delay(100);
  }
  if(menu_chooseGame == 0) {
    menu(); 
  }
  if(menu_chooseGame == 1) {
    speedz();
  }

}

void speedz() {
  int pressed = B11111111;

  // Button presses
  if (!digitalRead(BUT_RED))
    pressed = (pressed & B11111110);
  if (!digitalRead(BUT_BLU))
    pressed = (pressed & B11111101);
  if (!digitalRead(BUT_WHI))
    pressed = (pressed & B11111011);
  if (!digitalRead(BUT_GRE))
    pressed = (pressed & B11110111);
  if (!digitalRead(BUT_YEL))
    pressed = (pressed & B11101111);

  // Change speedz_gameStatus
  if (pressed == speedz_gameStatus) {
    speedz_lastGameStatus = speedz_gameStatus;
    while (speedz_lastGameStatus == speedz_gameStatus)
      speedz_gameStatus = (B11111111 &  ~(B00000001 << random(BUT_NUM)));
    speedz_lengthCounter--;
    if (speedz_lengthCounter == 0) {
      speedz_recordScore = 1;
      speedz_gameStatus = B00000000;
    }
    tone(SPEAKER, random(800, 1500), 80);
  }

  // Lights
  if (speedz_lengthCounter > 0) {
    if (speedz_gameStatus == B11111110)
      led.on(LED_RED);
    if (speedz_gameStatus == B11111101)
      led.on(LED_BLU);
    if (speedz_gameStatus == B11111011)
      led.on(LED_WHI);
    if (speedz_gameStatus == B11110111)
      led.on(LED_GRE);
    if (speedz_gameStatus == B11101111)
      led.on(LED_YEL);
  }
  else {
    speedz_lengthCounter = 0;
    led.on(LED_OFF);

    if (!(digitalRead(BUT_WHI) || digitalRead(BUT_RED))) {
      // START SEQUENCE
      int startDelay = 50;
      tone(SPEAKER, 700, 500);
      led.on(LED_RED);
      delay(startDelay);
      led.on(LED_BLU);
      delay(startDelay);
      led.on(LED_WHI);
      delay(startDelay);
      led.on(LED_GRE);
      delay(startDelay);
      led.on(LED_YEL);
      delay(startDelay);
      led.on(LED_YEL);
      delay(startDelay);
      led.on(LED_GRE);
      delay(startDelay);
      led.on(LED_WHI);
      delay(startDelay);
      led.on(LED_BLU);
      delay(startDelay);
      led.on(LED_RED);
      delay(startDelay);
      led.on(LED_OFF);
      delay(500);
      
      tone(SPEAKER, 700, 500);
      led.on(LED_YEL);
      delay(startDelay);
      led.on(LED_GRE);
      delay(startDelay);
      led.on(LED_WHI);
      delay(startDelay);
      led.on(LED_BLU);
      delay(startDelay);
      led.on(LED_RED);
      delay(startDelay);
      led.on(LED_RED);
      delay(startDelay);
      led.on(LED_BLU);
      delay(startDelay);
      led.on(LED_WHI);
      delay(startDelay);
      led.on(LED_GRE);
      delay(startDelay);
      led.on(LED_YEL);
      delay(startDelay);
      led.on(LED_OFF);
      delay(500);
      
      tone(SPEAKER, 700, 500);
      led.on(LED_RED);
      delay(startDelay);
      led.on(LED_BLU);
      delay(startDelay);
      led.on(LED_WHI);
      delay(startDelay);
      led.on(LED_GRE);
      delay(startDelay);
      led.on(LED_YEL);
      delay(startDelay);
      led.on(LED_YEL);
      delay(startDelay);
      led.on(LED_GRE);
      delay(startDelay);
      led.on(LED_WHI);
      delay(startDelay);
      led.on(LED_BLU);
      delay(startDelay);
      led.on(LED_RED);
      delay(startDelay);
      led.on(LED_OFF);
      delay(500);
      
      tone(SPEAKER, 1400, 500);
      delay(500);
      swMilisecs.reset();
      swSeconds.reset();
      swMilisecs.start();
      swSeconds.start();
      speedz_lengthCounter = speedz_gameLength;
      speedz_recordScore = 0;
      speedz_gameStatus = (B11111111 &  ~(B00000001 << random(BUT_NUM)));
    }
  }

  if ((speedz_recordScore == 1) && (speedz_lengthCounter == 0)) {
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
  lcd.setCursor(0,0);
  lcd.print("SPEEDSZ ");
  
  int minOut;
  lcd.setCursor(9, 0);
  minOut = ((swSeconds.elapsed() / 60) % 60);
  lcd.print(minOut);
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

void memoz() {
  
  
  lcd.setCursor(0,0);
  lcd.print("MEMOZ  ");
}

void menu() {
  int currentTime;
  int breakTime;
  int delayTime = 1500;
  lcd.setCursor(0,0);
  lcd.print("  CHOOSE GAME  ");
  
  if(menu_chooseGame == 0) {
    lcd.setCursor(0,1);
    lcd.print("    SPEEDZ     ");
    currentTime = millis();
    breakTime = (currentTime + delayTime);
    while(currentTime < breakTime) {
      currentTime = millis();
      led.on(LED_RED);
      if(!digitalRead(BUT_RED)) {
        menu_chooseGame = 1;
        break;
      }
      if(!digitalRead(BUT_GRE)) {
        menu_chooseGame = 2;
        break;
      }
      if(!digitalRead(BUT_YEL)) {
        menu_chooseGame = 3;
        break;
      }
      if(!digitalRead(BUT_BLU)) {
        menu_chooseGame = 4;
        break;
      }
    }
  }
  
  if(menu_chooseGame == 0) {
    lcd.setCursor(0,1);
    lcd.print("    MEMOZ     ");
    currentTime = millis();
    breakTime = (currentTime + delayTime);
    while(currentTime < breakTime) {
      currentTime = millis();
      led.on(LED_GRE);
      if(!digitalRead(BUT_RED)) {
        menu_chooseGame = 1;
        break;
      }
      if(!digitalRead(BUT_GRE)) {
        menu_chooseGame = 2;
        break;
      }
      if(!digitalRead(BUT_YEL)) {
        menu_chooseGame = 3;
        break;
      }
      if(!digitalRead(BUT_BLU)) {
        menu_chooseGame = 4;
        break;
      }
    }
  }
  if(menu_chooseGame == 0) {
    lcd.setCursor(0,1);
    lcd.print("    TEST1     ");
    currentTime = millis();
    breakTime = (currentTime + delayTime);
    while(currentTime < breakTime) {
      currentTime = millis();
      led.on(LED_YEL);
      if(!digitalRead(BUT_RED)) {
        menu_chooseGame = 1;
        break;
      }
      if(!digitalRead(BUT_GRE)) {
        menu_chooseGame = 2;
        break;
      }
      if(!digitalRead(BUT_YEL)) {
        menu_chooseGame = 3;
        break;
      }
      if(!digitalRead(BUT_BLU)) {
        menu_chooseGame = 4;
        break;
      }
    }
  }
  
  if(menu_chooseGame == 0) {
    lcd.setCursor(0,1);
    lcd.print("    TEST2     ");
    currentTime = millis();
    breakTime = (currentTime + delayTime);
    while(currentTime < breakTime) {
      currentTime = millis();
      led.on(LED_BLU);
      if(!digitalRead(BUT_RED)) {
        menu_chooseGame = 1;
        break;
      }
      if(!digitalRead(BUT_GRE)) {
        menu_chooseGame = 2;
        break;
      }
      if(!digitalRead(BUT_YEL)) {
        menu_chooseGame = 3;
        break;
      }
      if(!digitalRead(BUT_BLU)) {
        menu_chooseGame = 4;
        break;
      }
    }
  }
}

