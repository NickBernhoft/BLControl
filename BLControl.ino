// hardware notes:
// 7 rotations per revolution
// 6 * 7 = 21 steps per rotation
// 360 / 42 = 8.5714 degrees per step = 0.1496 radians per step

// testing notes:
// max achieved rpm using simple ramping: 3571rpm
// max achieved left-right without losing steps: 5ms step time, 50ms rest between time
// without rapming RPM the max instant start speed is about 600 rpm



// TODO
// make the rate for ramp() customisable

/*
------------------------------------------------OLD CONTROL CODE--------------------------------------------------------------------------------------

#include "music.h"
#include "control.h"
#include "demos.h"


void setup()
{
  Serial.begin(115200);

  // right side motors
  pinMode(13, OUTPUT);  //LED
  pinMode(12, OUTPUT);
  pinMode(11, OUTPUT);
  pinMode(10, OUTPUT);
  pinMode(9, OUTPUT);
  pinMode(8, OUTPUT);
  pinMode(7, OUTPUT);

  // left side motors
  // setting the analog out pins as digital out
  pinMode(A0, OUTPUT);
  pinMode(A1, OUTPUT);
  pinMode(A2, OUTPUT);
  pinMode(A3, OUTPUT);
  pinMode(A4, OUTPUT);
  pinMode(A5, OUTPUT);
}


byte incoming_byte = 255;
int speed = 0;



void loop()
{  
  
  incoming_byte = Serial.read();
  //updateBank(0, 200, REV);
  updateBank(1, 200, REV);

  if(incoming_byte == 255)
  {
    runAll();
  }
  else 
  {
    // control logic here
    Serial.println(incoming_byte);
  }

}


-----------------------------------------------OLD CONTROL CODE-------------------------------------------------------------
*/

#include <SimpleFOC.h>

#define POLE_PAIRS 7

BLDCMotor motor(POLE_PAIRS);
BLDCDriver3PWM driver(9, 10, 11, 8);

float target_angle = 0.0;
float step_size = 0.25;   // radians per step

void setup() {
  Serial.begin(115200);

  pinMode(8, OUTPUT);
  digitalWrite(8, HIGH);

  driver.voltage_power_supply = 12;
  driver.voltage_limit = 8;
  driver.pwm_frequency = 20000;
  driver.init();

  motor.linkDriver(&driver);

  // stepper like mode
  motor.controller = MotionControlType::angle_openloop;

  motor.voltage_limit = 8;

  motor.init();
  motor.enable();

  Serial.println("Motor ready");
}

void loop() {

  motor.loopFOC();
  motor.move(target_angle);

  // step every second
  static unsigned long last_step = 0;

  if (millis() - last_step > 100) {
    target_angle += step_size;
    last_step = millis();

    Serial.print("Target angle: ");
    Serial.println(target_angle);
  }
}