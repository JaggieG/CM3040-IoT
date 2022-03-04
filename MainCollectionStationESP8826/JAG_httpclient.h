

#ifndef JAG_httpclient_h
#define JAG_httpclient_h

#include "Arduino.h"
#include <ESP8266HTTPClient.h>
#include <WiFiClient.h>

class JAG_httpclient
{
  public:
    JAG_httpclient(bool logSerial, String remoteStationIP);
    HTTPClient http;  //Declare an object of class HTTPClient
    void sendAllValues(String Temperature, String Humidity);
    void sendLocalTemperatureValuesToRemoteStation(String temperature);
    void sendLocalHumidityValuesToRemoteStation(String Humidity);
    void sendCardValueToRemoteStation(String username);
    void sendLastUpdateCardValueToRemoteStation(String username);
    void sendAggregateCardValueToRemoteStation(String username);
    WiFiClient wifiClient;
  private:
    bool logSerial_;
    String remoteStationIP_;
    String localIdent_;
    String localStationName_;
};

#endif
