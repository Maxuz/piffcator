#include "Indicator.h"

void Indicator::resetNumber()
{
  // Для сброса на мгновение ставим контакт
  // reset в HIGH и возвращаем обратно в LOW
  digitalWrite(_resetPin, HIGH);
  digitalWrite(_resetPin, LOW);
}

void Indicator::showNumber(int value)
{
  // Первым делом обнуляем текущее значение
  resetNumber();

  // Далее быстро «прокликиваем» счётчик до нужного значения
  int n;
  if (value < 1) {
    n = 0;
  } else if (value > 100) {
    n = 99;
  } else if (_curDigit == 1) {
    n = value / 10;
  } else {
    n = value % 10;
  }

  if (n == 0) {
    return;
  }
  while (n--) {
    digitalWrite(_clockPin, HIGH);
    digitalWrite(_clockPin, LOW);
  }
}

Indicator::Indicator(int clockPin, int resetPin, int firstDigitPin, int secondDigitPin) {
  _clockPin = clockPin;
  _resetPin = resetPin;
  _firstDigitPin = firstDigitPin;
  _secondDigitPin = secondDigitPin;
  _curDigit = 1;
}

void Indicator::setValue(int value)
{
  _value = value;
  switchDigit();
  showNumber(value);
}

void Indicator::switchDigit()
{
  if (_curDigit == 1) {
    _curDigit = 2;
    digitalWrite(_firstDigitPin, LOW);
    digitalWrite(_secondDigitPin, HIGH);
  } else {
    _curDigit = 1;
    digitalWrite(_firstDigitPin, HIGH);
    digitalWrite(_secondDigitPin, LOW);
  }
}
