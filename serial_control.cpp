#include <Arduino.h>
#include "control.h"
#include "serial_control.h"

namespace {
int activeBank = 0;
unsigned int runtimeRpm[NUM_BANKS] = {200, 200};
int runtimeDir[NUM_BANKS] = {REV, REV};
bool motorRunEnabled = true;

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
}

void serialControlInit()
{
  Serial.println("Control2 ready. Commands: S status, B<0|1> bank, R<rpm>, D<1|-1>, M<0|1>");
}

void serialControlProcess()
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

void serialControlApplyMotorSettings()
{
  for (int bank = 0; bank < NUM_BANKS; bank++)
  {
    updateBank(bank, runtimeRpm[bank], runtimeDir[bank]);
  }
}

bool serialControlIsMotorRunEnabled()
{
  return motorRunEnabled;
}
