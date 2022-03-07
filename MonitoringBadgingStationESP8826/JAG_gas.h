/*
 * JAG_gas.h
 * header file for the gas Chip 
 */

#ifndef JAG_gas_h
#define JAG_gas_h

#include "Arduino.h"

class JAG_gas
{
  public:
    JAG_gas(int gasSensorInput_, bool logSerial_);
    void setupGasSensor();
    int getGasValue();
  private:
    bool logSerial;
    int gasSensorInput;  
};

#endif
