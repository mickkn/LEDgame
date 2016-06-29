/* Arduino Nano */

#include <StopWatch.h>
#include <stdio.h>
#include <LiquidCrystal_SR_LCD3.h>
#include <Wire.h>
//#include <EEPROM.h>
#include "pitches.h"

#define DEBUG 1

#define BUT_NUM  5  // Number of BUTTONS and LEDS (5)
#define SPEAKER  3  // Speakerport PWM (3)
#define BCKLGHT A1  // Backligt strength Active low

// hardware pins
// -----------------------------------
#define BUT_RED  8  // Red Button pin
#define BUT_BLU  9  // Blue Button pin
#define BUT_WHI  2  // White Button pin INT0
#define BUT_YEL 12  // Yellow Button pin
#define BUT_GRE 11  // Green Button pin

#define LED_RED A0  // Red LED pin
#define LED_BLU A2  // Blue LED pin
#define LED_WHI A4  // White LED pin
#define LED_GRE A5  // Green LED pin
#define LED_YEL A3  // Yellow LED pin
char ledChar[5] = {LED_RED,LED_BLU,LED_WHI,LED_GRE,LED_YEL};

// LCD3 (3 wire LCD)
// -----------------------------------
#define SRDATA  6  // HEF4094 Data pin
#define STROBE  7  // HEF4094 Strobe pin
#define SRCLCK  5  // HEF4094 Clock pin
LiquidCrystal_SR_LCD3 lcd(SRDATA, SRCLCK, STROBE);

// danish LCD characters
// -----------------------------------
byte newChar0[8] = { B10000,B01000,B00100,B00010,B00001,B00010,B00100,B00000}; // >
byte Lae[8] = { B00000,B00000,B11010,B00101,B01111,B10100,B11111,B00000}; // æ
byte Loe[8] = { B00000,B00001,B01110,B10101,B10101,B01110,B10000,B00000}; // ø
byte Laa[8] = { B00100,B00000,B01110,B00001,B01111,B10001,B01111,B00000}; // å
byte Sae[8] = { B01111,B10100,B10100,B11110,B10100,B10100,B10111,B00000}; // Æ
byte Soe[8] = { B01110,B10011,B10101,B10101,B10101,B11001,B01110,B00000}; // Ø
byte Saa[8] = { B00100,B00000,B01110,B10001,B11111,B10001,B10001,B00000}; // Å

// menu variables
// -----------------------------------
static int menu_chooseGame = 0;
static int menu_playIntro = 1;
void menu(void);

// EEPROM Addresses

#define speedz_minHiAddr  4
#define speedz_secHiAddr  5
#define speedz_hunHiAddr  6
#define pickz_levelHiAddr 7

// speedz variables
// -----------------------------------
int speedz_gameLength = 30;
int speedz_lengthCounter = B00000000;
int speedz_gameStatus = B00000000;
int speedz_lastGameStatus = 0;
int speedz_recordScore = 0;
int speedz_tones[6] = {NOTE_C6,NOTE_D6,NOTE_E6,NOTE_F6,NOTE_G6,NOTE_A6};
char speedz_minutesOutput[10];
char speedz_secondsOutput[10];
char speedz_hundredsOutput[10];
char speedz_minutesHiOut[10];
char speedz_secondsHiOut[10];
char speedz_hundredsHiOut[10];
int speedz_minutesHiscore = 59;//EEPROM.read(speedz_minHiAddr);
int speedz_secondsHiscore = 59;//EEPROM.read(speedz_secHiAddr);
int speedz_hundredsHiscore = 99;//EEPROM.read(speedz_hunHiAddr);
void speedz(void);
StopWatch swMilisecs;
StopWatch swSeconds(StopWatch::SECONDS);

// memoz variables
// -----------------------------------
#define MEMOZSIZE 100
#define M_RED 1
#define M_BLU 2
#define M_WHI 3
#define M_GRE 4
#define M_YEL 5
int memoz_seqArray[MEMOZSIZE];
const int memoz_redTone = NOTE_C4;
const int memoz_bluTone = NOTE_D4;
const int memoz_whiTone = NOTE_E4;
const int memoz_greTone = NOTE_F4;
const int memoz_yelTone = NOTE_G4;
const int memoz_goTone = NOTE_G2;
const int memoz_toneLength = 500;
int memoz_makeNewSeq = 1;
int memoz_curLoc = 0;
int memoz_curGameLen = 1;
int memoz_playSequence = 1;
int memoz_playGame = 0;
void memoz(void);
void memoz_makeSeq(void);
void memoz_playSeq(void);
void memoz_pressSeq(void);

// pickz variables
// -----------------------------------
#define PICKZSTARTDELAY 1000
int pickz_Level = 0;
int pickz_Hiscore = 0;//EEPROM.read(pickz_levelHiAddr);
int pickz_Correct = 0;
int pickz_Delay = PICKZSTARTDELAY;
char pickz_levelOutput[10];
char pickz_levelHiOut[10];
void pickz(void);
void pickzPress(void);

// other functions
// -----------------------------------
void clearHiscore(void);
void ledOff(void);


// *******************************************
/*
  Setup
*/
// *******************************************

void setup() {
  
  pinMode(LED_RED, OUTPUT);
  pinMode(LED_BLU, OUTPUT);
  pinMode(LED_WHI, OUTPUT);
  pinMode(LED_YEL, OUTPUT);
  pinMode(LED_GRE, OUTPUT);
  ledOff();

  pinMode(SPEAKER, OUTPUT);
  pinMode(BCKLGHT, OUTPUT);

  pinMode(BUT_RED, INPUT_PULLUP);
  pinMode(BUT_BLU, INPUT_PULLUP);
  pinMode(BUT_WHI, INPUT_PULLUP);
  pinMode(BUT_GRE, INPUT_PULLUP);
  pinMode(BUT_YEL, INPUT_PULLUP);
  
  lcd.begin(16, 2);
  lcd.createChar(0, newChar0); // upload 8 selvdef. karakterer
  lcd.createChar(1, Lae); // æ
  lcd.createChar(2, Loe); // ø
  lcd.createChar(3, Laa); // å
  lcd.createChar(4, Sae); // Æ
  lcd.createChar(5, Soe); // Ø
  lcd.createChar(6, Saa); // Å
  
  lcd.setCursor(0, 0);
  lcd.print("LED-spil af");
  lcd.setCursor(0, 1);
  lcd.print("Mick Kirkegaard");
  attachInterrupt(0,clearHiscore,LOW);  // Clear hiscore interrupt on BUT_WHI
  delay(3000);
  detachInterrupt(0);
  lcd.clear();
  if(DEBUG)Serial.begin(115200);
}

// *******************************************
/*
  Loop
*/
// *******************************************

void loop() {
    analogWrite(BCKLGHT,0);
  // Pick a Game
  if((((!digitalRead(BUT_RED) && !digitalRead(BUT_BLU)) && !digitalRead(BUT_WHI)) && !digitalRead(BUT_GRE)) && !digitalRead(BUT_YEL)) {
    detachInterrupt(0);
    menu_chooseGame = 0;
    menu_playIntro = 1;
    speedz_recordScore = 1;
    speedz_lengthCounter = 0;
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("LED-spil af");
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
      lcd.print("SPEEDZ af");
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
      lcd.print("MEMOZ af");
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
      lcd.print("PICKZ af");
      lcd.setCursor(0, 1);
      lcd.print("Mick Kirkegaard");
      delay(2000);
      lcd.clear();
      menu_playIntro = 0;
    }
    pickz();
  }

  if(DEBUG)Serial.write("Hiscores:\n");
  if(DEBUG)Serial.write("Speedz: ");
  //if(DEBUG)Serial.write(EEPROM.read(speedz_minHiAddr));
  if(DEBUG)Serial.write(":");
  //if(DEBUG)Serial.write(EEPROM.read(speedz_secHiAddr));
  if(DEBUG)Serial.write(":");
  //if(DEBUG)Serial.write(EEPROM.read(speedz_hunHiAddr));
  if(DEBUG)Serial.write("\n");
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
    
    tone(SPEAKER, speedz_tones[random(0, 5)], 300); // Press-tone
    
    if (speedz_lengthCounter == 0) {
      speedz_recordScore = 1;
      speedz_gameStatus = B00000000;
      ledOff();
      tone(SPEAKER, NOTE_C3, 1500);
    }
  }

  // Lights
  if (speedz_lengthCounter > 0) {
    if (speedz_gameStatus == B11111110)
      digitalWrite(LED_RED, LOW);
    else
      digitalWrite(LED_RED, HIGH);
    if (speedz_gameStatus == B11111101)
      digitalWrite(LED_BLU, LOW);
    else
      digitalWrite(LED_BLU, HIGH);
    if (speedz_gameStatus == B11111011)
      digitalWrite(LED_WHI, LOW);
    else
      digitalWrite(LED_WHI, HIGH);
    if (speedz_gameStatus == B11110111)
      digitalWrite(LED_GRE, LOW);
    else
      digitalWrite(LED_GRE, HIGH);
    if (speedz_gameStatus == B11101111)
      digitalWrite(LED_YEL, LOW);
    else
      digitalWrite(LED_YEL, HIGH);
  }
  else {
    speedz_lengthCounter = 0;
    digitalWrite(LED_WHI, LOW);
    // make sound

    if (!digitalRead(BUT_WHI)) {
      // START SEQUENCE
      int startDelay = 50;

      tone(SPEAKER, 700, 500);
      // LAV SEKVENS HER
      delay(startDelay * 10);
      delay(500);
      
      tone(SPEAKER, 700, 500);
      // LAV SEKVENS HER
      delay(startDelay * 10);
      delay(500);
      
      tone(SPEAKER, 700, 500);
      // LAV SEKVENS HER
      delay(startDelay * 10);
      delay(500);
      
      tone(SPEAKER, 1400, 500);
      delay(500);
      swMilisecs.reset();
      swSeconds.reset();
      swMilisecs.start();
      swSeconds.start();
      speedz_lengthCounter = speedz_gameLength;
      speedz_recordScore = 0;
      randomSeed(millis());
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
  //EEPROM.write(speedz_minHiAddr,speedz_minutesHiscore);
  //EEPROM.write(speedz_secHiAddr,speedz_secondsHiscore);
  //EEPROM.write(speedz_hunHiAddr,speedz_hundredsHiscore);
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

void memoz(void) {
  
  lcd.setCursor(0,0);
  lcd.print("MEMOZ");

  if(memoz_playGame == 0) {
    digitalWrite(LED_WHI, LOW);
    memoz_curLoc = 0;
    memoz_makeNewSeq = 1;
    memoz_playSequence = 1;
    memoz_curGameLen = 1;
    if(!digitalRead(BUT_WHI)) {
      memoz_playGame = 1;
      ledOff();
    }
  }
  else {
    if(memoz_makeNewSeq == 1) {
      memoz_makeSeq();
      memoz_makeNewSeq = 0;
    }
    
    if(memoz_playSequence == 1) {
      memoz_curLoc = 0;
      memoz_playSeq();
      memoz_playSequence = 0; 
    }
    /*
    if(!digitalRead(BUT_RED))
      memoz_makeNewSeq = 1;
      
    if(!digitalRead(BUT_WHI))
      memoz_playSequence = 1;
  */
  
    memoz_pressSeq();
  }

}

void memoz_pressSeq(void) {
  if(!digitalRead(BUT_RED)) {
    if(memoz_seqArray[memoz_curLoc] == M_RED) {
      tone(SPEAKER,memoz_redTone,memoz_toneLength);
      digitalWrite(LED_RED, LOW);
      delay(memoz_toneLength);
      ledOff();
      memoz_curLoc++;
    }
    else
      tone(SPEAKER,memoz_goTone,memoz_toneLength);
      delay(memoz_toneLength);
      memoz_playGame = 0;
  }  
  if(!digitalRead(BUT_BLU)) {
    if(memoz_seqArray[memoz_curLoc] == M_BLU) {
      tone(SPEAKER,memoz_bluTone,memoz_toneLength);
      digitalWrite(LED_BLU, LOW);
      delay(memoz_toneLength);
      ledOff();
      memoz_curLoc++;
    }
    else
      tone(SPEAKER,memoz_goTone,memoz_toneLength);
      delay(memoz_toneLength);
      memoz_playGame = 0;
  }
  if(!digitalRead(BUT_WHI)) {
    if(memoz_seqArray[memoz_curLoc] == M_WHI) {
      tone(SPEAKER,memoz_whiTone,memoz_toneLength);
      digitalWrite(LED_WHI, LOW);
      delay(memoz_toneLength);
      ledOff();
      memoz_curLoc++;
    }
    else
      tone(SPEAKER,memoz_goTone,memoz_toneLength);
      delay(memoz_toneLength);
      memoz_playGame = 0;
  }
  if(!digitalRead(BUT_GRE)) {
    if(memoz_seqArray[memoz_curLoc] == M_GRE) {
      tone(SPEAKER,memoz_greTone,memoz_toneLength);
      digitalWrite(LED_GRE, LOW);
      delay(memoz_toneLength);
      ledOff();
      memoz_curLoc++;
    }
    else
      tone(SPEAKER,memoz_goTone,memoz_toneLength);
      delay(memoz_toneLength);
      memoz_playGame = 0;
  }
  if(!digitalRead(BUT_YEL)) {
    if(memoz_seqArray[memoz_curLoc] == M_YEL) {
      tone(SPEAKER,memoz_yelTone,memoz_toneLength);
      digitalWrite(LED_YEL, LOW);
      delay(memoz_toneLength);
      ledOff();
      memoz_curLoc++;
    }
    else
      tone(SPEAKER,memoz_goTone,memoz_toneLength);
      delay(memoz_toneLength);
      memoz_playGame = 0;
  }
  
  if(memoz_curLoc == memoz_curGameLen) {
    memoz_curGameLen++;
    memoz_playSequence = 1;
    memoz_curLoc = 0;
  }
}

void memoz_playSeq(void) {
  
  ledOff();
  
  while(memoz_curLoc < memoz_curGameLen) {
    if(memoz_seqArray[memoz_curLoc] == M_RED) {
      tone(SPEAKER,memoz_redTone,memoz_toneLength);
      digitalWrite(LED_RED,LOW);
      delay(memoz_toneLength);
      ledOff();
      delay(memoz_toneLength);
    }
    if(memoz_seqArray[memoz_curLoc] == M_BLU) {
      tone(SPEAKER,memoz_bluTone,memoz_toneLength);
      digitalWrite(LED_BLU,LOW);
      delay(memoz_toneLength);
      ledOff();
      delay(memoz_toneLength);
    }
    if(memoz_seqArray[memoz_curLoc] == M_WHI) {
      tone(SPEAKER,memoz_whiTone,memoz_toneLength);
      digitalWrite(LED_WHI,LOW);
      delay(memoz_toneLength);
      ledOff();
      delay(memoz_toneLength);
    }
    if(memoz_seqArray[memoz_curLoc] == M_GRE) {
      tone(SPEAKER,memoz_greTone,memoz_toneLength);
      digitalWrite(LED_GRE,LOW);
      delay(memoz_toneLength);
      ledOff();
      delay(memoz_toneLength);
    }
    if(memoz_seqArray[memoz_curLoc] == M_YEL) {
      tone(SPEAKER,memoz_yelTone,memoz_toneLength);
      digitalWrite(LED_YEL,LOW);
      delay(memoz_toneLength);
      ledOff();
      delay(memoz_toneLength);
    }
    memoz_curLoc++;
  }
}

void memoz_makeSeq(void) {

  randomSeed(millis());
  
  for(int i = 0 ; i < MEMOZSIZE ; i++) {
    memoz_seqArray[i] = random(1,5);  
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
  digitalWrite(LED_RED, LOW);
  delay(pickz_Delay);
  if((((!digitalRead(BUT_RED) && !digitalRead(BUT_BLU)) && !digitalRead(BUT_WHI)) && !digitalRead(BUT_GRE)) && !digitalRead(BUT_YEL))
    detachInterrupt(0);
  else
    attachInterrupt(0, pickzPress, LOW);
  digitalWrite(LED_RED, HIGH);
    
  pickz_Correct = 0;
  digitalWrite(LED_GRE, LOW);
  delay(pickz_Delay);
  if((((!digitalRead(BUT_RED) && !digitalRead(BUT_BLU)) && !digitalRead(BUT_WHI)) && !digitalRead(BUT_GRE)) && !digitalRead(BUT_YEL))
    detachInterrupt(0);
  else
    attachInterrupt(0, pickzPress, LOW);
  digitalWrite(LED_GRE, HIGH);
  
  pickz_Correct = 1;
  digitalWrite(LED_WHI, LOW);
  delay(pickz_Delay);
  if((((!digitalRead(BUT_RED) && !digitalRead(BUT_BLU)) && !digitalRead(BUT_WHI)) && !digitalRead(BUT_GRE)) && !digitalRead(BUT_YEL))
    detachInterrupt(0);
  else
    attachInterrupt(0, pickzPress, LOW);
  digitalWrite(LED_WHI, HIGH);
  
  pickz_Correct = 0;
  digitalWrite(LED_BLU, LOW);
  delay(pickz_Delay);
  if((((!digitalRead(BUT_RED) && !digitalRead(BUT_BLU)) && !digitalRead(BUT_WHI)) && !digitalRead(BUT_GRE)) && !digitalRead(BUT_YEL))
    detachInterrupt(0);
  else
    attachInterrupt(0, pickzPress, LOW);
  digitalWrite(LED_BLU, HIGH);
  
  pickz_Correct = 0;
  digitalWrite(LED_YEL, LOW);
  delay(pickz_Delay);
  if((((!digitalRead(BUT_RED) && !digitalRead(BUT_BLU)) && !digitalRead(BUT_WHI)) && !digitalRead(BUT_GRE)) && !digitalRead(BUT_YEL))
    detachInterrupt(0);
  else
    attachInterrupt(0, pickzPress, LOW);
  digitalWrite(LED_YEL, HIGH);
  
  pickz_Correct = 0;
  digitalWrite(LED_BLU, LOW);
  delay(pickz_Delay);
  if((((!digitalRead(BUT_RED) && !digitalRead(BUT_BLU)) && !digitalRead(BUT_WHI)) && !digitalRead(BUT_GRE)) && !digitalRead(BUT_YEL))
    detachInterrupt(0);
  else
    attachInterrupt(0, pickzPress, LOW);
  digitalWrite(LED_BLU, HIGH);
  
  pickz_Correct = 1;
  digitalWrite(LED_WHI, LOW);
  delay(pickz_Delay);
  if((((!digitalRead(BUT_RED) && !digitalRead(BUT_BLU)) && !digitalRead(BUT_WHI)) && !digitalRead(BUT_GRE)) && !digitalRead(BUT_YEL))
    detachInterrupt(0);
  else
    attachInterrupt(0, pickzPress, LOW);
  digitalWrite(LED_WHI, HIGH);
  
  pickz_Correct = 0;
  digitalWrite(LED_GRE, LOW);
  delay(pickz_Delay);
  digitalWrite(LED_GRE, HIGH);  
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
    //EEPROM.write(pickz_levelHiAddr,pickz_Hiscore);
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
  lcd.print("Sletter");
  lcd.setCursor(0,1   );
  lcd.print("alle hi-scores...");
  //EEPROM.write(speedz_minHiAddr,99);
  //EEPROM.write(speedz_secHiAddr,99);
  //EEPROM.write(speedz_hunHiAddr,99);
  //EEPROM.write(pickz_levelHiAddr,0);
  if(DEBUG)Serial.write("Sletter hiscores");
  //if(DEBUG)Serial.write(EEPROM.read(speedz_minHiAddr));
  delay(4000);
}

// *******************************************
/*
  Turn off all LEDs
  Created by Mick Kirkegaard 2015.
*/ 
// *******************************************

void ledOff(void) {
  for(int i = 0 ; i < 5 ; i++)
    digitalWrite(ledChar[i], HIGH);
}

// *******************************************
/*
  Menu system for picking the Gamemode
  Created by Mick Kirkegaard 2015.
*/ 
// *******************************************

void menu() {
  int currentTime;
  int breakTime;
  int delayTime = 1500;
  lcd.setCursor(0,0);
  lcd.print("  V");
  lcd.setCursor(3,0);
  lcd.write(4);
  lcd.setCursor(4,0);
  lcd.print("LG ET SPIL  ");

  
  if(menu_chooseGame == 0) {
    lcd.setCursor(0,1);
    lcd.print("     SPEEDZ     ");
    currentTime = millis();
    breakTime = (currentTime + delayTime);
    while(currentTime < breakTime) {
      currentTime = millis();
      digitalWrite(LED_RED, LOW);
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
    digitalWrite(LED_RED, HIGH);
  }
  
  if(menu_chooseGame == 0) {
    lcd.setCursor(0,1);
    lcd.print("     MEMOZ     ");
    currentTime = millis();
    breakTime = (currentTime + delayTime);
    while(currentTime < breakTime) {
      currentTime = millis();
      digitalWrite(LED_GRE, LOW);
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
    digitalWrite(LED_GRE, HIGH);
  }
  if(menu_chooseGame == 0) {
    lcd.setCursor(0,1);
    lcd.print("     KOMMER..     ");
    currentTime = millis();
    breakTime = (currentTime + delayTime);
    while(currentTime < breakTime) {
      currentTime = millis();
      digitalWrite(LED_YEL, LOW);
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
    digitalWrite(LED_YEL, HIGH);
  }
  
  if(menu_chooseGame == 0) {
    lcd.setCursor(0,1);
    lcd.print("     PICKZ     ");
    currentTime = millis();
    breakTime = (currentTime + delayTime);
    while(currentTime < breakTime) {
      currentTime = millis();
      digitalWrite(LED_BLU, LOW);
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
    digitalWrite(LED_BLU, HIGH);
  }
}

