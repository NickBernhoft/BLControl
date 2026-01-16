// control.h
#ifndef CONTROL_H
#define CONTROL_H

// function prototypes
void incrementPos(int diff);
void step(int dir);
void allLow();
void runRPM(int rpm, float seconds);
void longDelayMicroseconds(unsigned long int microDelay);
//TODO
void ramp(int startRPM, int finalRPM);

// variables
extern int motorPosition;

#endif // CONTROL_H