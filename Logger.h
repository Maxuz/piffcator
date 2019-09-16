#include <Arduino.h>

class Logger
{
  public:
    Logger(int debugPort);
    void logTemp(float value);
    void logMsg(String msg);
    void logMsg(String msg, int value);
    void logMsg(String msg, bool value);
    void logLongMsg(String msg, unsigned long value);
    void on();
    void off();
  private:
    bool _enabled;
};
