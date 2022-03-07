/*
  JAG_webserver.h - Library for handling web server for ESP8826
  Created by John Gerhardt  10/10/2021
*/

#ifndef JAG_webserver_h
#define JAG_webserver_h

#include "Arduino.h"
#include <ESP8266WebServer.h>

class JAG_webserver
{
  public:
    JAG_webserver(int port, String my_name_, bool logSerial);
    void setup();
    void runServerInLoop();
    void setupRoutes();
    ESP8266WebServer webserver;
    String get404PageContents();
    String getRootPageContents();
    String createHTMLpageWithContent_(String theContent);

       
    void addGasValueToArray(String value);
    void addTempValueToArray(String value);
    void addHumidityValueToArray(String value);

  private:
    int port_;
    bool logSerial_;


    String my_TEMP;
    String my_TEMP_history[100];
    String my_HUMIDITY;
    String my_HUMIDITY_history[100];
    String my_GAS;
    String my_GAS_history[100];
    String my_name;
    String my_lastbadge;
    bool my_showing;
    unsigned long my_lastconnect;
    
    String createJSDataForGAS();
    String createJSDataForTEMP();
    String createJSDataForHUMIDITY();  

    String createStationInfo();
    String showNiceLastConnect(unsigned long last_connect);
};

#endif
