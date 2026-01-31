// control.h
#ifndef CONTROL_H
#define CONTROL_H

// function prototypes
void incrementPos(int diff);
void step(int dir);
void allLow();
void runRPM(unsigned int rpm, float seconds, int dir);
void stepRPM(unsigned int rpm, int numSteps, int dir);
void longDelayMicroseconds(unsigned long int microDelay);
void ramp(unsigned int startRPM, unsigned int finalRPM, int dir);
void ramp2(unsigned int startRPM, unsigned int finalRPM, int dir, unsigned int rateIn);
long unsigned int getStepTime(unsigned int rpm);

// variables
extern int motorPosition;

enum motor_dir
{
  FWD = 1,
  REV = -1
};

#endif // CONTROL_H