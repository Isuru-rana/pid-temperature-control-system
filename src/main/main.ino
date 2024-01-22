/* ---------- Temperature Control System v1.4.0 ------------

v1.1 - 01/20/24 - Changed the serial response value from float to int.
                  Fixed a bug in the PID.

v1.1.1 - 01/20/24 - Added Power Off state to the system.
                    Cleared all temperature setpoints.
                    Turned off all I/O pins.
                    Removed setpoint values from EEPROM.
                    Removed Heater Cooler pin.

v1.1.2 - 01/20/24 - Corrected serial response for heater state errors.
                    Changed heater temperature to int and added +200.
                    Added a 5-second timer for the heater.

v1.1.3 - 01/21/24 - Fixed a bug related to EEPROM.

v1.1.5            - Fixed a bug.

v1.2.0 + 1.2.1    - Changed the serial communication protocol.
                      Fixed bugs in serial and PID loops.

v1.2.2            - Improved code (Display).

v1.3.0 - 01/22/24 - Changed serial communication protocol.
                      Added "L" parameter to switch servopin 0, 1, 2.

v1.3.1            - Added another pin for the "L" parameter.
                    Added J2 pin.
                    Changed "P00" to display as "OFF."

v1.4.0            - Changed serial communication protocol.
                      Added "F" parameter to set voltages.
                      Added "D" parameter for setting DC voltages.
                      Minor bug fixes.
*/




#include <SPI.h>                //SPI Protocol for MAX31855 connection
#include <Wire.h>               // I2C protocol for OLED connection
#include "Adafruit_MAX31855.h"  //For MAX31855 connection
#include <Adafruit_SH1106.h>
#include <PID_v1.h>
#include <EEPROM.h>
#include "MCP4725.h"


int device_address = 5;
#define numControlUnits 4


//===== Pins=======
#define TIMER_ON_LED_PIN 27  //N  //  Fan pin will change it later
#define POWER_ON_PIN 28      //  /5J001R
#define HEATER_Heat_PIN 29   //  /J101 Heater Power on pin  /5J101R
#define HEATER_PWM_PIN 2
#define COOLER_PIN 31  // note: Cooler is controlled by MPC1 (0x60)
#define PELTIER_PIN 32
#define AMBIENT_PIN 33

int MAX_CS[numControlUnits] = { 22, 23, 24, 25 };  // MAX31855 module pins

#define heaterTimeout 5000
#define serialTimeout 2000

#define J2COMMAND_PIN 10
#define servopinno0 6
#define servopinno1 7
#define servopinno2 8
#define servopinno3 9
int servopinno[4] = { servopinno0, servopinno1, servopinno2, servopinno3 };


#define TX_ON_PIN 49
#define ANALOG_PIN A0

// ====== System Flags =======
bool systemPower = false;
bool serialConState = false;
bool coolerPower = true;
bool heaterPower = 0;

bool heatSafeT = 0;

bool peltierPower = false;
bool ambientPower = false;

bool J2 = false;
float valveVoltage = 0;
float dcMotorVoltage = 0;

//===OLED Display setup====
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1
#define SCREEN_ADDRESS 0x3C
int displayVPos[numControlUnits] = { 10, 25, 40, 55 };


int eepromAddress[9] = { 0, 1, 2, 3, 4, 5, 6, 7, 8 };
bool pinArray[16][4] = {
  { 0, 0, 0, 0 },
  { 0, 0, 0, 1 },
  { 0, 0, 1, 0 },
  { 0, 0, 1, 1 },
  { 0, 1, 0, 0 },
  { 0, 1, 0, 1 },
  { 0, 1, 1, 0 },
  { 0, 1, 1, 1 },
  { 1, 0, 0, 0 },
  { 1, 0, 0, 1 },
  { 1, 0, 1, 0 },
  { 1, 0, 1, 1 },
  { 1, 1, 0, 0 },
  { 1, 1, 0, 1 },
  { 1, 1, 1, 0 },
  { 1, 1, 1, 1 }
};
int tempArrayInt[numControlUnits];
float tempArray_p[numControlUnits];
float setTempArray[4];
int setTempArrayInt[4];


char Lbuff[4];
int jValue = 1;
unsigned long heaterTimer;
unsigned long serialTimer;

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

MCP4725 MCP1(0x60);  //ADDR open
MCP4725 MCP2(0x61);  //ADDR to GND
MCP4725 MCP3(0x64);  //Variable voltage out
MCP4725 MCP4(0x65);  //DC moto controller
float mcpMaxVoltage[4] = { 5.1, 5.1, 5.1, 5.1 };

uint8_t selectPin[4] = { 50, 51, 52, 53 };

void ReadEEPROM();
void parseCommand(String command);
void serialSync();



void setup() {
  Serial.begin(9600);
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
  MCP4.begin();
  MCP1.setMaxVoltage(mcpMaxVoltage[0]);
  MCP2.setMaxVoltage(mcpMaxVoltage[1]);
  MCP3.setMaxVoltage(mcpMaxVoltage[2]);
  MCP4.setMaxVoltage(mcpMaxVoltage[3]);

  pinMode(POWER_ON_PIN, OUTPUT);
  pinMode(HEATER_PWM_PIN, OUTPUT);
  pinMode(HEATER_Heat_PIN, OUTPUT);
  pinMode(COOLER_PIN, OUTPUT);
  pinMode(PELTIER_PIN, OUTPUT);
  pinMode(AMBIENT_PIN, OUTPUT);
  pinMode(servopinno0, OUTPUT);
  pinMode(servopinno1, OUTPUT);
  pinMode(servopinno2, OUTPUT);
  pinMode(J2COMMAND_PIN, OUTPUT);
}

bool senseError[numControlUnits];

void loop() {
  if ((millis() - heaterTimer > heaterTimeout) && heatSafeT) {
    heaterTimer = 0;
    heaterPower = 0;
  } else if (heatSafeT) heaterPower = 1;

  //if ((millis() - serialTimer > serialTimeout) && systemPower) serialSync();

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
    Set[i] = setTempArray[i];
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
  //temp[0] += 200;
  //tempArrayInt[0] += 200;
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
    coolOrHeat[i] = (temp[i] > Set[i]) ? 0 : 1;
    coolOrHeatPower[i] = map(absOutErr[i], pidMinValue[i], pidMaxValue[i], coolOrHeatPowerMin[i], coolOrHeatPowerMax[i]);

    if (systemPower) {
      if (heaterPower) {
        if (coolOrHeat[0] == 1) {
          digitalWrite(HEATER_Heat_PIN, HIGH);
          analogWrite(HEATER_PWM_PIN, coolOrHeatPower[0]);
        } else {
          digitalWrite(HEATER_Heat_PIN, LOW);
          analogWrite(HEATER_PWM_PIN, 0);
        }
      } else {
        analogWrite(HEATER_PWM_PIN, 0);
        digitalWrite(HEATER_Heat_PIN, LOW);
      }

      if (coolerPower) {
        if (coolOrHeat[1] == 0) {
          digitalWrite(COOLER_PIN, HIGH);
        } else {
          digitalWrite(COOLER_PIN, LOW);
        }
        MCP1.setVoltage(coolOrHeatPower[1]);

      } else {
        MCP1.setVoltage(0);
        digitalWrite(COOLER_PIN, LOW);
      }

      if (peltierPower) {
        MCP2.setVoltage(coolOrHeatPower[2]);
        if (coolOrHeat[2] == 0) {
          digitalWrite(PELTIER_PIN, HIGH);
        } else {
          digitalWrite(PELTIER_PIN, LOW);
        }
      } else {
        MCP2.setVoltage(0);
        digitalWrite(PELTIER_PIN, LOW);
      }

      if (ambientPower) {
        //MCP3.setVoltage(coolOrHeatPower[3]);
        if (coolOrHeat[3] == 1) {
          digitalWrite(AMBIENT_PIN, HIGH);
        } else {
          digitalWrite(AMBIENT_PIN, LOW);
        }
      } else {
        digitalWrite(AMBIENT_PIN, LOW);
      }
    } else {
      digitalWrite(POWER_ON_PIN, systemPower);
      digitalWrite(HEATER_PWM_PIN, LOW);
      digitalWrite(HEATER_Heat_PIN, LOW);
      digitalWrite(COOLER_PIN, LOW);
      digitalWrite(PELTIER_PIN, LOW);
      digitalWrite(AMBIENT_PIN, LOW);
      MCP1.setVoltage(0);
      MCP2.setVoltage(0);
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
  systemPower = false;
  digitalWrite(POWER_ON_PIN, systemPower);
  heaterPower = 0;
  for (int i = 0; i < 4; i++) {
    setTempArray[i] = 0;
    setTempArrayInt[i] = 0;
  }
  displayWriteData(0);
}
float valveVoltage = 0;
float dcMotorVoltage = 0;

void setValveVoltage(int device) {
  if (device == 1) MCP3.setVoltage(valveVoltage);
  else if (device == 2) MCP3.setVoltage(dcMotorVoltage);
}