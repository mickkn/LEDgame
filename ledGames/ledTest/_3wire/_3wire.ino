
#include <LiquidCrystal_SR_LCD3.h>
#include <Wire.h>

#define srdata  8
#define srclock 9
#define enable  7

LiquidCrystal_SR_LCD3 lcd(srdata,srclock,enable);

void setup() {
  lcd.begin(16,2);

}

void loop() {
  lcd.setCursor(0,0);
  lcd.print("test");
  

}
