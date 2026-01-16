#include <Arduino.h>
#include "control.h"

// global variables
int motorPosition = 0; // 0-5, there are 6 positions

void runRPM(int rpm, float seconds)
{
  // rmp to steps conversion:
  float numRevs = rpm * (seconds/60.0);
  int numSteps = numRevs * 42.0;

  // Serial.println(numRevs);
  // Serial.println(numSteps);
  
  // delay between steps in microseconds
  unsigned long microDelay = (seconds / (numRevs * 42)) * 1000000.0;


  for(int i = 0; i < numSteps; i++)
  {
    step(1);

    // delayMicroseconds is only accurate up to a few thousand
    longDelayMicroseconds(microDelay);
  }
}

//TODO
void ramp(int startRPM, int finalRPM)
{

}





// this function is for more percise delays when you get over 15ms
// delayMicroseconds is only accurate up to a few thousand, after this use milliseconds
void longDelayMicroseconds(unsigned long int microDelay)
{
    if(microDelay > 5000)
  {
    delay(microDelay / 1000);
    // pickup the starggling microseconds for accuracy
    delayMicroseconds(microDelay % 5000);
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
