#include <SPI.h>
#include <Wire.h>
#include "Adafruit_MAX31855.h"
#include <Adafruit_SH1106.h>
#include <PID_v1.h>
#include "MCP4725.h"

#define COOLER_PIN 31
MCP4725 MCP1(0x60);

float Kp = 2.5;
float Ki = 0.5;
float Kd = 0.02;

float pidMinValue = 0;    // Minimum value of PID output
float pidMaxValue = 100;  // Maximum value that concider for PID output
int coolOrHeatPowerMin = 0;  // Minimum output voltage
int coolOrHeatPowerMax = 5.1;  //maximum output voltage

const float expVal = 2.0;

#define MAX_CS 23
#define set_temp 25.00

#define OLED_Width 128
#define OLED_Height 64
#define OLED_Reset -1
#define SCREEN_ADDRESS 0x3C

Adafruit_MAX31855 sensor1(MAX_CS);
Adafruit_SH1106 display(MAX_CS);

double Set;
double temp;
double OutErr;

PID PID1(&temp, &OutErr, &Set, Kp, Ki, Kd, DIRECT);

float temp_p;
const int sampleRate = 1;

bool coolOrHeat;
int coolOrHeatPower;

void setup() {
  Serial.begin(9600);

  PID1.SetMode(AUTOMATIC);
  PID1.SetSampleTime(sampleRate);
  //display.begin(SH1106_SWITCHCAPVCC, SCREEN_ADDRESS);
  //display.clearDisplay();
  Set = set_temp;
  Wire.begin();
  MCP1.begin();
  MCP1.setMaxVoltage(5.1);
}

void loop() {
  // Read the potentiometer value to dynamically adjust set temperature
  int potValue = analogRead(A0);
  Set = map(potValue, 0, 1023, 20, 30); // Map potentiometer value to a temperature range

  temp = sensor1.readCelsius();

  if (sensor1.readError()) {
    Serial.println("Sensor 1 reading error");
  } else {
    PID1.Compute();
    Serial.print("Temp reading done!  ");
    Serial.print("Sensor 1: ");
    Serial.print(temp);
    Serial.print(" C");
    Serial.print(" | ");
    Serial.print("PID Output: ");
    Serial.print(OutErr);
    tempControl();
  }
  delay(250);
}

void tempControl() {
  float floatOutErr = static_cast<float>(OutErr);
  float absOutErr = fabs(floatOutErr);

  coolOrHeat = (temp > Set) ? 0 : 1;
  coolOrHeatPower = map(absOutErr, pidMinValue, pidMaxValue, coolOrHeatPowerMin, coolOrHeatPowerMax);

  MCP1.setVoltage(coolOrHeatPower);
  Serial.print(" | Output Voltage: ");
  Serial.println(coolOrHeatPower);
}