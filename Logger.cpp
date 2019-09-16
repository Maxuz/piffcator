#include "Logger.h"

Logger::Logger(int debugPort) {
  _enabled = true;
}

void Logger::logTemp(float value)
{
  if (_enabled) {
    Serial.print("Temp C: ");
    Serial.println(value);
  }
}

void Logger::logMsg(String msg)
{
  if (_enabled) {
    Serial.println(msg);
  }
}

void Logger::logMsg(String msg, int value)
{
  if (_enabled) {
    Serial.print(msg);
    Serial.println(value);
  }
}

void Logger::logMsg(String msg, bool value)
{
  if (_enabled) {
    Serial.print(msg);
    Serial.println(value);
  }
}

void Logger::logLongMsg(String msg, unsigned long value)
{
  if (_enabled) {
    Serial.print(msg);
    Serial.println(value);
  }
}

void Logger::on()
{
  _enabled = true;
}

void Logger::off()
{
  _enabled = false;
}
