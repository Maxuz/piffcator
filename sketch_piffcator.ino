#include "TempSensor.h"
#include "Logger.h"
#include "Indicator.h"

#define STATE 1
#define MAIN_LOOP_DELAY 5

#define DEBUG_PORT_NUMBER 9600
#define RUN_BTN_PIN 7
#define GREEN_LED_PIN 4 // Индикатор стадии: выключен - программа не запущена. Горит - программа выполняется. Моргает - программа завершена.
#define TEMP_SENSOR_PIN 6
#define RELAY_SENSOR_PIN 5

#define TEMP_IND_RESET_PIN 11
#define TEMP_IND_FIRST_DIGIT_PIN 2
#define TEMP_IND_SECOND_DIGIT_PIN 3
#define TEMP_IND_PIN 12

#define ZERO_STAGE 0 // 0 состояние: плата включена, программа не запущена.
#define FIRST_STAGE 1 // 1 состояние: плата включена, программа запущена, выполняется первый этап
#define SECOND_STAGE 2 // 2 состояние: плата включена, программа запущена, выполняется второй этап
#define THIRD_STAGE 3 // 3 состояние: плата включена, программа запущена, выполняется третий этап
#define FOURTH_STAGE 4 // 4 состояние: плата включена, программа закончила выполнение
#define ERROR_STAGE 5 // Ошибка в выполнении

//#define SECOND_STAGE_START_TIME 3000000 // время начала второго этапа
#define SECOND_STAGE_START_TIME 61000 // время начала второго этапа
//#define THIRD_STAGE_START_TIME 6000000 // время начала третьего этапа
#define THIRD_STAGE_START_TIME 121000 // время начала третьего этапа
//#define FOURTH_STAGE_START_TIME 6600000 // время начала четвертого этапа
#define FOURTH_STAGE_START_TIME 151000 // время начала четвертого этапа

//#define FIRST_STAGE_WARM_UP_TEMP 60 // температура после которой включится реле для нагрева
//#define FIRST_STAGE_WARM_DOWN_TEMP 62 // температура после которой выключится реле для нагрева
#define FIRST_STAGE_WARM_UP_TEMP 30 // температура после которой включится реле для нагрева
#define FIRST_STAGE_WARM_DOWN_TEMP 32 // температура после которой выключится реле для нагрева

//#define SECOND_STAGE_WARM_UP_TEMP 70 // температура после которой включится реле для нагрева
//#define SECOND_STAGE_WARM_DOWN_TEMP 72 // температура после которой выключится реле для нагрева
#define SECOND_STAGE_WARM_UP_TEMP 35 // температура после которой включится реле для нагрева
#define SECOND_STAGE_WARM_DOWN_TEMP 37 // температура после которой выключится реле для нагрева

//#define THIRD_STAGE_WARM_UP_TEMP 76 // температура после которой включится реле для нагрева
//#define THIRD_STAGE_WARM_DOWN_TEMP 78 // температура после которой выключится реле для нагрева
#define THIRD_STAGE_WARM_UP_TEMP 40 // температура после которой включится реле для нагрева
#define THIRD_STAGE_WARM_DOWN_TEMP 42 // температура после которой выключится реле для нагрева


TemperatureSensor sensor(TEMP_SENSOR_PIN);
Logger logger(DEBUG_PORT_NUMBER);
Indicator indicator(TEMP_IND_PIN, TEMP_IND_RESET_PIN, TEMP_IND_FIRST_DIGIT_PIN, TEMP_IND_SECOND_DIGIT_PIN);

bool _bGreenLedStateOn = false;
bool _bRelayStateOn = false;
bool _bRunBtnWasUp = true;
unsigned _lStartTime = 0;

void setup() {
  Serial.begin(DEBUG_PORT_NUMBER);

  pinMode(RUN_BTN_PIN, INPUT_PULLUP);
  pinMode(GREEN_LED_PIN, OUTPUT);
  pinMode(TEMP_SENSOR_PIN, OUTPUT);
  pinMode(RELAY_SENSOR_PIN, OUTPUT);

  pinMode(TEMP_IND_FIRST_DIGIT_PIN, OUTPUT);
  pinMode(TEMP_IND_SECOND_DIGIT_PIN, OUTPUT);
  pinMode(TEMP_IND_PIN, OUTPUT);
  pinMode(TEMP_IND_RESET_PIN, OUTPUT);

  // Обнуляем счётчик при старте, чтобы он не оказался в случайном состоянии
  indicator.setValue(0);

  bool _bGreenLedStateOn = false;
  bool _bRelayStateOn = false;
  bool _bRunBtnWasUp = true;
  unsigned _lStartTime = 0;

  logger.off();
}

void loop() {
  if (STATE > 0) {
    float temperature = sensor.getTemperature();
    indicator.setValue(temperature);

    int currentStage = getCurrentStage();
    logger.logMsg("Current stage: ", currentStage);

    if (currentStage == ZERO_STAGE) {
      if (isRunBtnPressed()) {
        startFirstStage();
      }
    } else if (currentStage == FIRST_STAGE) {
      handleWarmingStage(temperature, FIRST_STAGE_WARM_UP_TEMP, FIRST_STAGE_WARM_DOWN_TEMP);
    } else if (currentStage == SECOND_STAGE) {
      handleWarmingStage(temperature, SECOND_STAGE_WARM_UP_TEMP, SECOND_STAGE_WARM_DOWN_TEMP);
    } else if (currentStage == THIRD_STAGE) {
      handleWarmingStage(temperature, THIRD_STAGE_WARM_UP_TEMP, THIRD_STAGE_WARM_DOWN_TEMP);
    } else if (currentStage == FOURTH_STAGE) {
      finishStage();
    } else {
      handleError();
    }
  }
  delay(MAIN_LOOP_DELAY);
}

int getCurrentStage()
{
  if (_lStartTime < 1) {
    return ZERO_STAGE;
  } else {
    unsigned long workingTime = millis() - _lStartTime;
    logger.logLongMsg("_lStartTime: ", _lStartTime);
    logger.logLongMsg("workingTime: ", workingTime);

    if (workingTime < SECOND_STAGE_START_TIME) {
      return FIRST_STAGE;
    } else if (workingTime < THIRD_STAGE_START_TIME) {
      return SECOND_STAGE;
    } else if (workingTime < FOURTH_STAGE_START_TIME) {
      return THIRD_STAGE;
    } else {
      return FOURTH_STAGE;
    }
  }
  return ERROR_STAGE;
}

bool isRunBtnPressed() {
  // определить момент «клика» несколько сложнее, чем факт того,
  // что кнопка сейчас просто нажата. Для определения клика мы
  // сначала понимаем, отпущена ли кнопка прямо сейчас...
  boolean buttonIsUp = digitalRead(RUN_BTN_PIN);

  // ...если «кнопка была отпущена и (&&) не отпущена сейчас»...
  if (_bRunBtnWasUp && !buttonIsUp) {
    // ...может это «клик», а может и ложный сигнал (дребезг),
    // возникающий в момент замыкания/размыкания пластин кнопки,
    // поэтому даём кнопке полностью «успокоиться»...
    delay(10);
    // ...и считываем сигнал снова
    buttonIsUp = digitalRead(RUN_BTN_PIN);
    if (!buttonIsUp) {  // если она всё ещё нажата...
      return true;
    }
  }

  // запоминаем последнее состояние кнопки для новой итерации
  _bRunBtnWasUp = buttonIsUp;
  return false;
}

void startFirstStage() {
  logger.logMsg("Start first stage");
  _lStartTime = millis();
  setGreenLedState(true);
}

void setGreenLedState(bool on) {
  logger.logMsg("Set green led on: ", on);
  _bGreenLedStateOn = on;
  digitalWrite(GREEN_LED_PIN, on ? HIGH : LOW);
}

void setRelayState(bool on)
{
  logger.logMsg("Set relay state on: ", on);
  if (on && _bRelayStateOn) {
    return;
  }
  _bRelayStateOn = on;
  digitalWrite(RELAY_SENSOR_PIN, on ? HIGH : LOW);
}

void handleWarmingStage(float currentTemp, int warmUpTemp, int warmDownTemp) {
  logger.logMsg("Handling warming stage");
  logger.logTemp(currentTemp);
  logger.logMsg("Warm up temp: ", warmUpTemp);
  logger.logMsg("Warm down temp: ", warmDownTemp);
  logger.logMsg("Relay state: ", _bRelayStateOn);

  if (!_bRelayStateOn && currentTemp < warmUpTemp) {
    setRelayState(true);
  } else if (_bRelayStateOn && currentTemp > warmDownTemp) {
    setRelayState(false);
  }
}

void finishStage() {
  logger.logMsg("Finished");
  setRelayState(false);
  setGreenLedState(true);
  delay(500);
  setGreenLedState(false);
  delay(200);
}

void handleError() {
  logger.logMsg("Error!!!");
  setRelayState(false);
  setGreenLedState(true);
  delay(100);
  setGreenLedState(false);
}
