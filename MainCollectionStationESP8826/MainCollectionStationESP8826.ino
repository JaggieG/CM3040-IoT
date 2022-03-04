/* ALWAYS RUNNING SERVER */
/* Import the required modules */
#include "JAG_wificlient.h"
#include "JAG_webserver.h"
#include "JAG_NFC.h"
#include "JAG_httpclient.h"
// for the cron

const unsigned long eventInterval = 10 * 1000; // every thirty seconds
unsigned long previousTime = 0;

// create the requried objects using my libraries

JAG_wificlient wifi_client = JAG_wificlient("CHALETEMMANUEL","LOCKEDDOWN", true);
JAG_webserver webserver = JAG_webserver(80, true);
JAG_NFC NFC_reader = JAG_NFC(true);
JAG_httpclient httpClient = JAG_httpclient(true, "192.168.1.139");

void setup() {
  Serial.begin(115200);

  // setup WIFi
  wifi_client.connectToWireless();
  
  // setup the NFS library
  NFC_reader.setup();
  
  // setup Web Server
  webserver.setup();

}

// The main loop 
void loop() {
  // setup and web server the respond to web requests in the loop
  webserver.runServerInLoop();
  
  // Make sure that the NFC reader is watching for new badge
  String cardHexFound = NFC_reader.loop();
  //looks like a card was badged, we should tell the remote station
  if (cardHexFound != "") {
     Serial.println("Card Hex is: " + cardHexFound );
     httpClient.sendCardValueToRemoteStation(cardHexFound);
  }
  
  //run the cron job to update the details to the remote station
  cronJob();
}

void cronJob() {
  unsigned long currentTime = millis();

  if (currentTime - previousTime >= eventInterval) {
    Serial.println("Sending values to the Remote Station");
    httpClient.sendAllValues("10", "5");
    NFC_reader.resetLastSeenBadge();
    previousTime = currentTime;
  }

  
}

/* ---------------------------------------------------------------------------------------------------*/
