/*
  JAG_webserver.h - Library for handling web server for ESP8826
  Created by John Gerhardt  10/10/2021
*/

#ifndef JAG_wificlient_h
#define JAG_wificlient_h

#include "Arduino.h"
#include <ESP8266WiFi.h>
#include <WiFiClient.h>

class JAG_wificlient
{
  public:
    JAG_wificlient(char* SSID, char* key, bool logSerial);
    void connectToWireless();
    String getIPAddress();
    char* getConnectedSSID();
  private:
    char* SSID_;
    char* key_;
    bool logSerial_;
    uint32_t IP_ADDRESS;
};

#endif
