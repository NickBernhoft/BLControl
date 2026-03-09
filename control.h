// control.h
#ifndef CONTROL_H
#define CONTROL_H

#define NUM_BANKS 2   // number of independent controllers
#define NUM_STEPS 21    // number of steps per revolution, could vary per motor

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
void updateBank(int bank, unsigned int rpm, int dir);
inline void setNextStep(int bank);
void runAll();
float radstoRPM(float rads);

// variables
extern int motorPosition[];
extern unsigned int currentRPM[];
extern unsigned long currentDelay[];
extern int currentDir[];
extern unsigned long nextStep[];

extern unsigned long time;

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

#endif // CONTROL_H