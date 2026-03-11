#include <SimpleFOC.h>
#include "controlfoc.h"

#define POLE_PAIRS 7

BLDCMotor motor0(POLE_PAIRS);
BLDCDriver3PWM driver0(9, 10, 11, 8);

BLDCMotor motor1(POLE_PAIRS);
BLDCDriver3PWM driver1(5, 6, 7, 4);

float target_angle = 0.0;
float step_size = 0.25;   // radians per step
float target_velocity[2] = {0.0}; // radians per second (actual, ramped)
float desired_velocity[2] = {0.0}; // radians per second (commanded)

#define RAMP_RATE 0.05 // rad/s per FOC loop iteration (runs LOOP_DUTY_CYCLE times per outer loop)

byte incoming_byte = 255;
int rover_speed[2] = {0}; // positive for clockwise

void setup() {
  Serial.begin(115200);

  // enable pin must be set to high on all drivers
  pinMode(8, OUTPUT);
  digitalWrite(8, HIGH);
  pinMode(4, OUTPUT);
  digitalWrite(4, HIGH);

  // motor 0 setup
  driver0.voltage_power_supply = SUPPLY_VOLTAGE;
  driver0.voltage_limit = 8;
  driver0.pwm_frequency = PWM_FREQUENCY;
  driver0.init();

  motor0.linkDriver(&driver0);
  motor0.controller = MotionControlType::velocity_openloop;
  motor0.voltage_limit = SUPPLY_VOLTAGE;
  motor0.init();
  motor0.enable();

  // motor 1 setup
  driver1.voltage_power_supply = SUPPLY_VOLTAGE;
  driver1.voltage_limit = 8;
  driver1.pwm_frequency = PWM_FREQUENCY;
  driver1.init();

  motor1.linkDriver(&driver1);
  motor1.controller = MotionControlType::velocity_openloop;
  motor1.voltage_limit = SUPPLY_VOLTAGE;
  motor1.init();
  motor1.enable();

  Serial.println("Motors ready");
}

void loop() {

  
  for(int i = 0; i < LOOP_DUTY_CYCLE; i++)
  {
    // ramp both motors toward desired velocity each FOC iteration
    for(int b = 0; b < NUM_BANKS; b++)
    {
      float diff = desired_velocity[b] - target_velocity[b];
      if(diff > RAMP_RATE)       target_velocity[b] += RAMP_RATE;
      else if(diff < -RAMP_RATE) target_velocity[b] -= RAMP_RATE;
      else                       target_velocity[b] = desired_velocity[b];
    }

    motor0.loopFOC();
    motor0.move(target_velocity[0]);
    motor1.loopFOC();
    motor1.move(target_velocity[1]);
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

  // dynamic voltage scaled to actual ramped speed per motor
  float rpm0 = abs(radstoRPM(target_velocity[0]));
  driver0.voltage_limit = 6 + (2.0 * rpm0 / RPM_MULT);
  float rpm1 = abs(radstoRPM(target_velocity[1]));
  driver1.voltage_limit = 6 + (2.0 * rpm1 / RPM_MULT);
}