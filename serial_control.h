#ifndef SERIAL_CONTROL_H
#define SERIAL_CONTROL_H

void serialControlInit();
void serialControlProcess();
void serialControlApplyMotorSettings();
bool serialControlIsMotorRunEnabled();

#endif // SERIAL_CONTROL_H
