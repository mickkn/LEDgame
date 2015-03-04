
#include <LiquidCrystal_SR_LCD3.h>
#include <Wire.h>

#define srdata  10
#define srclock 11
#define enable  12

LiquidCrystal_SR_LCD3 lcd(srdata,srclock,enable);

void setup() {
  lcd.begin(16,2);

}

void loop() {
  lcd.setCursor(0,0);
  lcd.print("test");
  

}
