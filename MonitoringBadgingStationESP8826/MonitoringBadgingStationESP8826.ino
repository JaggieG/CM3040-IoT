/* Import the required modules */
#include "JAG_wificlient.h"
#include "JAG_webserver.h"
#include "JAG_NFC.h"
#include "JAG_gas.h"
#include "JAG_httpclient.h"
#include "DHT.h"

// for the cron
const unsigned long eventInterval = 10 * 1000; // every thirty seconds
unsigned long previousTime = 0;

// create the requried objects using my libraries

/* Change the remote server IP address here */
String remote_server_ip = "192.168.1.139";

JAG_wificlient wifi_client = JAG_wificlient("CHALETEMMANUEL","LOCKEDDOWN", true);
JAG_webserver webserver = JAG_webserver(80, true);
JAG_NFC NFC_reader = JAG_NFC(true);
JAG_httpclient httpClient = JAG_httpclient(true, remote_server_ip);


JAG_gas gas_sensor = JAG_gas(A0, 600, true);


#define DHTPIN 4     // what digital pin the DHT22 is conected to
#define DHTTYPE DHT11   // there are multiple kinds of DHT sensors

DHT dht(DHTPIN, DHTTYPE);

void setup() {
  Serial.begin(115200);
   // Wait for serial to initialize.
  while(!Serial) { }
  
  // setup WIFi
  wifi_client.connectToWireless();
  
  // setup the NFS library
  NFC_reader.setup();
  
  // setup Web Server
  webserver.setup();
  
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
     Serial.println("Card Username is: " + cardUsername );
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
      httpClient.sendAllValues(String(t), String(h));
    }



    int gas_sensor_reading = gas_sensor.getGasValue();
    if (gas_sensor_reading > 0) {
      httpClient.sendGasSensorValueToRemoteStation(String(gas_sensor_reading));
    }
      
    previousTime = currentTime;
  }


}

/* ---------------------------------------------------------------------------------------------------*/
