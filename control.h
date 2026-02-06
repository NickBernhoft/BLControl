// control.h
#ifndef CONTROL_H
#define CONTROL_H

// function prototypes
void incrementPos(int dir, int bank);
void step(int dir, int bank);
void updateSignal(unsigned int bank);
void allLow(int bank);
void runRPM(unsigned int rpm, float seconds, int dir, int bank);
void stepRPM(unsigned int rpm, int numSteps, int dir, int bank);
void longDelayMicroseconds(unsigned long int microDelay);
void ramp(unsigned int startRPM, unsigned int finalRPM, int dir);
void ramp2(unsigned int startRPM, unsigned int finalRPM, int dir, unsigned int rateIn, int bank);
long unsigned int getStepTime(unsigned int rpm);

// variables
extern int motorPosition[];

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
  ROVER_HALT = 0xff,
  ROVER_FRONT = 0x05,
  ROVER_BACK = 0x06,
  ROVER_RAISE = 0X07,
  ROVER_LOWER = 0X08
};

#endif // CONTROL_H