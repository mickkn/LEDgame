#include "LedHC138.h"
#include <arduino.h>

LedHC138::LedHC138(const int A0, const int A1, const int A2) {
  _A0 = A0;
  _A1 = A1;
  _A2 = A2;
}

void LedHC138::on(int LED) {
  if (LED == 1) {
    digitalWrite(_A0, LOW);
    digitalWrite(_A1, LOW);
    digitalWrite(_A2, LOW);
  }
  else if (LED == 2) {
    digitalWrite(_A0, HIGH);
    digitalWrite(_A1, LOW);
    digitalWrite(_A2, LOW);
  }
  else if (LED == 3) {
    digitalWrite(_A0, LOW);
    digitalWrite(_A1, HIGH);
    digitalWrite(_A2, LOW);
  }
  else if (LED == 4) {
    digitalWrite(_A0, HIGH);
    digitalWrite(_A1, HIGH);
    digitalWrite(_A2, LOW);
  }
  else if (LED == 5) {
    digitalWrite(_A0, LOW);
    digitalWrite(_A1, LOW);
    digitalWrite(_A2, HIGH);
  }
  else if (LED == 6) {
    digitalWrite(_A0, HIGH);
    digitalWrite(_A1, LOW);
    digitalWrite(_A2, HIGH);
  }
  else if (LED == 7) {
    digitalWrite(_A0, LOW);
    digitalWrite(_A1, HIGH);
    digitalWrite(_A2, HIGH);
  }
  else {
    // OFF FUNCTION THEREFORE ONLY 7 LEDS
    digitalWrite(_A0, HIGH);
    digitalWrite(_A1, HIGH);
    digitalWrite(_A2, HIGH);
  }
}
