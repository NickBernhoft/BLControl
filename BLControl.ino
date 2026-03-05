// hardware notes:
// 7 rotations per revolution
// 6 * 7 = 21 steps per rotation
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


int activeBank = 0;
unsigned int runtimeRpm[NUM_BANKS] = {200, 200};
int runtimeDir[NUM_BANKS] = {REV, REV};
bool motorRunEnabled = true;


void processSerialCommand();
void printStatus();


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

  allLow(-1);
  Serial.println("Control2 ready. Commands: S status, B<0|1> bank, R<rpm>, D<1|-1>, M<0|1>");
}


byte incoming_byte = 255;
int speed = 0;



void loop()
{  
  processSerialCommand();

  for (int bank = 0; bank < NUM_BANKS; bank++)
  {
    updateBank(bank, runtimeRpm[bank], runtimeDir[bank]);
  }

  if (motorRunEnabled)
  {
    runAll();
  }
  else
  {
    allLow(-1);
  }
}


void processSerialCommand()
{
  if (!Serial.available())
  {
    return;
  }

  char command = (char)Serial.read();

  if (command == 'S' || command == 's')
  {
    printStatus();
    return;
  }

  if (command == 'B' || command == 'b')
  {
    int bank = Serial.parseInt();
    if (bank < 0)
    {
      bank = 0;
    }
    if (bank >= NUM_BANKS)
    {
      bank = NUM_BANKS - 1;
    }

    activeBank = bank;
    Serial.print("Active bank set to ");
    Serial.println(activeBank);
    return;
  }

  if (command == 'R' || command == 'r')
  {
    long requestedRpm = Serial.parseInt();
    if (requestedRpm < 0)
    {
      requestedRpm = 0;
    }
    if (requestedRpm > 6000)
    {
      requestedRpm = 6000;
    }

    runtimeRpm[activeBank] = (unsigned int)requestedRpm;
    Serial.print("Bank ");
    Serial.print(activeBank);
    Serial.print(" RPM set to ");
    Serial.println(runtimeRpm[activeBank]);
    return;
  }

  if (command == 'D' || command == 'd')
  {
    int dir = Serial.parseInt();
    if (dir >= 0)
    {
      runtimeDir[activeBank] = FWD;
    }
    else
    {
      runtimeDir[activeBank] = REV;
    }

    Serial.print("Bank ");
    Serial.print(activeBank);
    Serial.print(" direction set to ");
    Serial.println(runtimeDir[activeBank] == FWD ? "FWD" : "REV");
    return;
  }

  if (command == 'M' || command == 'm')
  {
    int mode = Serial.parseInt();
    motorRunEnabled = (mode != 0);
    if (!motorRunEnabled)
    {
      allLow(-1);
    }

    Serial.print("Motor run ");
    Serial.println(motorRunEnabled ? "ENABLED" : "DISABLED");
    return;
  }

  if (command == '\n' || command == '\r')
  {
    return;
  }

  Serial.print("Unknown command: ");
  Serial.println(command);
}


void printStatus()
{
  Serial.print("activeBank=");
  Serial.print(activeBank);
  Serial.print(" run=");
  Serial.print(motorRunEnabled ? "1" : "0");

  for (int bank = 0; bank < NUM_BANKS; bank++)
  {
    Serial.print(" | B");
    Serial.print(bank);
    Serial.print(":rpm=");
    Serial.print(runtimeRpm[bank]);
    Serial.print(",dir=");
    Serial.print(runtimeDir[bank] == FWD ? "FWD" : "REV");
  }

  Serial.println();

}