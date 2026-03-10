// control.h
#ifndef CONTROLFOC_H
#define CONTROLFOC_H

#define NUM_BANKS 2   // number of independent controllers
#define NUM_STEPS 21    // number of steps per revolution, could vary per motor


#define POLE_PAIRS 7
#define PWM_FREQUENCY 20000
#define SUPPLY_VOLTAGE 12.0
#define NUM_SPEEDS 3  // number of motor speeds. 3 foward, 3 backwards
#define LOOP_DUTY_CYCLE 1000 // ratio between motor mo

// function prototypes
float radstoRPM(float rads);
float RPMtoRads(float rpm);
int clamp(int input, int min, int max);

enum motor_dir
{
  FWD = 1,
  REV = -1,
  SPIN = 0
};

// commands from the pi
enum COMMANDS {
  ROVER_STOP = 0x00,
  ROVER_FWD = 0x01,
  ROVER_REV = 0x02,
  ROVER_LEFT = 0x03,
  ROVER_RIGHT = 0x04,
  //ROVER_HALT = 0xff, // depricated
  ROVER_FRONT = 0x05,
  ROVER_BACK = 0x06,
  ROVER_RAISE = 0X07,
  ROVER_LOWER = 0X08
};

#endif // CONTROLFOC_H