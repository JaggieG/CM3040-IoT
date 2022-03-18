/*
 * This sketch if for the Remote Monitroing stations
 * 
 * HARDWARE dependancies
 * 
 * RFID sensor plugged in via Serial
 * A Gas Sensor plugged in via ANALOG
 * A DHT sensor Plugged in via DIGITAL
 */

/* 
 *   Notes to marker
 *  
 *  When the sketch is launched it required that the client borwser has internet access to
 *  to get access to some online CDNs for certain javascript libraries.  
 *  
 *  If the sensor is 5V then they will need to powered via difference power supply rather than the 3.3V that comes out of the ESP8826
 *  
 *  DHT22 sensor should be plugged into PIN D2
 *  Gas sensor should be plugged into PIN A0
 *  
 *  RDIS-RC522, pin setup
 *  
 *  VCC = 5V or 3.3V depending on model
 * 
+-------------+-------------+------------+
| ESP8826 PIN | Sensor Name | Sensor PIN |
+-------------+-------------+------------+
| A0          | Gas         | Out        |
| D0          | RFID        | RST        |
| D2          | DHT         | Out        |
| D5          | RFID        | SCK        |
| D6          | RFID        | MISO       |
| D7          | RFID        | MOSI       |
| D8          | RFID        | NSS        |
+-------------+-------------+------------+
 *  
 */

/* MODULES / LIBRIAIES
 *  
 * Here I import the libraries that I have made to keep my ino sketch clean.
 * Also any thirs parties libraries needed for my sketch
*/

#include "JAG_wificlient.h"
#include "JAG_webserver.h"
#include "JAG_NFC.h"
#include "JAG_gas.h"
#include "JAG_httpclient.h"
#include "DHT.h"

/* EDITABLE GLOBAL VARIABLES
 *  
 * logSerial = should we log details to the serial port
 * station_id = the id the station will have when it send the data to the data collector - this should be unique and can be 1,2 or 3
 * station_name = this is the name the station will show on it's web interface and that of the collector dashboard.
 * http_server_port = the port that the http server will run on (normally 80) - change for security.
 * wirelessSSID = the SSID of the wireless network the micro controller should join.
 * wirelessKey = the key of the wireless netwok that the micro controller sould join.
 * GASPIN = the analog pin for the gas sensor
 * DHTPIN = the digital pin for the DHT sensor
 * DHTTYPE = the type of the DHT (DHT22 / DHT11)
*/

bool logSerial = true;
String station_id = "1";
String station_name = "My Desk Left";
char* wirelessSSID = "CHALETEMMANUEL";
char* wirelessKey = "LOCKEDDOWN";
int http_server_port = 80;
String remote_server_ip = "192.168.1.133";

#define GASPIN A0 // Analog pin for the gas sensor
#define DHTPIN D4 // what digital pin the DHT22 is connected to
#define DHTTYPE DHT22 // there are multiple kinds of DHT sensors

/* END EDITABLE GLOBAL VARIABLES */

/* As we don't want to run certain jobs at every iteration of the loop() function 
 * This should not be set too low in order to avoid overloading the micro controller
*/
const unsigned long eventInterval = 30 * 1000; // every thirty seconds
unsigned long previousTime = 0;
int dots = 0;

/* Module Variables
 *  
 * Each of the libaries / modules that I have developed
 * are import here
*/

JAG_wificlient wifi_client = JAG_wificlient(wirelessSSID, wirelessKey, logSerial);
JAG_webserver webserver = JAG_webserver(http_server_port, station_name, logSerial);
JAG_NFC NFC_reader = JAG_NFC(logSerial);
JAG_httpclient httpClient = JAG_httpclient(remote_server_ip, logSerial, station_id, station_name);
JAG_gas gas_sensor = JAG_gas(GASPIN, logSerial);

/* DHT sensor info */

DHT dht(DHTPIN, DHTTYPE);




/* Main Setup function
 *  
 * This is loaded as the Micro controller starts up.
 * It sets up the controller with everything it needs to run correctly
*/
void setup() {
  Serial.begin(115200); // CHANGE as needed
  while (!Serial) {}// Wait for serial to initialize.

  // Use the library to connect to the wireless
  wifi_client.connectToWireless();

  // setup the NFC library
  NFC_reader.setup();

  // setup Web Server
  webserver.setup();

  // Start using the DHT sensor
  dht.begin();

  //Setup the gas sensor
  gas_sensor.setupGasSensor();

}

// The main loop 

void loop() {
   printDots(); // So we know that we are alive and waiting.

   // Make sure that the NFC reader is watching for new badge
  String cardUsername = NFC_reader.loop();
    
  //looks like a card was badged, we should tell the remote station
  if (cardUsername != "") {
    Serial.println("Card Username is: " + cardUsername);
    httpClient.sendCardValueToRemoteStation(cardUsername);
    webserver.updateLocalLastBadged(cardUsername); 
    dots = 0;
  }
    // setup and web server the respond to web requests in the loop
    webserver.runServerInLoop();
    
    //run the cron job to update the details to the remote station
    cronJob();
}

/*
 * Helper funtions
 */

// this function prints dots on the serial output show you can see the controller is working.
void printDots() {
  int maxDots = 20;
  String dotsString = "";
  for (int i=0; i < dots; i++) {
        dotsString += ".";
  }
 Serial.println(dotsString);
 if (maxDots > dots) {
  dots++;
 } else {
  dots = 0;
 }
 
}

// this function is launched by the loop() function and will only run at the internal specified in the global settings.
void cronJob() {
  unsigned long currentTime = millis();
  if (currentTime - previousTime >= eventInterval) {
    // Read the Humiidyt from the DHT sensor
    int h = dht.readHumidity();
    
    // Read temperature in Celsius from the DHT sensor
    int t = dht.readTemperature();

    // Check if any reads failed and exit early (to try again).
    if (isnan(h) || isnan(t)) {
      Serial.println("Failed to read from DHT sensor!");
    } else {
     // add the values to the arrays for history
      webserver.addTempValueToArray(String(t));
      webserver.addHumidityValueToArray(String(h));
      // send the details to the agreated dashboard controller
      httpClient.sendAllValues(String(t), String(h));
    }
    
    // Read the value from the gas sensor and if it is >0 then send that the the aggreated dashbaord
    int gas_sensor_reading = gas_sensor.getGasValue();
    if (gas_sensor_reading > 0) {
        webserver.addGasValueToArray(String(gas_sensor_reading));
      httpClient.sendGasSensorValueToRemoteStation(String(gas_sensor_reading));
    }

    // set our last run time
    previousTime = currentTime;
  }
}
