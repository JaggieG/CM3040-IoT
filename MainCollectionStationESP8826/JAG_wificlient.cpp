#include "Arduino.h"
#include "JAG_wificlient.h"

// initialise

JAG_wificlient::JAG_wificlient(char* SSID, char* key, bool logSerial)
{
 SSID_ = SSID;
 key_ = key;
 logSerial_ = logSerial;
}

// Public Functions

void JAG_wificlient::connectToWireless() {
 // We start by connecting to a WiFi network
  if (logSerial_) {
      Serial.println();
      Serial.println();
      Serial.print("Connecting to ");
      Serial.println(SSID_);
  }
  
  // Client mode
  WiFi.mode(WIFI_STA);
  WiFi.begin(SSID_, key_);
  if (logSerial_) {
    Serial.print("Awaiting Connection");
     while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
  }

  if (logSerial_) {
    Serial.println("");
  }
  
  IP_ADDRESS = WiFi.localIP();

  if (logSerial_) {
    Serial.println("Connected to:");
    Serial.println(SSID_);
    Serial.println("with IP:");
    Serial.println(getIPAddress());

  }
}

String JAG_wificlient::getIPAddress() {
char ipAddr[16];
    snprintf(ipAddr,sizeof ipAddr,"%u.%u.%u.%u" ,(IP_ADDRESS & 0x000000ff)
                                                ,(IP_ADDRESS & 0x0000ff00) >> 8
                                                ,(IP_ADDRESS & 0x00ff0000) >> 16
                                                ,(IP_ADDRESS & 0xff000000) >> 24);
    // depending on the byte order your GetHostIp() returns the IP address in
    // you might need to reverse the above (i.e. print (ipAddress &0x000000ff)) first.
    return "%s", ipAddr;
}

char* JAG_wificlient::getConnectedSSID() {
  return SSID_;
}
