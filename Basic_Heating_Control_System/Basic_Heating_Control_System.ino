/*
 * Basic Heater Control System for upliance.ai
 * Author: D Yaswanth Kumar
 * Platform: ESP32
 */

#include <OneWire.h>
#include <DallasTemperature.h>

// Pin configurations
#define ONE_WIRE_BUS 4
#define HEATER_PIN   2
#define INDICATOR_PIN 5

// Thresholds
const float TARGET_TEMP   = 75.0;
const float HYST_LOWER    = 73.0;
const float HYST_UPPER    = 77.0;
const float OVERHEAT_TEMP = 90.0;

enum State {
  IDLE,
  HEATING,
  STABILIZING,
  TARGET_REACHED,
  OVERHEAT
};


// defining functions
void IRAM_ATTR onTimer();
void timer_setup(void);
float readTemperature();
void updateState(float temp);
void controlHeater();
void visualIndicator();
void logStatus(float temp);



State currentState = IDLE;
hw_timer_t *My_timer = NULL;
volatile int timerFlag = 0;


// Setup DS18B20
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);

void setup()
 {
  Serial.begin(115200);
  pinMode(HEATER_PIN, OUTPUT);
  pinMode(INDICATOR_PIN, OUTPUT);
  digitalWrite(HEATER_PIN, LOW);
  digitalWrite(INDICATOR_PIN, LOW);
  sensors.begin();
  currentState = IDLE;
  Serial.println("Heater Control System with DS18B20 starting...");
  timer_setup();
}

void loop()
{

  if (timerFlag)
  {
    float temp = readTemperature();

    if (temp != DEVICE_DISCONNECTED_C)
    {
      updateState(temp);
      controlHeater();
      visualIndicator();
      logStatus(temp);
    }
    else
      Serial.println("Error: Could not read temperature from DS18B20 sensor!");

    timerFlag = 0;
  }
  
}

float readTemperature()
{
  sensors.requestTemperatures();
  return sensors.getTempCByIndex(0);
}

void updateState(float temp)
{
  switch (currentState)
  {
    case IDLE:
      if (temp < TARGET_TEMP) currentState = HEATING;
      break;
    case HEATING:
      if (temp >= HYST_LOWER && temp <= HYST_UPPER)
        currentState = STABILIZING;
      else if (temp >= OVERHEAT_TEMP)
        currentState = OVERHEAT;
      break;
    case STABILIZING:
      if (temp >= HYST_LOWER && temp <= HYST_UPPER)
        currentState = TARGET_REACHED;
      else if (temp < HYST_LOWER)
        currentState = HEATING;
      else if (temp > OVERHEAT_TEMP)
        currentState = OVERHEAT;
      break;
    case TARGET_REACHED:
      if (temp > OVERHEAT_TEMP)
        currentState = OVERHEAT;
      else if (temp < HYST_LOWER)
        currentState = HEATING;
      break;
    case OVERHEAT:
      if (temp < TARGET_TEMP - 5) // cool down to safe range
        currentState = IDLE;
      break;
  }
}

void controlHeater()
{
  if (currentState == HEATING)
    digitalWrite(HEATER_PIN, HIGH);
  else
    digitalWrite(HEATER_PIN, LOW);
}

void visualIndicator()
{
  if (currentState == OVERHEAT)
    digitalWrite(INDICATOR_PIN, HIGH);
  else
    digitalWrite(INDICATOR_PIN, LOW);
}

void logStatus(float temp)
{
  const char* stateNames[] = { "IDLE", "HEATING", "STABILIZING", "TARGET_REACHED", "OVERHEAT" };
  Serial.print("Temp: ");
  Serial.print(temp);
  Serial.print("°C | State: ");
  Serial.print(stateNames[currentState]);
  Serial.print(" | Heater: ");
  Serial.println(digitalRead(HEATER_PIN) ? "ON" : "OFF");
}



void timer_setup(void)
{
  My_timer = timerBegin(1000000);
  timerAttachInterrupt(My_timer, &onTimer);
  timerAlarm(My_timer, 1000000, true, 0);
}

void IRAM_ATTR onTimer()
{
  timerFlag = 1;
}