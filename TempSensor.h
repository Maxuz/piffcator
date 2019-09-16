// библиотека для работы с протоколом 1-Wire
#include <OneWire.h>
// библиотека для работы с датчиком DS18B20
#include <DallasTemperature.h>

#define MEASURE_DELTA_TIME_MS 1000

class TemperatureSensor
{
  public:
    TemperatureSensor(int pin);
    ~TemperatureSensor();
    float getTemperature();
  private:
    DallasTemperature *sensor;
    float _temperature;
    unsigned long _lastMeasureTime;
};
