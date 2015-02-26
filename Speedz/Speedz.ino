#include <StopWatch.h>
#include <stdio.h>
#include <LiquidCrystal.h>
//#include <LiquidCrystal_SR.h>

//#define _debug_

#define START      13
#define NOB         3 // Number of Buttons and Leds
#define testTone    11

#define redBut      8
#define blueBut     9
//#define whiteBut    9
//#define greenBut   6
//#define yellowBut  7

#define redLed     10
#define blueLed    12
//#define whiteLed   12
//#define greenLed  10
//#define yellowLed 11

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
void myDelay(int delaytime);

void setup() {

  #ifdef _debug_
    Serial.begin(9600);
  #endif

  lcd.begin(16, 2);

  lcd.setCursor(0,0);
  lcd.print("SPEEDZ by");
  lcd.setCursor(0,1);
  lcd.print("Mick Kirkegaard");
  delay(2000);
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("SPEEDZ");

  pinMode(START, INPUT);
  pinMode(testTone, OUTPUT);

  pinMode(redBut, INPUT);
  pinMode(blueBut, INPUT);
//  pinMode(whiteBut, INPUT);
  //  pinMode(greenBut,INPUT);
  //  pinMode(yellowBut,INPUT);

  pinMode(redLed, OUTPUT);
  pinMode(blueLed, OUTPUT);
//  pinMode(whiteLed, OUTPUT);
  //  pinMode(greenLed,OUTPUT);
  //  pinMode(yellowLed,OUTPUT);

  digitalWrite(redLed, LOW);
  digitalWrite(blueLed, LOW);
//  digitalWrite(whiteLed, LOW);
  //  digitalWrite(greenLed,LOW);
  //  digitalWrite(yellowLed,LOW);
}

void loop() {
  int pressed = B11111111;
  
  if (digitalRead(redBut))
    pressed = (pressed & B11111110);
  if (digitalRead(blueBut))
    pressed = (pressed & B11111101);
//  if (digitalRead(whiteBut))
//    pressed = (pressed & B11111011);
  //  if(digitalRead(greenBut))
  //    pressed = (pressed & B11110111);
  //  if(digitalRead(yellowBut))
  //    pressed = (pressed & B11101111);

  if (pressed == gameStatus) {
    
    // Change LED to something differnt
    lastGameStatus = gameStatus;
    while(lastGameStatus == gameStatus)
      gameStatus = (B11111111 &  ~(B00000001 << random(NOB)));
    lengthCounter--;
    if(lengthCounter == 0)
      recordScore = 1;
    //myDelay(100);
  }
            // Debug
            #ifdef _debug_
            if (digitalRead(whiteBut)) {
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
  if(lengthCounter > 0) {
    if (gameStatus == B11111110)
      digitalWrite(redLed, HIGH);
    else
      digitalWrite(redLed, LOW);
    if (gameStatus == B11111101)
      digitalWrite(blueLed, HIGH);
    else
      digitalWrite(blueLed, LOW);
//    if (gameStatus == B11111011)
//      digitalWrite(whiteLed, HIGH);
//    else
//      digitalWrite(whiteLed, LOW);
//  if (gameStatus == B11110111)
//    digitalWrite(greenLed, HIGH);
//  else
//    digitalWrite(greenLed, LOW);
//  if (gameStatus == B11101111)
//    digitalWrite(yellowLed, HIGH);
//  else
//    digitalWrite(yellowLed, LOW);
  }
  else {
    lengthCounter = 0;
    digitalWrite(redLed, LOW);
    digitalWrite(blueLed, LOW);
//    digitalWrite(whiteLed, LOW);
//    digitalWrite(greenLed,LOW);
//    digitalWrite(yellowLed,LOW);
    
    if (digitalRead(START)) {
      // START SEQUENCE
        digitalWrite(redLed,HIGH);
        digitalWrite(blueLed,HIGH);
        tone(11, 500, 1000);
//        digitalWrite(whiteLed,HIGH);
//        digitalWrite(greenLed,HIGH);
//        digitalWrite(yellowLed,HIGH);
        delay(1000);
        digitalWrite(redLed,LOW);
        digitalWrite(blueLed,LOW);
//        digitalWrite(whiteLed,LOW);
//        digitalWrite(greenLed,LOW);
//        digitalWrite(yellowLed,LOW);
        delay(500);
        digitalWrite(redLed,HIGH);
        digitalWrite(blueLed,HIGH);
        tone(11, 500, 1000);
//        digitalWrite(whiteLed,HIGH);
//        digitalWrite(greenLed,HIGH);
//        digitalWrite(yellowLed,HIGH);
        delay(1000);
        digitalWrite(redLed,LOW);
        digitalWrite(blueLed,LOW);
//        digitalWrite(whiteLed,LOW);
//        digitalWrite(greenLed,LOW);
//        digitalWrite(yellowLed,LOW);
        delay(500);
        digitalWrite(redLed,HIGH);
        digitalWrite(blueLed,HIGH);
        tone(11, 500, 1000);
//        digitalWrite(whiteLed,HIGH);
//        digitalWrite(greenLed,HIGH);
//        digitalWrite(yellowLed,HIGH);
        delay(1000);
        digitalWrite(redLed,LOW);
        digitalWrite(blueLed,LOW);
//        digitalWrite(whiteLed,LOW);
//        digitalWrite(greenLed,LOW);
//        digitalWrite(yellowLed,LOW);
        delay(500);
        tone(11, 1000, 1000);
        delay(1000);
      swMilisecs.reset();
      swSeconds.reset();
      swMilisecs.start();
      swSeconds.start();
      lengthCounter = gameLength;
      recordScore = 0;
      gameStatus = (B11111111 &  ~(B00000001 << random(NOB)));
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

void myDelay(int delayTime) {
  int currentTime = millis();
  int breakTime = (currentTime + delayTime);
  while(breakTime > currentTime) {
    currentTime = millis();
    #ifdef _debug_
      Serial.print('\n');
      Serial.print(currentTime);
    #endif
  }
}

