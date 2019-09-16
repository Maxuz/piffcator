#include "TempSensor.h"

TemperatureSensor::TemperatureSensor(int pin)
{
  OneWire *oneWire = new OneWire(pin);
  sensor = new DallasTemperature(oneWire);
  sensor->begin();
  sensor->setResolution(12);
  _lastMeasureTime = millis();

  sensor->requestTemperatures();
  _temperature = sensor->getTempCByIndex(0);
}

TemperatureSensor::~TemperatureSensor()
{
  //  delete oneWire;
  delete sensor;
}

float TemperatureSensor::getTemperature()
{
  if ((millis() - _lastMeasureTime) > MEASURE_DELTA_TIME_MS) {
    sensor->requestTemperatures();
    float temperature = sensor->getTempCByIndex(0);
    if (temperature > 0) {
      _temperature = temperature;
      _lastMeasureTime = millis();
    }
  }

  return _temperature;
}
