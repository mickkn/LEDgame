#include <StopWatch.h>
#include <stdio.h>
#include <LiquidCrystal_SR_LCD3.h>
#include <Wire.h>
#include <EEPROM.h>
#include "LedHC138.h"
#include "pitches.h"

#define BUT_NUM 5  // Number of BUTTONS and LEDS
#define SPEAKER 6  // Speakerport PWM

#define BUT_RED 0  // Red Button pin
#define BUT_BLU 1  // Blue Button pin
#define BUT_WHI 2  // White Button pin
#define BUT_GRE 3  // Green Button pin
#define BUT_YEL 4  // Yellow Button pin

#define LED_RED 4  // Red LED number (not pin)
#define LED_BLU 5  // Blue LED number (not pin)
#define LED_WHI 3  // White LED number (not pin)
#define LED_GRE 2  // Green LED number (not pin)
#define LED_YEL 1  // Yellow LED number (not pin)
#define LED_OFF 0  // LED-OFF HACK

const int LED_A0 = 12;  // 74HC138 PIN A0
const int LED_A1 = 11;  // 74HC138 PIN A1
const int LED_A2 = 10;  // 74HC138 PIN A2
LedHC138 led(LED_A0, LED_A1, LED_A2);

// LCD3
#define SRDATA  8  // HEF4094 Data pin
#define STROBE  7  // HEF4094 Strobe pin
#define SRCLOCK 9  // HEF4094 Clock pin
LiquidCrystal_SR_LCD3 lcd(SRDATA, SRCLOCK, STROBE);

// menu variables
// -----------------------------------
static int menu_chooseGame = 0;
static int menu_playIntro = 1;
void menu(void);

// speedz variables
// -----------------------------------
int speedz_gameLength = 1;
int speedz_lengthCounter = B00000000;
int speedz_gameStatus = B00000000;
int speedz_lastGameStatus = 0;
int speedz_recordScore = 0;
char speedz_minutesOutput[10];
char speedz_secondsOutput[10];
char speedz_hundredsOutput[10];
char speedz_minutesHiOut[10];
char speedz_secondsHiOut[10];
char speedz_hundredsHiOut[10];
const int speedz_minHiAddr = 0;
const int speedz_secHiAddr = 1;
const int speedz_hunHiAddr = 2;
int speedz_minutesHiscore = EEPROM.read(speedz_minHiAddr);
int speedz_secondsHiscore = EEPROM.read(speedz_secHiAddr);
int speedz_hundredsHiscore = EEPROM.read(speedz_hunHiAddr);
void speedz(void);
StopWatch swMilisecs;
StopWatch swSeconds(StopWatch::SECONDS);

// memoz variables
// -----------------------------------
#define MEMOZSIZE 100
int memoz_gameSequence[MEMOZSIZE];
int memoz_makeNewSequence = 1;
int memoz_currentLocation;
int memoz_sequenceLength;
int memoz_playSequence;
int memoz_gameEnd;
int memoz_nextLocation;
const int memoz_redTone = NOTE_C4;
const int memoz_bluTone = NOTE_D4;
const int memoz_whiTone = NOTE_E4;
const int memoz_greTone = NOTE_F4;
const int memoz_yelTone = NOTE_G4;
const int memoz_toneLength = 500;
void memoz(void);

// pickz variables
// -----------------------------------
#define PICKZSTARTDELAY 1000
const int pickz_levelHiAddr = 3;
int pickz_Level = 0;
int pickz_Hiscore = EEPROM.read(pickz_levelHiAddr);
int pickz_Correct = 0;
int pickz_Delay = PICKZSTARTDELAY;
char pickz_levelOutput[10];
char pickz_levelHiOut[10];
void pickz(void);
void pickzPress(void);

// clear hiscore
// -----------------------------------
void clearHiscore(void);

// *******************************************
/*
  Setup
*/
// *******************************************

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
  attachInterrupt(0,clearHiscore,LOW);  // Clear hiscore interrupt on BUT_WHI
  delay(3000);
  detachInterrupt(0);
  lcd.clear();
}

// *******************************************
/*
  Loop
*/
// *******************************************

void loop() {
  
  // Pick a Game
  if((((!digitalRead(BUT_RED) && !digitalRead(BUT_BLU)) && !digitalRead(BUT_WHI)) && !digitalRead(BUT_GRE)) && !digitalRead(BUT_YEL)) {
    detachInterrupt(0);
    menu_chooseGame = 0;
    menu_playIntro = 1;
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("LED Games by");
    lcd.setCursor(0, 1);
    lcd.print("Mick Kirkegaard");
    delay(3000);
  }
  if(menu_chooseGame == 0) {
    menu(); 
  }
  if(menu_chooseGame == 1) {
    if(menu_playIntro == 1) {
      lcd.setCursor(0, 0);
      lcd.print("SPEEDZ by");
      lcd.setCursor(0, 1);
      lcd.print("Mick Kirkegaard");
      delay(2000);
      lcd.clear();
      menu_playIntro = 0;
    }
    speedz();
  }
  if(menu_chooseGame == 2) {
    if(menu_playIntro == 1) {
      lcd.setCursor(0, 0);
      lcd.print("MEMOZ by");
      lcd.setCursor(0, 1);
      lcd.print("Mick Kirkegaard");
      delay(2000);
      lcd.clear();
      menu_playIntro = 0;
    }
    memoz();
  }
  if(menu_chooseGame == 4) {
    if(menu_playIntro == 1) {
      lcd.setCursor(0, 0);
      lcd.print("PICKZ by");
      lcd.setCursor(0, 1);
      lcd.print("Mick Kirkegaard");
      delay(2000);
      lcd.clear();
      menu_playIntro = 0;
    }
    pickz();
  }

}

// *******************************************
/*
  Speedz - Code for the multitask reflex game Speedz
  Created by Mick Kirkegaard 2015.
*/
// *******************************************

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
    
    tone(SPEAKER, random(800, 1500), 80); // Press-tone
    
    if (speedz_lengthCounter == 0) {
      speedz_recordScore = 1;
      speedz_gameStatus = B00000000;
    }
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
    led.on(LED_WHI);
    delay(20);
    led.on(LED_RED);

    if (!digitalRead(BUT_WHI)) {
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
  
  // Determine Hi-score
  if ((speedz_recordScore == 1) && (speedz_lengthCounter == 0)) {
    swMilisecs.stop();
    swSeconds.stop();

    if (((swSeconds.elapsed() / 60) % 60) <= speedz_minutesHiscore) {
      if (((swSeconds.elapsed() / 60) % 60) < speedz_minutesHiscore) {
        speedz_minutesHiscore = ((swSeconds.elapsed() / 60) % 60);
        speedz_secondsHiscore = (swSeconds.elapsed() % 60);
        speedz_hundredsHiscore = (swMilisecs.elapsed() % 100);
      }
      else if ((swSeconds.elapsed() % 60) <= speedz_secondsHiscore) {
        if ((swSeconds.elapsed() % 60) < speedz_secondsHiscore) {
          speedz_minutesHiscore = ((swSeconds.elapsed() / 60) % 60);
          speedz_secondsHiscore = (swSeconds.elapsed() % 60);
          speedz_hundredsHiscore = (swMilisecs.elapsed() % 100);
        }
        else if ((swMilisecs.elapsed() % 100) < speedz_hundredsHiscore) {
          speedz_minutesHiscore = ((swSeconds.elapsed() / 60) % 60);
          speedz_secondsHiscore = (swSeconds.elapsed() % 60);
          speedz_hundredsHiscore = (swMilisecs.elapsed() % 100);
        }
      }
    }
  }

  // Print Hi-score
  EEPROM.write(speedz_minHiAddr,speedz_minutesHiscore);
  EEPROM.write(speedz_secHiAddr,speedz_secondsHiscore);
  EEPROM.write(speedz_hunHiAddr,speedz_hundredsHiscore);
  lcd.setCursor(0, 1);
  lcd.print("Hiscore");
  sprintf(speedz_minutesHiOut, "%2u", speedz_minutesHiscore);
  sprintf(speedz_secondsHiOut, "%2u", speedz_secondsHiscore);
  sprintf(speedz_hundredsHiOut, "%2u", speedz_hundredsHiscore);
  lcd.setCursor(8, 1);
  lcd.print(speedz_minutesHiOut);
  lcd.setCursor(10, 1);
  lcd.print(":");
  lcd.setCursor(11, 1);
  lcd.print(speedz_secondsHiOut);
  lcd.setCursor(13, 1);
  lcd.print(":");
  lcd.setCursor(14, 1);
  lcd.print(speedz_hundredsHiOut);

  //LCD Stopwatch
  lcd.setCursor(0,0);
  lcd.print("SPEEDZ ");
  
  lcd.setCursor(8, 0);
  sprintf(speedz_minutesOutput, "%2u", ((swSeconds.elapsed() / 60) % 60));
  lcd.print(speedz_minutesOutput);
  lcd.setCursor(10, 0);
  lcd.print(":");

  lcd.setCursor(11, 0);
  sprintf(speedz_secondsOutput, "%2u", (swSeconds.elapsed() % 60));
  lcd.print(speedz_secondsOutput);
  lcd.setCursor(13, 0);
  lcd.print(":");

  lcd.setCursor(14, 0);
  sprintf(speedz_hundredsOutput, "%2u", (swMilisecs.elapsed() % 100));
  lcd.print(speedz_hundredsOutput);
}

// *******************************************
/*
  Memoz - Code for the Simon Says-like game Memoz
  Created by Mick Kirkegaard 2015.
*/
// *******************************************

void memoz() {
  
  lcd.setCursor(0,0);
  lcd.print("MEMOZ");
  
  const int memoz_pushDelay = 2000;
  
  // Fill array with random numbers one time
  if(memoz_makeNewSequence == 1) {
    for(int i = 0 ; i < MEMOZSIZE ; i++) {
      memoz_gameSequence[i] = random(1,5);
    }
    memoz_makeNewSequence = 0;
    memoz_currentLocation = 1;
    memoz_sequenceLength = 1;
    memoz_playSequence = 1;
  }
  
  // if end of sequence, play from the start
  if(memoz_playSequence == 1) {
    memoz_currentLocation = 0;
    for(int i = 0 ; i < memoz_sequenceLength ; i++) {
      if(memoz_gameSequence[i] == 1) {
        led.on(LED_RED);
        tone(SPEAKER,memoz_redTone,memoz_toneLength);
      }
      else if(memoz_gameSequence[i] == 2) {
        led.on(LED_BLU);
        tone(SPEAKER,memoz_bluTone,memoz_toneLength);
      }
      else if(memoz_gameSequence[i] == 3) {
        led.on(LED_WHI);
        tone(SPEAKER,memoz_whiTone,memoz_toneLength);
      }   
      else if(memoz_gameSequence[i] == 4) {
        led.on(LED_GRE);
        tone(SPEAKER,memoz_greTone,memoz_toneLength);
      }   
      else if(memoz_gameSequence[i] == 5) {
        led.on(LED_YEL);
        tone(SPEAKER,memoz_yelTone,memoz_toneLength);
      }   
      delay(1000);
    }
    memoz_playSequence = 0;
    led.on(LED_OFF);
  }
  
  // Stay in loop until correct or incorrect buttonpush
  while(memoz_gameEnd == 0 || memoz_nextLocation == 1) {
    if(memoz_gameSequence[memoz_currentLocation] == 1) {
      if(!digitalRead(BUT_RED)) {
        tone(SPEAKER,memoz_redTone,memoz_toneLength);
        led.on(BUT_RED);
        delay(memoz_pushDelay);
        break;
      }
      else if(!digitalRead(BUT_BLU) || !digitalRead(BUT_WHI) || !digitalRead(BUT_GRE) || !digitalRead(BUT_YEL)) {
        memoz_gameEnd = 1;
      }  
    }
    else if(memoz_gameSequence[memoz_currentLocation] == 2) {
      if(!digitalRead(BUT_BLU)) {
        tone(SPEAKER,memoz_bluTone,memoz_toneLength);
        led.on(BUT_BLU);
        delay(memoz_pushDelay);
        break;
      }
      else if(!digitalRead(BUT_RED) || !digitalRead(BUT_WHI) || !digitalRead(BUT_GRE) || !digitalRead(BUT_YEL)) {
        memoz_gameEnd = 1;
      } 
    }
    else if(memoz_gameSequence[memoz_currentLocation] == 3) {
      if(!digitalRead(BUT_WHI)) {
        tone(SPEAKER,memoz_whiTone,memoz_toneLength);
        led.on(BUT_WHI);
        delay(memoz_pushDelay);
        break;
      }
      else if(!digitalRead(BUT_BLU) || !digitalRead(BUT_RED) || !digitalRead(BUT_GRE) || !digitalRead(BUT_YEL)) {
        memoz_gameEnd = 1;
      } 
    }
    else if(memoz_gameSequence[memoz_currentLocation] == 4) {
      if(!digitalRead(BUT_GRE)) {
        tone(SPEAKER,memoz_greTone,memoz_toneLength);
        led.on(BUT_GRE);
        delay(memoz_pushDelay);
        break;
      }
      else if(!digitalRead(BUT_BLU) || !digitalRead(BUT_WHI) || !digitalRead(BUT_RED) || !digitalRead(BUT_YEL)) {
        memoz_gameEnd = 1;
      } 
    }
    else if(memoz_gameSequence[memoz_currentLocation] == 5) {
      if(!digitalRead(BUT_YEL)) {
        tone(SPEAKER,memoz_yelTone,memoz_toneLength);
        led.on(BUT_YEL);
        delay(memoz_pushDelay);
        break;
      }
      else if(!digitalRead(BUT_BLU) || !digitalRead(BUT_WHI) || !digitalRead(BUT_GRE) || !digitalRead(BUT_RED)) {
        memoz_gameEnd = 1;
      } 
    }
  }
  if(memoz_gameEnd == 0)
    memoz_currentLocation++;
    if(memoz_currentLocation >= memoz_sequenceLength) {
      memoz_sequenceLength++;
      memoz_playSequence = 1;
    }
  else {
    memoz_makeNewSequence = 1;
    tone(SPEAKER,NOTE_AS1,2000);
  }
}

// *******************************************
/*
  Pickz - Code for the reflex game Pickz
  Created by Mick Kirkegaard 2015.
*/
// *******************************************

void pickz() {
  attachInterrupt(0, pickzPress, LOW);

  lcd.setCursor(13,0);
  sprintf(pickz_levelOutput,"%2d",pickz_Level);
  lcd.print(pickz_levelOutput);
  lcd.setCursor(13,1);
  sprintf(pickz_levelHiOut,"%2d",pickz_Hiscore);
  lcd.print(pickz_levelHiOut);

  lcd.setCursor(0,0);
  lcd.print("PICKZ   Lvl:");
  lcd.setCursor(0,1);
  lcd.print("       HiSc:");
  
  pickz_Correct = 0;
  led.on(LED_RED);
  delay(pickz_Delay);
  if((((!digitalRead(BUT_RED) && !digitalRead(BUT_BLU)) && !digitalRead(BUT_WHI)) && !digitalRead(BUT_GRE)) && !digitalRead(BUT_YEL))
    detachInterrupt(0);
  else
    attachInterrupt(0, pickzPress, LOW);
    
  pickz_Correct = 0;
  led.on(LED_GRE);
  delay(pickz_Delay);
  if((((!digitalRead(BUT_RED) && !digitalRead(BUT_BLU)) && !digitalRead(BUT_WHI)) && !digitalRead(BUT_GRE)) && !digitalRead(BUT_YEL))
    detachInterrupt(0);
  else
    attachInterrupt(0, pickzPress, LOW);
    
  pickz_Correct = 1;
  led.on(LED_WHI);
  delay(pickz_Delay);
  if((((!digitalRead(BUT_RED) && !digitalRead(BUT_BLU)) && !digitalRead(BUT_WHI)) && !digitalRead(BUT_GRE)) && !digitalRead(BUT_YEL))
    detachInterrupt(0);
  else
    attachInterrupt(0, pickzPress, LOW);
    
  pickz_Correct = 0;
  led.on(LED_BLU);
  delay(pickz_Delay);
  if((((!digitalRead(BUT_RED) && !digitalRead(BUT_BLU)) && !digitalRead(BUT_WHI)) && !digitalRead(BUT_GRE)) && !digitalRead(BUT_YEL))
    detachInterrupt(0);
  else
    attachInterrupt(0, pickzPress, LOW);
    
  pickz_Correct = 0;
  led.on(LED_YEL);
  delay(pickz_Delay);
  if((((!digitalRead(BUT_RED) && !digitalRead(BUT_BLU)) && !digitalRead(BUT_WHI)) && !digitalRead(BUT_GRE)) && !digitalRead(BUT_YEL))
    detachInterrupt(0);
  else
    attachInterrupt(0, pickzPress, LOW);
    
  pickz_Correct = 0;
  led.on(LED_BLU);
  delay(pickz_Delay);
  if((((!digitalRead(BUT_RED) && !digitalRead(BUT_BLU)) && !digitalRead(BUT_WHI)) && !digitalRead(BUT_GRE)) && !digitalRead(BUT_YEL))
    detachInterrupt(0);
  else
    attachInterrupt(0, pickzPress, LOW);
    
  pickz_Correct = 1;
  led.on(LED_WHI);
  delay(pickz_Delay);
  if((((!digitalRead(BUT_RED) && !digitalRead(BUT_BLU)) && !digitalRead(BUT_WHI)) && !digitalRead(BUT_GRE)) && !digitalRead(BUT_YEL))
    detachInterrupt(0);
  else
    attachInterrupt(0, pickzPress, LOW);
    
  pickz_Correct = 0;
  led.on(LED_GRE);
  delay(pickz_Delay);
  
}

void pickzPress() {
  detachInterrupt(0);
  if(pickz_Correct == 1) {
    pickz_Level = pickz_Level+1;
    pickz_Delay = (pickz_Delay - (pickz_Delay/20)); // Decrement with 5%
    tone(SPEAKER, 1500,100);
  }
  else {
    pickz_Level = 0;
    pickz_Delay = PICKZSTARTDELAY;
    tone(SPEAKER, 100,1000);
  }
  lcd.setCursor(13,0);
  sprintf(pickz_levelOutput,"%2d",pickz_Level);
  lcd.print(pickz_levelOutput);
  if(pickz_Level > pickz_Hiscore) {
    pickz_Hiscore = pickz_Level;
    lcd.setCursor(13,1);
    sprintf(pickz_levelHiOut,"%2d",pickz_Hiscore);
    lcd.print(pickz_levelHiOut);
    EEPROM.write(pickz_levelHiAddr,pickz_Hiscore);
  }
  delay(1000);
  //attachInterrupt(0, pickzPress, LOW);
}

// *******************************************
/*
  Clear the hiscore (working on introscreen)
  Created by Mick Kirkegaard 2015.
*/ 
// *******************************************

void clearHiscore() {
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Clearing");
  lcd.setCursor(0,1   );
  lcd.print("all hi-scores...");
  EEPROM.write(speedz_minHiAddr,99);
  EEPROM.write(speedz_secHiAddr,99);
  EEPROM.write(speedz_hunHiAddr,99);
  EEPROM.write(pickz_levelHiAddr,0);
  delay(4000);
}

// *******************************************
/*
  Menu for picking the Gamemode
  Created by Mick Kirkegaard 2015.
*/ 
// *******************************************

void menu() {
  int currentTime;
  int breakTime;
  int delayTime = 1500;
  lcd.setCursor(0,0);
  lcd.print("   PICK  GAME   ");
  
  if(menu_chooseGame == 0) {
    lcd.setCursor(0,1);
    lcd.print("     SPEEDZ     ");
    currentTime = millis();
    breakTime = (currentTime + delayTime);
    while(currentTime < breakTime) {
      currentTime = millis();
      led.on(LED_RED);
      if(!digitalRead(BUT_RED)) {
        menu_chooseGame = 1;
        lcd.clear();
        break;
      }
      if(!digitalRead(BUT_GRE)) {
        menu_chooseGame = 2;
        lcd.clear();
        break;
      }
      if(!digitalRead(BUT_YEL)) {
        menu_chooseGame = 3;
        lcd.clear();
        break;
      }
      if(!digitalRead(BUT_BLU)) {
        menu_chooseGame = 4;
        lcd.clear();
        break;
      }
    }
  }
  
  if(menu_chooseGame == 0) {
    lcd.setCursor(0,1);
    lcd.print("     MEMOZ     ");
    currentTime = millis();
    breakTime = (currentTime + delayTime);
    while(currentTime < breakTime) {
      currentTime = millis();
      led.on(LED_GRE);
      if(!digitalRead(BUT_RED)) {
        menu_chooseGame = 1;
        lcd.clear();
        break;
      }
      if(!digitalRead(BUT_GRE)) {
        menu_chooseGame = 2;
        lcd.clear();
        break;
      }
      if(!digitalRead(BUT_YEL)) {
        menu_chooseGame = 3;
        lcd.clear();
        break;
      }
      if(!digitalRead(BUT_BLU)) {
        menu_chooseGame = 4;
        lcd.clear();
        break;
      }
    }
  }
  if(menu_chooseGame == 0) {
    lcd.setCursor(0,1);
    lcd.print("     COMING..     ");
    currentTime = millis();
    breakTime = (currentTime + delayTime);
    while(currentTime < breakTime) {
      currentTime = millis();
      led.on(LED_YEL);
      if(!digitalRead(BUT_RED)) {
        menu_chooseGame = 1;
        lcd.clear();
        break;
      }
      if(!digitalRead(BUT_GRE)) {
        menu_chooseGame = 2;
        lcd.clear();
        break;
      }
      if(!digitalRead(BUT_YEL)) {
        menu_chooseGame = 3;
        lcd.clear();
        break;
      }
      if(!digitalRead(BUT_BLU)) {
        menu_chooseGame = 4;
        lcd.clear();
        break;
      }
    }
  }
  
  if(menu_chooseGame == 0) {
    lcd.setCursor(0,1);
    lcd.print("     PICKZ     ");
    currentTime = millis();
    breakTime = (currentTime + delayTime);
    while(currentTime < breakTime) {
      currentTime = millis();
      led.on(LED_BLU);
      if(!digitalRead(BUT_RED)) {
        menu_chooseGame = 1;
        lcd.clear();
        break;
      }
      if(!digitalRead(BUT_GRE)) {
        menu_chooseGame = 2;
        lcd.clear();
        break;
      }
      if(!digitalRead(BUT_YEL)) {
        menu_chooseGame = 3;
        lcd.clear();
        break;
      }
      if(!digitalRead(BUT_BLU)) {
        menu_chooseGame = 4;
        lcd.clear();
        break;
      }
    }
  }
}

