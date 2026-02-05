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
  Serial.begin(115200);

  // right side motors
  pinMode(13, OUTPUT);  //LED
  pinMode(12, OUTPUT);
  pinMode(11, OUTPUT);
  pinMode(10, OUTPUT);
  pinMode(9, OUTPUT);
  pinMode(8, OUTPUT);
  pinMode(7, OUTPUT);

  // left side motors
  // setting the analog out pins as digital out
  pinMode(A0, OUTPUT);
  pinMode(A1, OUTPUT);
  pinMode(A2, OUTPUT);
  pinMode(A3, OUTPUT);
  pinMode(A4, OUTPUT);
  pinMode(A5, OUTPUT);
}


byte incoming_byte = 255;
int speed = 0;

void loop()
{
  //demos and testing
  // meglovania();
  // ramp2(100, 20000, FWD, 1);
  // ramp2(20000, 100, FWD, 3);
  // runRPM(3000, 10, FOWARD);
  // delay(5000);

  runRPM(500, 5.0, SPIN, -1);
  runRPM(500, 5.0, REV, -1);
  // runRPM(500, 5.0, REV, 1);
  delay(2000);

  // incoming_byte = Serial.read();
  // Serial.println(incoming_byte);

  // incoming_byte = ROVER_FWD;

  // switch(incoming_byte)
  // {
  //   case ROVER_HALT: // fallthrough
  //   case ROVER_STOP:
  //   speed = 0;
  //     break;

  //   case ROVER_FWD:
  //     if(speed < 3) {
  //       speed++;
  //     }
  //     break;

  //   case ROVER_REV:
  //     if(speed > -3) {
  //       speed--;
  //     }
  //     break;

  //   default:
  //     break;
  // }

  // switch(speed)
  // {
  //   case 1:
  //     runRPM(100, 1.0, FWD, -1);
  //     break;

  //   case 2:
  //     runRPM(250, 1.0, FWD, -1);
  //     break;

  //   case 3:
  //     runRPM(500, 1.0, FWD, -1);
  //     break;

  //   case -1:
  //     runRPM(100, 1.0, REV, -1);
  //     break;

  //   case -2:
  //     runRPM(250, 1.0, REV, -1);
  //     break;

  //   case -3:
  //     runRPM(500, 1.0, REV, -1);
  //     break;

  //   default:
  //     speed = 0;
  //     break;

  // }

}