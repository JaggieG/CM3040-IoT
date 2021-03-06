
/*
 *  JAG_httpclient = library to handle making http request to the remote services
 * 
 */
#include "Arduino.h"
#include "JAG_httpclient.h"
#include <WiFiClient.h>
#include <ArduinoJson.h>

// initialise

JAG_httpclient::JAG_httpclient(String remoteStationIP,bool logSerial)
{
  HTTPClient http;  //Declare an object of class HTTPClient
  logSerial_ = logSerial;
  
  remoteStationIP_ = remoteStationIP;
  
  /*
   *  Make sure you change the name and ID 
   * of the station here so that appears as expected on
   * the dashboards
   * 
   */
  localIdent_ = "3";
  localStationName_ = "Main Station";
  
  WiFiClient wifiClient;

}

// Public Functions

// helper function to send all DHT values
void JAG_httpclient::sendAllValues(String Temperature, String Humidity) {
  sendLocalTemperatureValuesToRemoteStation(Temperature);
  sendLocalHumidityValuesToRemoteStation(Humidity);
}

/*
 *  Creating a JSON value with the JSON libary to send to the central monitoring station
 * 
 */
void JAG_httpclient::sendLocalTemperatureValuesToRemoteStation(String Temperature) {
     String remote_server_address = "http://"  + remoteStationIP_ + "/update";
      StaticJsonDocument<200> doc;
      doc["station_id"] = localIdent_;
      doc["station_name"] = localStationName_;
      doc["sensor_type"] = "TEMP";
      doc["sensor_value"] = Temperature;
      
    String json;
    serializeJson(doc, json);

    
   if (logSerial_) {
    Serial.println(json); 
    Serial.println(remote_server_address);
   }

    int httpCode = http.POST(json); 
    
    http.addHeader("Content-Type", "application/json");
    if (httpCode > 0) { //Check the returning code
      String payload = http.getString();   //Get the request response payload
      Serial.println(payload);             //Print the response payload
    }
    http.end();   //Close connection
}

/*
 *  Creating a JSON value with the JSON libary to send to the central monitoring station
 * 
 */
void JAG_httpclient::sendLocalHumidityValuesToRemoteStation(String Humidity) {
    String remote_server_address = "http://"  + remoteStationIP_ + "/update";
    StaticJsonDocument<200> doc;
      doc["station_id"] = localIdent_;
      doc["station_name"] = localStationName_;
      doc["sensor_type"] = "HUMIDITY";
      doc["sensor_value"] = Humidity;
      
    String json;
    serializeJson(doc, json);
     if (logSerial_) {
    Serial.println(json); 
    Serial.println(remote_server_address); 
     }
    
    http.begin(wifiClient, remote_server_address);  //Specify request destination
    
    int httpCode = http.POST(json); 
    
    http.addHeader("Content-Type", "application/json");
    if (httpCode > 0) { //Check the returning code
      String payload = http.getString();   //Get the request response payload
       if (logSerial_) {
      Serial.println(payload);             //Print the response payload
       }
    }
    http.end();   //Close connection
}

/*
 *  Creating a JSON value with the JSON libary to send to the central monitoring station
 * 
 */
void JAG_httpclient::sendAggregateCardValueToRemoteStation(String username) {
  String remote_server_address = "http://"  + remoteStationIP_ + "/userBadged";

     StaticJsonDocument<200> doc;
      doc["station_id"] = localIdent_;
      doc["station_name"] = localStationName_;
      doc["username"] = username;
      
    String json;
    serializeJson(doc, json);
 if (logSerial_) {
    Serial.println(remote_server_address); 
    Serial.println(json);
 }
    http.begin(wifiClient, remote_server_address);  //Specify request destination
    
    int httpCode = http.POST(json); 
    
    http.addHeader("Content-Type", "application/json");
    if (httpCode > 0) { //Check the returning code
      String payload = http.getString();   //Get the request response payload
       if (logSerial_) {
      Serial.println(payload);             //Print the response payload
       }
    }
    http.end();   //Close connection

}

/*
 *  Creating a JSON value with the JSON libary to send to the central monitoring station
 * 
 */
void JAG_httpclient::sendLastUpdateCardValueToRemoteStation(String username) {
      String remote_server_address = "http://"  + remoteStationIP_ + "/update";
   
    StaticJsonDocument<200> doc;
      doc["station_id"] = localIdent_;
      doc["station_name"] = localStationName_;
      doc["sensor_type"] = "BADGE";
      doc["sensor_value"] = username;
      
    String json;
    serializeJson(doc, json);
     if (logSerial_) {
    Serial.println(remote_server_address); 
    Serial.println(json);
     }
    http.begin(wifiClient, remote_server_address);  //Specify request destination
    
    int httpCode = http.POST(json); 
    
    http.addHeader("Content-Type", "application/json");
    if (httpCode > 0) { //Check the returning code
      String payload = http.getString();   //Get the request response payload
       if (logSerial_) {
          Serial.println(payload);             //Print the response payload
       }
    }
    http.end();   //Close connection

}

/*
 *  Creating a JSON value with the JSON libary to send to the central monitoring station
 * 
 */
void JAG_httpclient::sendGasSensorValueToRemoteStation(String gasValue) {
      String remote_server_address = "http://"  + remoteStationIP_ + "/update";
  
      StaticJsonDocument<200> doc;
      doc["station_id"] = localIdent_;
      doc["station_name"] = localStationName_;
      doc["sensor_type"] = "GAS";
      doc["sensor_value"] = gasValue;

  String json;
    serializeJson(doc, json);
   
     if (logSerial_) {
        Serial.println(remote_server_address); 
        Serial.println(json);
     }
    http.begin(wifiClient, remote_server_address);  //Specify request destination
    
    int httpCode = http.POST(json); 
    
    http.addHeader("Content-Type", "application/json");
    if (httpCode > 0) { //Check the returning code
      String payload = http.getString();   //Get the request response payload
       if (logSerial_) {
         Serial.println(payload);             //Print the response payload
       }
    }
    http.end();   //Close connection

}



void JAG_httpclient::sendCardValueToRemoteStation(String username) {
  sendAggregateCardValueToRemoteStation(username);
  sendLastUpdateCardValueToRemoteStation(username);
}
