// hardware notes:
// 6 steps in a full rotation of the windings
// 7 rotations per revolution
// 6 * 7 = 42 steps per rotation
// 360 / 42 = 8.5714 degrees per step = 0.1496 radians per step

// testing notes:
// max achieved rpm using simple ramping: 3571rpm
// max achieved left-right without losing steps: 5ms step time, 50ms rest between time
// without rapming RPM the max instant start speed is about 600 rpm



// TODO
// add support for the Serial inputs from the Pi / potato
// make a function that spin
// maybe make it so we dont have to use allLow() which is slightly wasteful of clock cycles
// make a change speed function that supports ramping of speeds
// global variables for current speed and other stuff?
// make rampRPM function

#include "music.h"
#include "control.h"


void setup()
{
  // for debugging
  Serial.begin(115200);

  //LED
  pinMode(13, OUTPUT);

  pinMode(12, OUTPUT);
  pinMode(11, OUTPUT);
  pinMode(10, OUTPUT);
  pinMode(9, OUTPUT);
  pinMode(8, OUTPUT);
  pinMode(7, OUTPUT);
}


float mrs = 3000;
int temp = 1;

void loop()
{
  // the first 4 notes of meglovania lmao
  music_bpm = 240;
  playNote(D, 3, 4);
  delay(10);
  playNote(D, 3, 4);
  delay(10);
  playNote(D, 4, 4);
  delay(10);
  playNote(A, 3, 4);
  delay(10);
  playNote(As, 3, 8);
  delay(10);

  delay(1000);



  //runRpm Demo
  runRPM(600, 3);
  delay(500);


  // rotate clockwise test code

  // for(int i = 0; i < 6 * 7; i++)
  // {
  //   step(1);
  //   delay(24);
  // }

  // delay(2000);


  //left -> right test code 

  // for(int i = 0; i < 6 * 7; i++)
  // {
  //   step(temp);
  //   delay(5);
  // }

  // delay(1000);
  // temp *= -1;



  // ramp to 3500 rpm test code

  // step(-1);
  // delayMicroseconds(mrs);
  // mrs--;

  // if(mrs < 400)
  // {
  //   mrs = 400;
  // }


}