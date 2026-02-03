#include <Arduino.h>
#include "control.h"

// global variables
int motorPosition = 0; // 0-5, there are 6 positions


// runs the motor at a certain RPM for a certain amount of seconds
// ints are unsigned bc RPM cannot be negative
void runRPM(unsigned int rpm, float seconds, int dir)
{
  // rmp to steps conversion:
  float numRevs = rpm * (seconds/60.0);
  int numSteps = numRevs * 42.0;
  
  // delay between steps in microseconds
  unsigned long microDelay = (seconds / (numRevs * 42)) * 1000000.0;

  // ramp2(100, rpm, dir, 1);

  for(int i = 0; i < numSteps; i++)
  {
    step(dir);
    // delayMicroseconds() is only accurate up to a few thousand
    longDelayMicroseconds(microDelay);
  }
}

// like runRPM() except it takes a certain number of steps in as an argument
// ints are unsigned bc RPM cannot be negative
void stepRPM(unsigned int rpm, int numSteps, int dir)
{
  // delay between steps in microseconds
  unsigned long microDelay = (((60.0/rpm) * 1000000.0) / 42.0);

  for(int i = 0; i < numSteps; i++)
  {
    step(dir);
    // delayMicroseconds() is only accurate up to a few thousand
    longDelayMicroseconds(microDelay);
  }
}


// rate is in microseconds (between steps) per step
// in this function we slowly reduce the steptime in order to speed up the motor
// this is a more direct / low level approach then ramp2
// this version is more open ended in the ramping funcgion
void ramp(unsigned int startRPM, unsigned int finalRPM, int dir)
{
  int currentStepTime = getStepTime(startRPM);
  int finalStepTime = getStepTime(finalRPM);

  // setting a steeper initial if were starting at a very low RPM;

  // increasing speed
  if(startRPM < finalRPM)
  {
    while(currentStepTime > finalStepTime)
    {
      step(dir);
      longDelayMicroseconds(currentStepTime);
      currentStepTime -= currentStepTime / 100;
    }

    return;
  }

  // decreasing speed
  if(finalRPM < startRPM)
  {
    while(currentStepTime < finalStepTime)
    {
      step(dir);
      longDelayMicroseconds(currentStepTime);
      currentStepTime += currentStepTime / 100;
    }

    return;
  }


}



// ints are unsigned bc RPM cannot be negative
// rate is the number by which we reduce the RPM per revolution (rate = -10, rpm: 100, 90, 80, 70)
void ramp2(unsigned int startRPM, unsigned int finalRPM, int dir, unsigned int rateIn)
{
  int currentRPM = startRPM;
  int diff = finalRPM - startRPM; // positave diff = increasing speed, negative meand decreasing
  int rate = rateIn;
  int dwell = 3; // number of steps to run a single RPM value before incrementing the RPM


  // if diff is negative, then reduce the speed
  if (diff < 0)
  {
    rate *= -1; 
  }

  int running = 1;
  while(running)
  {
    // check if we are about to overshoot the final RPM, well pisk up the slack after the while loop

    // case for increasing speed
    if(diff > 0 && currentRPM + rate >= finalRPM)
    {
      running = 0;
      break;
    }

    //case for decreasing speed
    if(diff < 0 && currentRPM + rate <= finalRPM)
    {
      running = 0;
      break;
    }

    currentRPM += rate;
    stepRPM(currentRPM, dwell, dir);

    // Serial.println(currentRPM);
  }

  // by this point we should have >rate number of steps to make up for
  stepRPM(finalRPM, dwell, dir);

}


// returns the number of microseconds between steps at a certain RPM
long unsigned int getStepTime(unsigned int rpm)
{
  if(rpm <= 0)
  {
    return 4294967295;
  }

  return (long unsigned int)( (60.0/(float)rpm * 1000000.0) /42);
}


// this function is for more percise delays when you get over 15ms
// delayMicroseconds is only accurate up to a few thousand, after this use milliseconds
void longDelayMicroseconds(unsigned long int microDelay)
{
    if(microDelay > 1000)
  {
    delay(microDelay / 1000);
    // pickup the starggling microseconds for accuracy
    delayMicroseconds(microDelay % 1000);
  }
  else
  {
    delayMicroseconds(microDelay);
  }
}

// pin > position table
// 12  0
// 10  1
// 8   2
// 11  3
// 9   4
// 7   5

// note: this code is not very optimized, its just for testing rn
// this sets the correct pin for the winding and polariey
void step(int dir)
{
  incrementPos(dir);
  allLow();

  switch(motorPosition)
  {
    case 0:
      digitalWrite(12, HIGH);
      digitalWrite(13, HIGH); // LED at the pos 0
      break;
    
    case 1:
      digitalWrite(10, HIGH);
      break;

    case 2:
      digitalWrite(8, HIGH);
      break;

    case 3:
      digitalWrite(11, HIGH);
      break;

    case 4:
      digitalWrite(9, HIGH);
      break;

    case 5:
      digitalWrite(7, HIGH);
      break;
  }
}



// this changes the motor position by diff
// looping within the range 0 - 6
// this doesnt support overflow right now
void incrementPos(int diff)
{
  // i honestly forgot now i figured this math out, but it works
  // its probably not fully optimized
  motorPosition += abs((6 + diff) % 6);
  // maybe motorPosition += abs((6 * diff) % 6);
  motorPosition = motorPosition % 6; // this supports the overflow
  //Serial.println(motorPosition);
}

// optimze this eventually?
// we may need the clock cycles at super high RPMs
void allLow()
{
  digitalWrite(13, LOW);  // LED
  digitalWrite(12, LOW);
  digitalWrite(11, LOW);
  digitalWrite(10, LOW);
  digitalWrite(9, LOW);
  digitalWrite(8, LOW);
  digitalWrite(7, LOW);
}