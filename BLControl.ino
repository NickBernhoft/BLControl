#include <SimpleFOC.h>
#include "controlfoc.h"

#define POLE_PAIRS 7

BLDCMotor motor(POLE_PAIRS);
BLDCDriver3PWM driver(9, 10, 11, 8);

float target_angle = 0.0;
float step_size = 0.25;   // radians per step
float target_velocity[2] = {0.0}; // radians per second (actual, ramped)
float desired_velocity[2] = {0.0}; // radians per second (commanded)

#define RAMP_RATE 0.005 // rad/s per FOC loop iteration (runs LOOP_DUTY_CYCLE times per outer loop)

byte incoming_byte = 255;s
int rover_speed[2] = {0}; // positive for clockwise

void setup() {
  Serial.begin(115200);

  pinMode(8, OUTPUT);
  digitalWrite(8, HIGH);

  driver.voltage_power_supply = SUPPLY_VOLTAGE;
  driver.voltage_limit = 8;
  driver.pwm_frequency = PWM_FREQUENCY;
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
    // ramp target_velocity toward desired_velocity each FOC iteration
    float diff = desired_velocity[0] - target_velocity[0];
    if(diff > RAMP_RATE)       target_velocity[0] += RAMP_RATE;
    else if(diff < -RAMP_RATE) target_velocity[0] -= RAMP_RATE;
    else                       target_velocity[0] = desired_velocity[0];

    motor.loopFOC();
    motor.move(target_velocity[0]);
  }

  /*
  IMPORTANT NOTE:
  we moved the input handling loop outside the motor loop.
  the inputs are buffered, so if you increase the LOOP_DUTY_CYCLE
  too much, it will be high latency between the inputs and seeing them
  on the actual robot.
  We chose to prioritize the execution of the simpleFOC loop
  in terms of overall CPU time for the smoothest operation.
  */


  // movment logic
  // ROVER_STOP = 0x00,
  // ROVER_FWD = 0x01,
  // ROVER_REV = 0x02,
  // ROVER_LEFT = 0x03,
  // ROVER_RIGHT = 0x04,
  
  incoming_byte = Serial.read();
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

  
  // motor control code.

  // note: using constrain() is super slow for some reason.
  for(int i = 0; i < NUM_BANKS; i++)
  {
    rover_speed[i] = clamp(rover_speed[i], NUM_SPEEDS * -1, NUM_SPEEDS);
    desired_velocity[i] = RPMtoRads(RPM_MULT * rover_speed[i]);
  }

  // dynamic voltage scaled to actual ramped speed
  float actual_rpm = abs(radstoRPM(target_velocity[0]));
  driver.voltage_limit = 6 + (2.0 * actual_rpm / RPM_MULT);
}