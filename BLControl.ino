// hardware notes:
// 6 steps in a full rotation of the windings
// 6 * 7 = 42 steps per rotation
// 360 / 42 = 8.5714 degrees per step = 0.1496 radians per step

// testing notes:
// max achieved rpm using simple ramping: 3571rpm
// max achieved left-right without losing steps: 5ms step time, 50ms rest between time



// TODO
// add support for the Serial inputs from the Pi / potato
// make a function that spin
// maybe make it so we dont have to use allLow() which is slightly wasteful of clock cycles
// make a change speed function that supports ramping of speeds
// global variables for current speed and other stuff?


// global variables
// 0 - 5
int motorPosition = 0;

// function prototypes
void incrementPos(int diff);
void step(int dir);
void allLow();

void setup()
{
  // for debugging
  Serial.begin(9600);

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

  // // rotate clockwise test code

  for(int i = 0; i < 6 * 7; i++)
  {
    step(1);
    delay(100);
  }

  //delay(1000);


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
