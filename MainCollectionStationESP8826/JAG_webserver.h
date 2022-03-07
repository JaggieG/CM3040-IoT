/*
  JAG_webserver.h - Library for handling web server for ESP8826
  Created by John Gerhardt  10/10/2021
*/

#ifndef JAG_webserver_h
#define JAG_webserver_h

#include "Arduino.h"
#include <ESP8266WebServer.h>
#include <ArduinoJson.h>


class JAG_webserver
{
  public:
    JAG_webserver(int port, bool logSerial);
    void setup();
    void runServerInLoop();
    void setupRoutes();
    ESP8266WebServer webserver;
    String get404PageContents();
    String getRootPageContents();
    String createHTMLpageWithContent_(String theContent);

    /* TEMP ITEMS */

    void printOutGas();
  private:
    int port_;
    bool logSerial_;

    void processReceivedStrings(DynamicJsonDocument doc);
    String createStationInfo(String stationNumber);
    String createBadgingInfo();
    String showNiceLastConnect(unsigned long last_connect);
    String IpAddress2String(const IPAddress& ipAddress);

    void addGasValueToArray(String value, String stationId);
    
    String remote1_temp;
    String remote1_humidity;
    String remote1_GAS;
    String remote1_GAS_history[100];
    String remote1_name;
    String remote1_lastbadge;
    unsigned long remote1_lastconnect;

    String remote2_temp;
    String remote2_humidity;
    String remote2_GAS;
    String remote2_GAS_history[100];
    String remote2_name;
    String remote2_lastbadge;
    unsigned long remote2_lastconnect;

    String remote3_temp;
    String remote3_humidity;
    String remote3_GAS;
    String remote3_name;
    String remote3_lastbadge;
    String remote3_GAS_history[100];
    unsigned long remote3_lastconnect;

    String badgingUser[99];
    unsigned long pageLastUpdate;

    String createJSDataForGAS(String stationId);
};

#endif
