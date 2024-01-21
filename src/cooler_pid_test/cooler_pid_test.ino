#include <SPI.h>                //SPI Protocol for MAX31855 connection
#include <Wire.h>               // I2C protocol for OLED connection
#include "Adafruit_MAX31855.h"  //For MAX31855 connection
#include <Adafruit_SH1106.h>
#include <PID_v1.h>

// ------ SPI ---------
#define tempSense 3  // CS pin of MAX31855 module 1 on SPI protocol

// ------ Pin assignment ------------
#define Cool1_PWM 15  //PWM pin for peltier control peltier cooling
#define Cool1_C 18    // Peltier cooling select
#define Cool1_H 21    // Peltier Heating select

//------ Details of the system ------
#define set_temp 25.00  // set temperatue of the system

//====== For OLED config ============
#define OLED_Width 128
#define OLED_Height 64
#define OLED_Reset -1
#define SCREEN_ADDRESS 0x3D

//---- PID LOOP variables for PID_v1-------
double Set;
double temp;
double OutErr;

float Kp = 2.5;   //Initial Proportional Gain
float Ki = 0.5;   //Initial Integral Gain
float Kd = 0.02;  //Initial Differential Gain

PID PID1(&temp, &OutErr, &Set, Kp, Ki, Kd, DIRECT);
Adafruit_MAX31855 sensor1(tempSense);
Adafruit_SH1106 display(OLED_Reset, SCREEN_ADDRESS);  //Initialize OLED

//------Global Variables--------

float temp_p;        //Used to store previous reading
int speed = 255;     //Peltier power intencity (default set to 255)
bool direction = 1;  //Is heating or cooling (1=cooling, 0=heating)

const float expVal = 2.0;  //This value is used to get visible power changes
                           //in peltier, rather than using linier power chan

const int sampleRate = 1;  // Variable that determines how fast our PID loop runs


void setup() {

  pinMode(Cool1_PWM, OUTPUT);
  pinMode(Cool1_C, OUTPUT);
  pinMode(Cool1_H, OUTPUT);

  PID1.SetMode(AUTOMATIC);  //Set PID to automatic mode
  PID1.SetSampleTime(sampleRate);

  Set = set_temp;  //get the set temperature form the define

  if ((!display.begin(SSD1306_SWITCHCAPVCC, 0x3C))) {
    Serial.println("Display starting Failed!");
  } else {
    display.clearDisplay();
    displayInt();
  }

  delay(250);
}


void loop() {
  temp = sensor1.readCelsius();

  if (sensor1.readError()) {
    Serial.println("Sensor 1 reading error");
  } else {
    feedDisplay(1, temp);
    PID1.Compute();
    Serial.println("Temp reading done!");
    Serial.printf("Sensor 1: %f\n", temp);
    Serial.printf("Sensor 1 PID: %f\n", OutErr);
    Serial.printf("\n");
    tempControl();
  }
}

float floatOutErr;  //Used for internal work on the code
float absOutErr;    //Used for internal work on the code

void tempControl() {

  floatOutErr = static_cast<float>(OutErr);  // convert double to float
  absOutErr = fabs(floatOutErr);             // get the absolute value to predict power

  if (temp > Set) {
    direction = 1;  // if temperature is higher than set, cooling active
  } else {
    direction = 0;  // if temperature is higher than set, heating active
  }
  setDirection();
  speed = map(absOutErr, 0, MAX_Out, 0, 255);  //Linear relationship between output and input
  //speed[i] = map(absOutErr[i],0,pow(MAX_Out, exponent),0,255); //Exponential relationship between output and input
  analogWrite(Cool1_PWM, speed);               //write power to peltier


  Serial.println("direction and power");
  Serial.printf("Power: %d  direction: %d\n", speed, direction);
  Serial.println("================");
}

void setDirection() {
  if (direction]) {
    digitalWrite(Cool1_C, HIGH);
    digitalWrite(Cool1_H, LOW);
  } else {
    digitalWrite(Cool1_C, LOW);
    digitalWrite(Cool1_H, HIGH);
  }
}
