#include "Arduino.h"

#include "JAG_NFC.h"
#include <SPI.h>
#include "MFRC522.h"

#define SS_PIN D8
#define RST_PIN D0

JAG_NFC::JAG_NFC(bool logSerial )
{
  MFRC522 rfid(SS_PIN, RST_PIN); // Instance of the class
  MFRC522::MIFARE_Key key;
  // Init array that will store new NUID
  byte nuidPICC[4];
  logSerial_ = logSerial;
}

// Public Functions

void JAG_NFC::setup() {
   SPI.begin(); // Init SPI bus
 rfid.PCD_Init(); // Init MFRC522
 Serial.println();
 Serial.print(F("Reader :"));
 rfid.PCD_DumpVersionToSerial();
 for (byte i = 0; i < 6; i++) {
   key.keyByte[i] = 0xFF;
 }
 Serial.println();
 Serial.println(F("This code scan the MIFARE Classic NUID."));
 Serial.print(F("Using the following key:"));
 printHex(key.keyByte, MFRC522::MF_KEY_SIZE);
 byte abyte = rfid.uid.uidByte[0];
 Serial.print(abyte);

  createArrayOfKnownCards();
 lastCardRead = "0000000";
 lastCardReadNow = millis();
}


String JAG_NFC::loop() {
    // Reset the loop if no new card present on the sensor/reader. This saves the entire process when idle.
   if ( ! rfid.PICC_IsNewCardPresent()) {
     return "";
   }
  
   // Verify if the NUID has been readed
   if ( ! rfid.PICC_ReadCardSerial()) {
     return "";
   }
  
 
  MFRC522::PICC_Type piccType = rfid.PICC_GetType(rfid.uid.sak);
 
 
   // Check is the PICC of Classic MIFARE type
   if (piccType != MFRC522::PICC_TYPE_MIFARE_MINI &&
       piccType != MFRC522::PICC_TYPE_MIFARE_1K &&
       piccType != MFRC522::PICC_TYPE_MIFARE_4K) {
     //Serial.println(F("Your tag is not of type MIFARE Classic."));
     return "";
   }
   
   
  String cardHex = stringHex(rfid.uid.uidByte, rfid.uid.size);
  
  if (lastCardRead == cardHex) {
    // this is the same badge, no cheating.
     if (millis() - lastCardReadNow >= threshold) {
      Serial.println("Same card but outside threshold");
      // ok the same card but it has been over the threshold so let them in
      lastCardRead = cardHex;
      lastCardReadNow = millis();
      String username = getUsernameOfCardIfKnown(cardHex);
      return username;
     } else {
       //Serial.println("Same card within threshold");
       return "";
     }
  } else {
     lastCardRead = cardHex;
     lastCardReadNow = millis();
     String username = getUsernameOfCardIfKnown(cardHex);
     return username;
  }
  

 // Halt PICC
 rfid.PICC_HaltA();
 // Stop encryption on PCD
 rfid.PCD_StopCrypto1();
}

/**
   Helper routine to dump a byte array as hex values to Serial.
*/
void JAG_NFC::printHex(byte *buffer, byte bufferSize) {
 for (byte i = 0; i < bufferSize; i++) {
   Serial.print(buffer[i] < 0x10 ? " 0" : " ");
   Serial.print(buffer[i], HEX);
 }
}
/**
   Helper routine to dump a byte array as dec values to Serial.
*/
void JAG_NFC::printDec(byte *buffer, byte bufferSize) {
 for (byte i = 0; i < bufferSize; i++) {
   Serial.print(buffer[i] < 0x10 ? " 0" : " ");
   Serial.print(buffer[i], DEC);
 }
}


String JAG_NFC::getUsernameOfCardIfKnown(String cardHex) {
String username = cardHex;

  for (int i=0; i < numberOfKnownCards; i++) { 
    String thisKnownCard = arrayOfKnownCards[i];
    String only_cardHex = arrayOfKnownCards[i].substring(0,arrayOfKnownCards[i].indexOf("_"));
    String only_username = arrayOfKnownCards[i].substring(arrayOfKnownCards[i].indexOf("_") + 1);
    if (cardHex == only_cardHex) {
     username = only_username;
    }
  }


  return username;
}


void JAG_NFC::createArrayOfKnownCards() {
  arrayOfKnownCards[0] = "97e98d62_John Doe";
  arrayOfKnownCards[1] = "b3339c0d_Jane Doe";
  arrayOfKnownCards[2] = "93800317_Anonymous Person";
}

String JAG_NFC::stringHex(byte *buffer, byte bufferSize) {
  String result = "";
  String hexstring = "";
 for (byte i = 0; i < bufferSize; i++) {
    if(buffer[i] < 0x10) {
      hexstring += '0';
    }
    hexstring += String(buffer[i], HEX);
  }

  return hexstring;
}
