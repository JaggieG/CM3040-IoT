/*
This sketch if for the Remote Monitroing stations
It relies on a 
  RFID sensor plugged in via Serial
  A Gas Sensor plugged in via ANALOG
  A DHT sensor Plugged in via DIGITAL
*/


/* Import the required modules */ 

#include "JAG_wificlient.h"
#include "JAG_webserver.h"
#include "JAG_NFC.h"
#include "JAG_gas.h"
#include "JAG_httpclient.h"
#include "DHT.h"

/* 
  MARKER

  Please change the IP address of the Dashboard collector here.
  this is where the data is sent

*/
String remote_server_ip = "192.168.1.139";

/* As we don't want to run certain jobs at every iteration of the loop() function  */
const unsigned long eventInterval = 5 * 1000; // every thirty seconds
unsigned long previousTime = 0;

/* should we log details to the serial port? */
bool logSerial = true;
String station_id = "1";
String station_name = "My Desk Right";

char* wirelessSSID = "CHALETEMMANUEL";
char* wirelessKey = "LOCKEDDOWN";
int http_server_port = 80;

// create the required objects using my libraries
JAG_wificlient wifi_client = JAG_wificlient(wirelessSSID, wirelessKey, logSerial);
JAG_webserver webserver = JAG_webserver(http_server_port, station_name, logSerial);
JAG_NFC NFC_reader = JAG_NFC(logSerial);
JAG_httpclient httpClient = JAG_httpclient(remote_server_ip, logSerial, station_id, station_name);
JAG_gas gas_sensor = JAG_gas(A0, 600, logSerial);

/* DHT sensor info */
#define DHTPIN 4 // what digital pin the DHT22 is conected to
#define DHTTYPE DHT22 // there are multiple kinds of DHT sensors

DHT dht(DHTPIN, DHTTYPE);

/* Main IoT Setup */
void setup() {
  Serial.begin(115200);
  // Wait for serial to initialize.
  while (!Serial) {}

  // setup WIFi
  wifi_client.connectToWireless();

  // setup the NFS library
  NFC_reader.setup();

  // setup Web Server
  webserver.setup();

  // Strat using the DT sensor
  dht.begin();

  //Setup the gas sensor
  gas_sensor.setupGasSensor();

}

// The main loop 

void loop() {
  // setup and web server the respond to web requests in the loop
  webserver.runServerInLoop();

  // Make sure that the NFC reader is watching for new badge
  String cardUsername = NFC_reader.loop();
  //looks like a card was badged, we should tell the remote station
  if (cardUsername != "") {
    Serial.println("Card Username is: " + cardUsername);
    httpClient.sendCardValueToRemoteStation(cardUsername);
  }

  //run the cron job to update the details to the remote station
  cronJob();
}

void cronJob() {
  unsigned long currentTime = millis();

  if (currentTime - previousTime >= eventInterval) {

    int h = dht.readHumidity();
    // Read temperature as Celsius (the default)
    int t = dht.readTemperature();

    // Check if any reads failed and exit early (to try again).
    if (isnan(h) || isnan(t)) {
      Serial.println("Failed to read from DHT sensor!");
    } else {
      webserver.addTempValueToArray(String(t));
      webserver.addHumidityValueToArray(String(h));
      
      httpClient.sendAllValues(String(t), String(h));
    }

    int gas_sensor_reading = gas_sensor.getGasValue();
    if (gas_sensor_reading > 0) {
        webserver.addGasValueToArray(String(gas_sensor_reading));
      httpClient.sendGasSensorValueToRemoteStation(String(gas_sensor_reading));
    }

    previousTime = currentTime;
  }

}

/* ---------------------------------------------------------------------------------------------------*/
