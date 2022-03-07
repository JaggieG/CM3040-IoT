/* ALWAYS RUNNING SERVER */
/* Import the required modules */
#include "JAG_wificlient.h"
#include "JAG_webserver.h"
#include "JAG_httpclient.h"
#include "DHT.h"

/* 
  MARKER

  Please change the IP address of the Dashboard collector here.
  this is where the data is sent

*/
String remote_server_ip = "127.0.0.1"; // loopback for local connection



/* As we don't want to run certain jobs at every iteration of the loop() function  */
const unsigned long eventInterval = 30 * 1000; // every thirty seconds
unsigned long previousTime = 0;

/* should we log details to the serial port? */
bool logSerial = true;

// create the requried objects using my libraries

JAG_wificlient wifi_client = JAG_wificlient("CHALETEMMANUEL","LOCKEDDOWN", logSerial);
JAG_webserver webserver = JAG_webserver(80, logSerial);
JAG_httpclient httpClient = JAG_httpclient(remote_server_ip, logSerial);

/* DHT sensor info */
#define DHTPIN 4 // what digital pin the DHT22 is conected to
#define DHTTYPE DHT22 // there are multiple kinds of DHT sensors

DHT dht(DHTPIN, DHTTYPE);


void setup() {
  Serial.begin(115200);
 // Wait for serial to initialize.
  while (!Serial) {}
  
  // setup WIFi
  wifi_client.connectToWireless();
  
   // Strat using the DT sensor
  dht.begin();

  
  // setup Web Server
  webserver.setup();

}

// The main loop 
void loop() {
  // setup and web server the respond to web requests in the loop
  webserver.runServerInLoop();
  
  
  //run the cron job to update the details to the remote station
  cronJob();
}

void cronJob() {
  unsigned long currentTime = millis();

  if (currentTime - previousTime >= eventInterval) {
    Serial.println("Sending values to the Remote Station");

     int h = dht.readHumidity();
    // Read temperature as Celsius (the default)
    int t = dht.readTemperature();

    // Check if any reads failed and exit early (to try again).
    if (isnan(h) || isnan(t)) {
      Serial.println("Failed to read from DHT sensor!");
    } else {
      httpClient.sendAllValues(String(t), String(h));
    }

    
    previousTime = currentTime;
  }
}

/* ---------------------------------------------------------------------------------------------------*/
