/* ---------- Temperature control system v1.1.1 ----------

v1.1 - 01/20/24 - Serial response value changed from float to int
                  Bug fixed in PID
v1.1.1 - 01/20/24 - Added Timer Interrupt for keep sync with serial connection
                    Added Power Off state to the system.
                        All the temperature setpoints cleard
                        all I/O pins are turned off
                    Set point values are removed form EEPROM
                    Heater Cooler pin removed

*/


#include <SPI.h>                //SPI Protocol for MAX31855 connection
#include <Wire.h>               // I2C protocol for OLED connection
#include "Adafruit_MAX31855.h"  //For MAX31855 connection
#include <Adafruit_SH1106.h>
#include <PID_v1.h>
#include <EEPROM.h>
#include "MCP4725.h"
#include <TimerOne.h>

int device_address = 0;

#define numControlUnits 4

//===== Pins=======
#define TIMER_ON_LED_PIN 27  //N  //  Fan pin will change it later
#define POWER_ON_PIN 28      //  /5J001R
#define HEATER_Heat_PIN 29   //  /J101 Heater Power on pin  /5J101R
#define HEATER_PWM_PIN 2
#define COOLER_PIN 31
#define PELTIER_PIN 32
#define AMBIENT_PIN 33


#define servopinno0 6  //N
#define servopinno1 7  //N
#define servopinno2 8  //N
#define servopinno3 9  //N

#define TX_ON_PIN 49
#define ANALOG_PIN A0

//#define HEATER_Cool_PIN 30
//#define AMBIENT_PWM_PIN 5  //N
//#define COOLER_PWM_PIN 3  //N
//#define PELTIER_PWM_PIN 4  //N

// ====== System Flags =======
bool systemPower = false;
bool coolerPower = true;

//===OLED Display setup====
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1
#define SCREEN_ADDRESS 0x3C
int displayVPos[numControlUnits] = { 10, 25, 40, 55 };

int MAX_CS[numControlUnits] = { 22, 23, 24, 25 };

int eepromAddress[9] = { 0, 1, 2, 3, 4, 5, 6, 7, 8 };

int tempArrayInt[numControlUnits];
//float tempArrayInt[numControlUnits];
float tempArray_p[numControlUnits];
float setTempArray[4];
int setTempArrayInt[4];
bool heaterState = 0;
bool heatSFT = 0;

int jValue = 1;

//==== PID config ======

float pidMinValue[numControlUnits] = { 0, 0, 0, 0 };
float pidMaxValue[numControlUnits] = { 255, 255, 255, 255 };

double Set[numControlUnits];
double temp[numControlUnits];
double OutErr[numControlUnits];

float Kp[numControlUnits] = { 1.5, 1.5, 1.5, 1.5 };      //Initial Proportional Gain
float Ki[numControlUnits] = { 0.8, 0.8, 0.8, 0.8 };      //Initial Integral Gain
float Kd[numControlUnits] = { 0.02, 0.02, 0.02, 0.02 };  //Initial Differential Gain

// == controllers Initialization ===
PID *pidControl[numControlUnits];
Adafruit_MAX31855 *sensor[numControlUnits];
Adafruit_SH1106 display(OLED_RESET);
void initControles();

MCP4725 MCP1(0x62);  //ADDR open
MCP4725 MCP2(0x63);  //ADDR to Vcc
MCP4725 MCP3(0x60);  //ADDR to GND

uint8_t selectPin[4] = { 50, 51, 52, 53 };

void ReadEEPROM();
void parseCommand(String command);
void serialSync();

float mcpMaxVoltage[3] = { 5.1, 5.1, 5.1 };

void setup() {
  Serial.begin(9600);
  Timer1.initialize(2000000);
  Timer1.attachInterrupt(serialSync);
  Timer1.stop();
  initControles();
  display.begin(SH1106_SWITCHCAPVCC, SCREEN_ADDRESS);
  display.clearDisplay();
  displayInit();
  ReadEEPROM();
  displayWriteData(0);

  const int sampleRate = 1;

  for (int i = 0; i < numControlUnits; i++) {
    pidControl[i]->SetMode(AUTOMATIC);
    pidControl[i]->SetSampleTime(sampleRate);
    Set[i] = setTempArray[i];
  }

  Wire.begin();
  MCP1.begin();
  MCP2.begin();
  MCP3.begin();
  MCP1.setMaxVoltage(mcpMaxVoltage[0]);
  MCP2.setMaxVoltage(mcpMaxVoltage[1]);
  MCP3.setMaxVoltage(mcpMaxVoltage[2]);

  pinMode(POWER_ON_PIN, OUTPUT);
  pinMode(HEATER_PWM_PIN, OUTPUT);
  pinMode(HEATER_Heat_PIN, OUTPUT);
  pinMode(COOLER_PIN, OUTPUT);
  pinMode(PELTIER_PIN, OUTPUT);
  pinMode(AMBIENT_PIN, OUTPUT);
}

bool senseError[numControlUnits];



void loop() {

  static String receivedCommand = "";

  while (Serial.available() > 0) {
    char c = Serial.read();
    if (c == '\r' || c == '\n') {
      parseCommand(receivedCommand);
      receivedCommand = "";
    } else {
      receivedCommand += c;
    }
  }

  for (int i = 0; i < numControlUnits; i++) {
    temp[i] = sensor[i]->readCelsius();
    tempArrayInt[i] = int(temp[i]);
  }


  for (int i = 0; i < numControlUnits; i++) {
    if (sensor[i]->readError()) {
      senseError[i] = true;
    } else {
      senseError[i] = false;
      pidControl[i]->Compute();
    }

    if (!senseError[i]) {
      if (temp[i] != tempArray_p[i]) {
        tempArray_p[i] = temp[i];
      }
    }
  }
  displayWriteData(1);
  tempControl();
  delay(250);
}

float floatOutErr[numControlUnits];
float absOutErr[numControlUnits];
bool coolOrHeat[numControlUnits];  // 1 == cool; 0 == heat
int coolOrHeatPower[numControlUnits];
float coolOrHeatPowerMin[numControlUnits] = { 0, 0, 0, 0 };
float coolOrHeatPowerMax[numControlUnits] = { 255, mcpMaxVoltage[0], mcpMaxVoltage[1], mcpMaxVoltage[2] };


void tempControl() {
  for (int i = 0; i < numControlUnits; i++) {
    floatOutErr[i] = static_cast<float>(OutErr[i]);
    absOutErr[i] = fabs(floatOutErr[i]);
    coolOrHeat[i] = (temp[i] > Set[i]) ? 1 : 0;
    coolOrHeatPower[i] = map(absOutErr[i], pidMinValue[i], pidMaxValue[i], coolOrHeatPowerMin[i], coolOrHeatPowerMax[i]);

    if (systemPower) {
      if (heaterState && coolOrHeat[0] == 1) {
        digitalWrite(HEATER_Heat_PIN, HIGH);
      } else {
        digitalWrite(HEATER_Heat_PIN, LOW);
      }
      if (heaterState) analogWrite(HEATER_PWM_PIN, coolOrHeatPower[0]);
      else analogWrite(HEATER_PWM_PIN, 0);

      if (coolerPower) {
        if (coolOrHeat[1] == 1) {
          digitalWrite(COOLER_PIN, HIGH);
        } else {
          digitalWrite(COOLER_PIN, LOW);
        }
        MCP1.setVoltage(coolOrHeatPower[1]);
      } else {
        MCP1.setVoltage(0);
        digitalWrite(COOLER_PIN, LOW);
      }

      MCP2.setVoltage(coolOrHeatPower[2]);
      if (coolOrHeat[2] == 1) {
        digitalWrite(PELTIER_PIN, HIGH);
      } else {
        digitalWrite(PELTIER_PIN, LOW);
      }

      MCP3.setVoltage(coolOrHeatPower[3]);
      if (coolOrHeat[2] == 1) {
        digitalWrite(AMBIENT_PIN, HIGH);
      } else {
        digitalWrite(AMBIENT_PIN, LOW);
      }
    } else {
      digitalWrite(POWER_ON_PIN, LOW);
      digitalWrite(HEATER_PWM_PIN, LOW);
      digitalWrite(HEATER_Heat_PIN, LOW);
      digitalWrite(COOLER_PIN, LOW);
      digitalWrite(PELTIER_PIN, LOW);
      digitalWrite(AMBIENT_PIN, LOW);
    }
  }
}

void initControles() {
  for (int i = 0; i < numControlUnits; i++) {
    pidControl[i] = new PID(&temp[i], &OutErr[i], &Set[i], Kp[i], Ki[i], Kd[i], DIRECT);
    sensor[i] = new Adafruit_MAX31855(MAX_CS[i]);
  }
}

void serialSync() {
  systemPower == false;
  for (int i = 0; i < 4; i++) {
    setTempArray[i] = 0;
    setTempArrayInt[i] = 0;
  }
}