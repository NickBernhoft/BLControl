#include <SimpleFOC.h>
#include "controlfoc.h"

#define POLE_PAIRS 7

BLDCMotor motor(POLE_PAIRS);
BLDCDriver3PWM driver(9, 10, 11, 8);

float target_angle = 0.0;
float step_size = 0.25;   // radians per step
float target_velocity[2] = {0.0}; // radians per second

byte incoming_byte = 255;
int rover_speed[2] = {0}; // positave for clockwise

void setup() {
  Serial.begin(115200);

  pinMode(8, OUTPUT);
  digitalWrite(8, HIGH);

  driver.voltage_power_supply = SUPPLY_VOLTAGE;
  driver.voltage_limit = 8;
  driver.pwm_frequency = 20000;
  driver.init();

  motor.linkDriver(&driver);


  motor.controller = MotionControlType::velocity_openloop;

  motor.voltage_limit = SUPPLY_VOLTAGE;

  motor.init();
  motor.enable();

  Serial.println("Motor ready");
}

void loop() {

  
  for(int i = 0; i < LOOP_DUTY_CYCLE; i++)
  {
    motor.loopFOC();
    motor.move(target_velocity[0]);

    incoming_byte = Serial.read();

    // movment logic
    switch(incoming_byte)
    {
      case 's':
      case ROVER_STOP:
        rover_speed[0] = 0;
        rover_speed[1] = 0;
        Serial.println("ROVER_STOP");
        break;

      case 'f':
      case ROVER_FWD:
        rover_speed[0]++;
        rover_speed[1]++;
        Serial.println("ROVER_FWD");
        break;

      case 'r':
      case ROVER_REV:
        rover_speed[0]--;
        rover_speed[1]--;
        Serial.println("ROVER_REV");
        break;
      
      case 'l':
      case ROVER_LEFT:
        rover_speed[0]--;
        rover_speed[1]++;
        Serial.println("ROVER_LEFT");
        break;

      case 'k': // k bc its next to l
      case ROVER_RIGHT:
        rover_speed[0]++;
        rover_speed[1]--;
        Serial.println("ROVER_RIGHT");
        break;
    }
    
  }

  

  // add some dynamic voltage code?
  // holding gets reduced voltage, higher speeds get higher voltage?

  // note: using constrain() is super slow for some reason.
  for(int i = 0; i < NUM_BANKS; i++)
  {
    rover_speed[i] = clamp(rover_speed[i], NUM_SPEEDS * -1, NUM_SPEEDS);
    target_velocity[i] = RPMtoRads(333 * rover_speed[0]);
  }

  // super basic dynamic voltage
  driver.voltage_limit = 6 + (2 * abs(rover_speed[0]));

  // debug code
  Serial.print("Rover Speeds: ");
  Serial.print(rover_speed[0]);
  Serial.print(", ");
  Serial.println(rover_speed[1]);
}