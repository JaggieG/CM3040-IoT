/*
  JAG_webserver.h - Library for handling MQ-2
  Created by John Gerhardt  10/10/2021
*/

#ifndef JAG_gas_h
#define JAG_gas_h

#include "Arduino.h"

class JAG_gas
{
  public:
    JAG_gas(int gasSensorInput_, int sensorThreshold_, bool logSerial_);
    void setupGasSensor();
    int getGasValue();
  private:
    bool logSerial;
    int gasSensorInput;
    int sensorThreshold;
};

#endif
