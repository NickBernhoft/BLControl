#include <Arduino.h>
#include "control.h"

// global variables
int motorPosition[NUM_BANKS] = {0}; // 0-5, there are 6 positions
unsigned int currentRPM[NUM_BANKS] = {0};
int currentDir = FWD;

/*
// runs the motor at a certain RPM for a certain amount of seconds
// ints are unsigned bc RPM cannot be negative
*/
void runRPM(unsigned int rpm, float seconds, int dir, int bank)
{
  // if switching direction, ramp down to low speed
  if (currentDir != dir)
  {
    ramp2(currentRPM, 10, currentDir, 2, bank);
    currentRPM = 10;
  }

  currentDir = dir; // update global variable

  // rmp to steps conversion:
  float numRevs = rpm * (seconds/60.0);
  int numSteps = numRevs * 42.0;
  
  // delay between steps in microseconds
  unsigned long microDelay = (seconds / (numRevs * 42)) * 1000000.0;

  ramp2(currentRPM, rpm, dir, 2, bank);
  currentRPM = rpm; // update global variable

  for(int i = 0; i < numSteps; i++)
  {
    step(dir, bank);
    // delayMicroseconds() is only accurate up to a few thousand
    longDelayMicroseconds(microDelay);
  }
}








// runs all banks at the specified speeds and directions in real time
runAll()
{
  for(int i = 0; i < NUM_BANKS)
  {
    
  }
}










/*
like runRPM() except it takes a certain number of steps in as an argument
ints are unsigned bc RPM cannot be negative
*/
void stepRPM(unsigned int rpm, int numSteps, int dir, int bank)
{
  // delay between steps in microseconds
  unsigned long microDelay = (((60.0/rpm) * 1000000.0) / 42.0);

  for(int i = 0; i < numSteps; i++)
  {
    step(dir, bank);
    // delayMicroseconds() is only accurate up to a few thousand
    longDelayMicroseconds(microDelay);
  }
}


// rate is in microseconds (between steps) per step
// in this function we slowly reduce the steptime in order to speed up the motor
// this is a more direct / low level approach then ramp2
// this version is more open ended in the ramping funcgion
void ramp(unsigned int startRPM, unsigned int finalRPM, int dir, int bank)
{
  int currentStepTime = getStepTime(startRPM);
  int finalStepTime = getStepTime(finalRPM);

  // setting a steeper initial if were starting at a very low RPM;

  // increasing speed
  if(startRPM < finalRPM)
  {
    while(currentStepTime > finalStepTime)
    {
      step(dir, bank);
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
      step(dir, bank);
      longDelayMicroseconds(currentStepTime);
      currentStepTime += currentStepTime / 100;
    }

    return;
  }
}


/*
ints are unsigned bc RPM cannot be negative
rate is the number by which we reduce the RPM per revolution (rate = -10, rpm: 100, 90, 80, 70)
TODO: doesnt support 0 right now, doesnt run
*/
void ramp2(unsigned int startRPM, unsigned int finalRPM, int dir, unsigned int rateIn, int bank)
{
  int currentRPM = startRPM;
  int diff = finalRPM - startRPM; // positave diff = increasing speed, negative meand decreasing
  int rate = rateIn;
  int dwell = 3; // number of steps to run a single RPM value before incrementing the RPM


  if(diff == 0)
  {
    diff = 1;
  }

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
    stepRPM(currentRPM, dwell, dir, bank);

    // Serial.println(currentRPM);
  }

  // by this point we should have >rate number of steps to make up for
  stepRPM(finalRPM, dwell, dir, bank);

}


// returns the number of microseconds between steps at a certain RPM
long unsigned int getStepTime(unsigned int rpm)
{
  if(rpm <= 0)
  {
    return 4294967295; // no easy bind for UINT_MAX?
  }

  return (long unsigned int)( (60.0/(float)rpm * 1000000.0) /42);
}


// this function is for more percise delays when you get over 15ms
// delayMicroseconds() is only accurate up to a few thousand, after this use milliseconds
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

/*
steps banks foward, backward, or both (spin)
changes the control signals / digital output pins as well
*/
void step(int dir, int bank) // does not accept negative bank number
{
  switch(dir)
  {
    case REV: // -1
    case FWD: // 1
      incrementPos(dir, bank);
      allLow(bank);
      break;

    // case for one bank going one direction, and the other bank going the opposite direction
    // if the bank is 0, it will spin regularly, if the bank is 1 it till spin inverse
    case SPIN: // 0
      switch(bank)
      {
        case 0:
          incrementPos(FWD, 0);
          incrementPos(REV, 1);
          break;
        
        case 1:
          incrementPos(REV, 0);
          incrementPos(FWD, 1);
          break;
      }

      allLow(-1);
    break;

    default:
      break;
  }

  // update the signal outputs of all banks
    updateSignal(0);
    updateSignal(1);
}


/*
this sets the correct pin for the winding and polarity
and updates the actual pin output of the arduino to reflect
whatever the current "motorPosition" of whatever bank you give it
expand this code if you want to support more banks
*/
void updateSignal(unsigned int bank) // does not support -1 (all banks)
{
  switch(bank)
  {
    case 0: // original bank
      switch(motorPosition[bank])
      {
        case 0:
          digitalWrite(12, HIGH);
          digitalWrite(13, HIGH); // LED on at the pos 0
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
      break;

    // A0 = 7, A1 = 8, A2 = 9, A3 = 10, A4 = 11, A5 = 12
    case 1: // second bank
      switch(motorPosition[bank])
    {
      case 0:
        digitalWrite(A5, HIGH);
        digitalWrite(13, HIGH); // LED on at the pos 0
        break;
      
      case 1:
        digitalWrite(A3, HIGH);
        break;

      case 2:
        digitalWrite(A1, HIGH);
        break;

      case 3:
        digitalWrite(A4, HIGH);
        break;

      case 4:
        digitalWrite(A2, HIGH);
        break;

      case 5:
        digitalWrite(A0, HIGH);
        break;

      default:
        allLow(-1);
        break;
      break;
    }
  }
}



// this changes the motor position by diff
// looping within the range 0 - 6
// supports a little overflow
void incrementPos(int dir, int bank)
{
  // i honestly forgot now i figured this math out, but it works
  motorPosition[bank] += abs((6 + dir) % 6);
  // maybe motorPosition += abs((6 * diff) % 6);
  motorPosition[bank] = motorPosition[bank] % 6; // this supports the overflow
  //Serial.println(motorPosition[0]);
}

// old all Low function 
// void allLow(int bank)
// {
//   switch(bank)
//   {
//     case -1: // all
//       digitalWrite(A0, LOW);
//       digitalWrite(A1, LOW);
//       digitalWrite(A2, LOW);
//       digitalWrite(A3, LOW);
//       digitalWrite(A4, LOW);
//       digitalWrite(A5, LOW);
//       digitalWrite(A6, LOW);
//       digitalWrite(13, LOW);  // LED
//       digitalWrite(12, LOW);
//       digitalWrite(11, LOW);
//       digitalWrite(10, LOW);
//       digitalWrite(9, LOW);
//       digitalWrite(8, LOW);
//       digitalWrite(7, LOW);
//       break;

//     case 0:
//       digitalWrite(13, LOW);  // LED
//       digitalWrite(12, LOW);
//       digitalWrite(11, LOW);
//       digitalWrite(10, LOW);
//       digitalWrite(9, LOW);
//       digitalWrite(8, LOW);
//       digitalWrite(7, LOW);
//       break;

//     case 1:
//       digitalWrite(A0, LOW);  // LED
//       digitalWrite(A1, LOW);
//       digitalWrite(A2, LOW);
//       digitalWrite(A3, LOW);
//       digitalWrite(A4, LOW);
//       digitalWrite(A5, LOW);
//       break;

//       default:
//         break;
//   }
// }

// optimized version using bit masks
// inline prevents branching to function and runs code directly for faster execution
// Note: these bit masks are specific to the nano and would need to be updated to accomodate to a different type of arduino
inline void allLow(int bank)
{
  switch (bank)
  {
    case -1:  // ALL motors
      PORTB &= ~0b00111111;   // D8–D13 LOW
      PORTD &= ~(1 << 7);     // D7 LOW
      PORTC &= ~0b00111111;   // A0–A5 LOW
      break;

    case 0:   // right motor only
      PORTB &= ~0b00111111; // D8-D13
      PORTD &= ~(1 << 7); // D7
      break;

    case 1:   // left motor only
      PORTC &= ~0b00111111; //A0-A5 LOW
      break;
  }
}
