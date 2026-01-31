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
// make the rate for ramp() customisable

#include "music.h"
#include "control.h"
#include "demos.h"


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


byte incoming_byte = 255;
int speed = 0;

void loop()
{
  //meglovania();

  incoming_byte = Serial.read();
  Serial.println(incoming_byte);

  switch(incoming_byte)
  {
    case 0:
    speed = 0;
      break;

    case 1:
      if(speed < 3) {
        speed++;
      }
      break;

    case 2:
      if(speed > -3) {
        speed--;
      }
      break;

    default:
      break;
  }

  switch(speed)
  {
    case 1:
      runRPM(100, 1.0, FWD);
      break;

    case 2:
      runRPM(250, 1.0, FWD);
      break;

    case 3:
      runRPM(500, 1.0, FWD);
      break;

    case -1:
      runRPM(100, 1.0, REV);
      break;

    case -2:
      runRPM(250, 1.0, REV);
      break;

    case -3:
      runRPM(500, 1.0, REV);
      break;

    default:
      speed = 0;
      break;

  }

}