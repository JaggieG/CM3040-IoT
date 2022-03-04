#include "Arduino.h"
#include "JAG_gas.h"
// initialise

JAG_gas::JAG_gas(int gasSensorInput_, int sensorThreshold_, bool logSerial_) 
{
 gasSensorInput = gasSensorInput_;
 sensorThreshold = sensorThreshold_;
 logSerial = logSerial_;
}

// Public Functions

void JAG_gas::setupGasSensor() {
    pinMode(gasSensorInput, INPUT);
}

int JAG_gas::getGasValue() {
  return analogRead(gasSensorInput);
}
