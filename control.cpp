#include <Arduino.h>
#include "control.h"

// global variables
int motorPosition[NUM_BANKS] = {0}; // 0-5, there are 6 positions
unsigned int currentRPM[NUM_BANKS] = {0};
unsigned long currentDelay[NUM_BANKS] = {0}; // time between steps to achieve currentRPM
int currentDir[NUM_BANKS] = {FWD};
unsigned long nextStep[NUM_BANKS];            // time that the next step should take place

unsigned long time; // program time from micros()


/*
runs all banks at the specified speeds and directions in real time
run a step for any overdue bank

This function assumes it is executed instantly in terms of timing.
there will be a very minor loss of time percision so actual RPM may vary slightly

TODO: make sure to keep track of Micros as it overflows to 0 every 70 min
*/
void runAll()
{
  time = micros(); // set time

  // scan for an overdue step in all banks
  for(int i = 0; i < NUM_BANKS; i++)
  {
    if (nextStep[i] < time)
    {
      step(currentDir[i], i);
      setNextStep(i);
    }
  }
}

/*
updates the currentRPM and currentDelay
generating the delay once and referencing is much faster
*/
void updateBank(int bank, unsigned int rpm, int dir)
{
  currentRPM[bank] = rpm;
  currentDir[bank] = dir;
  currentDelay[bank] = getStepTime(rpm);
}

/*
calculated the time (micros) that the next step should take place
for a certain bank

takes the current time and adds the step Delay to it
this should be called after every step a motor takes
*/
inline void setNextStep(int bank)
{
  // this will overflow automatically
  //nextStep[bank] = micros() + currentDelay[bank];
  nextStep[bank] = time + currentDelay[bank];
}






/*
// runs the motor at a certain RPM for a certain amount of seconds
// ints are unsigned bc RPM cannot be negative
*/
void runRPM(unsigned int rpm, float seconds, int dir, int bank)
{
  // if switching direction, ramp down to low speed
  if (currentDir != dir)
  {
    ramp2(currentRPM[bank], 10, currentDir[bank], 2, bank);
    currentRPM[bank] = 10;
  }

  currentDir[bank] = dir; // update global variable

  // rmp to steps conversion:
  float numRevs = rpm * (seconds/60.0);
  int numSteps = numRevs * (float)NUM_STEPS;
  
  // delay between steps in microseconds
  unsigned long microDelay = (seconds / (numRevs * NUM_STEPS)) * 1000000.0;

  ramp2(currentRPM, rpm, dir, 2, bank);
  currentRPM[bank] = rpm; // update global variable

  for(int i = 0; i < numSteps; i++)
  {
    step(dir, bank);
    // delayMicroseconds() is only accurate up to a few thousand
    longDelayMicroseconds(microDelay);
  }
}




/*
like runRPM() except it takes a certain number of steps in as an argument
ints are unsigned bc RPM cannot be negative
*/
void stepRPM(unsigned int rpm, int numSteps, int dir, int bank)
{
  // delay between steps in microseconds
  unsigned long microDelay = (((60.0/rpm) * 1000000.0) / (float)NUM_STEPS);

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

  return (long unsigned int)( (60.0/(float)rpm * 1000000.0) / (float)NUM_STEPS);
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
    // this will not be executed in scheduled
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
    case 0: // original bank uses pins 10, 11, and 12
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
        }
      break;

    case 1: // second bank uses pins 7, 8, and 9
      switch(motorPosition[bank])
    {
      case 0:
        digitalWrite(7, HIGH);
        break;
      
      case 1:
        digitalWrite(8, HIGH);
        break;

      case 2:
        digitalWrite(9, HIGH);
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
  motorPosition[bank] += abs((3 + dir) % 3);
  motorPosition[bank] = motorPosition[bank] % 3; // this supports the overflow on upj to three for some
  //Serial.println(motorPosition[0]);
}


// TODO: update this for the new controller
/*
optimized version using bit masks
inline prevents branching to function and runs code directly for faster execution
Note: these bit masks are specific to the nano and would need to be updated to accomodate to a different type of arduino
*/ 
inline void allLow(int bank)
{
  switch (bank)
  {
    case -1:  // ALL motors
      PORTB &= ~0b00001111;   // D10–D13 LOW
      //PORTD &= ~(1 << 7);     // D7 LOW
      PORTC &= ~0b00111111;   // A0–A5 LOW
      break;

    case 0:   // right motor only
      PORTB &= ~0b00001111; // D10-D13
      //PORTD &= ~(1 << 7); // D7
      break;

    case 1:   // left motor only
      PORTC &= ~0b00111111; //A0-A5 LOW
      break;
  }
}
