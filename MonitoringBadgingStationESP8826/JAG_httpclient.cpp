#include "Arduino.h"
#include "JAG_httpclient.h"
#include <WiFiClient.h>
#include <ArduinoJson.h>

// initialise

JAG_httpclient::JAG_httpclient(bool logSerial, String remoteStationIP)
{
  HTTPClient http;  //Declare an object of class HTTPClient
  logSerial_ = logSerial;
  remoteStationIP_ = remoteStationIP;
  localIdent_ = "1";
  localStationName_ = "My Desk";
  WiFiClient wifiClient;

}

// Public Functions

void JAG_httpclient::sendAllValues(String Temperature, String Humidity) {
  sendLocalTemperatureValuesToRemoteStation(Temperature);
  sendLocalHumidityValuesToRemoteStation(Humidity);
}


void JAG_httpclient::sendLocalTemperatureValuesToRemoteStation(String Temperature) {
     String remote_server_address = "http://"  + remoteStationIP_ + "/update";
      StaticJsonDocument<200> doc;
      doc["station_id"] = localIdent_;
      doc["station_name"] = localStationName_;
      doc["sensor_type"] = "TEMP";
      doc["sensor_value"] = Temperature;
      
    String json;
    serializeJson(doc, json);

    
  
    Serial.println(json); 
    Serial.println(remote_server_address);

    int httpCode = http.POST(json); 
    
    http.addHeader("Content-Type", "text/plain");
    if (httpCode > 0) { //Check the returning code
      String payload = http.getString();   //Get the request response payload
      Serial.println(payload);             //Print the response payload
    }
    http.end();   //Close connection
}

void JAG_httpclient::sendLocalHumidityValuesToRemoteStation(String Humidity) {
    String remote_server_address = "http://"  + remoteStationIP_ + "/update";
    StaticJsonDocument<200> doc;
      doc["station_id"] = localIdent_;
      doc["station_name"] = localStationName_;
      doc["sensor_type"] = "HUMIDITY";
      doc["sensor_value"] = Humidity;
      
    String json;
    serializeJson(doc, json);
    
    Serial.println(json); 
    Serial.println(remote_server_address); 
    
    http.begin(wifiClient, remote_server_address);  //Specify request destination
    
    int httpCode = http.POST(json); 
    
    http.addHeader("Content-Type", "application/json");
    if (httpCode > 0) { //Check the returning code
      String payload = http.getString();   //Get the request response payload
      Serial.println(payload);             //Print the response payload
    }
    http.end();   //Close connection
}

void JAG_httpclient::sendAggregateCardValueToRemoteStation(String username) {
  String remote_server_address = "http://"  + remoteStationIP_ + "/userBadged";

     StaticJsonDocument<200> doc;
      doc["station_id"] = localIdent_;
      doc["station_name"] = localStationName_;
      doc["username"] = username;
      
    String json;
    serializeJson(doc, json);

    Serial.println(remote_server_address); 
    Serial.println(username);
    http.begin(wifiClient, remote_server_address);  //Specify request destination
    
    int httpCode = http.POST(json); 
    
    http.addHeader("Content-Type", "text/plain");
    if (httpCode > 0) { //Check the returning code
      String payload = http.getString();   //Get the request response payload
      Serial.println(payload);             //Print the response payload
    }
    http.end();   //Close connection

}

void JAG_httpclient::sendLastUpdateCardValueToRemoteStation(String username) {
      String remote_server_address = "http://"  + remoteStationIP_ + "/update";
   
    StaticJsonDocument<200> doc;
      doc["station_id"] = localIdent_;
      doc["station_name"] = localStationName_;
      doc["sensor_type"] = "BADGE";
      doc["sensor_value"] = username;
      
    String json;
    serializeJson(doc, json);
    Serial.println(remote_server_address); 
    Serial.println(username);
    http.begin(wifiClient, remote_server_address);  //Specify request destination
    
    int httpCode = http.POST(json); 
    
    http.addHeader("Content-Type", "text/plain");
    if (httpCode > 0) { //Check the returning code
      String payload = http.getString();   //Get the request response payload
      Serial.println(payload);             //Print the response payload
    }
    http.end();   //Close connection

}

void JAG_httpclient::sendGasSensorValueToRemoteStation(String gasValue) {
      String remote_server_address = "http://"  + remoteStationIP_ + "/update";
    String identString = localIdent_ + "_" + localStationName_ + "_" +  "GAS_" + gasValue + "_END";
  
    Serial.println(remote_server_address); 
    Serial.println(gasValue);
    http.begin(wifiClient, remote_server_address);  //Specify request destination
    
    int httpCode = http.POST(identString); 
    
    http.addHeader("Content-Type", "text/plain");
    if (httpCode > 0) { //Check the returning code
      String payload = http.getString();   //Get the request response payload
      Serial.println(payload);             //Print the response payload
    }
    http.end();   //Close connection

}



void JAG_httpclient::sendCardValueToRemoteStation(String username) {
  sendAggregateCardValueToRemoteStation(username);
  sendLastUpdateCardValueToRemoteStation(username);
}
