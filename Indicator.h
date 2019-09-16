#include <Arduino.h>

class Indicator
{
  public:
    Indicator(int clockPin, int resetPin, int firstDigitPin, int secondDigitPin);
    void setValue(int value);
  private:
    int _clockPin;
    int _resetPin;
    int _firstDigitPin;
    int _secondDigitPin;
    int _value;
    int _curDigit;
    void resetNumber();
    void showNumber(int n);
    void switchDigit();
};
