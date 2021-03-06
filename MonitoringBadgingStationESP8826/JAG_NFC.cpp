/*
 * JAG_NFC.cpp
 * Implementation file for the NFC Chip 
 */

#include "JAG_NFC.h"

/*
 * We required the chips liabry as well as SPI and Arudino library for PIN defintions
 * 
 *  RDIS-RC522, pin setup
 *  
 *  VCC = 5V or 3.3V depending on model
+-------------+-------------+------------+
| ESP8826 PIN | Sensor Name | Sensor PIN |
+-------------+-------------+------------+
| D0          | RFID        | RST        |
| D5          | RFID        | SCK        |
| D6          | RFID        | MISO       |
| D7          | RFID        | MOSI       |
| D8          | RFID        | NSS        |
+-------------+-------------+------------+
 *  
 */
 
#include "Arduino.h"
#include <SPI.h>
#include "MFRC522.h"

/* Defined th PINs that we are going to use */
#define SS_PIN D8
#define RST_PIN D0

JAG_NFC::JAG_NFC(bool logSerial) {
  MFRC522 rfid(SS_PIN, RST_PIN); // Instance of the class
  MFRC522::MIFARE_Key key;
  // Init array that will store new NUID
  byte nuidPICC[4];
  logSerial_ = logSerial;
}


//Here we run the setup of the chip and log items out to the serial console as required.
void JAG_NFC::setup() {
  SPI.begin(); // Init SPI bus
  rfid.PCD_Init(); // Init MFRC522
  if (logSerial_) {
    Serial.println();
    Serial.print(F("Reader :"));
  }

  rfid.PCD_DumpVersionToSerial();
  for (byte i = 0; i < 6; i++) {
    key.keyByte[i] = 0xFF;
  }
  if (logSerial_) {
    Serial.println();
    Serial.println(F("This code scan the MIFARE Classic NUID."));
    Serial.print(F("Using the following key:"));

  }
  printHex(key.keyByte, MFRC522::MF_KEY_SIZE);
  byte abyte = rfid.uid.uidByte[0];
  if (logSerial_) {
    Serial.print(abyte);
  }

  createArrayOfKnownCards();
  lastCardRead = "0000000";
  lastCardReadNow = millis();
}

/*
 *  This function runs via the main sketch file loop() function
 *  It checks whether there is a card in front fo the reader
 *  if so then it reads it and returns the card holder name
 *  
 *  Any actiosn that happen in the loop() wwill stop the reader picking up the card
 *  as it runs syncrohously
 * 
 * 
 */
String JAG_NFC::loop() {
  // Reset the loop if no new card present on the sensor/reader. This saves the entire process when idle.
  if (!rfid.PICC_IsNewCardPresent()) {
    return "";
  }

  // Verify if the NUID has been readed
  if (!rfid.PICC_ReadCardSerial()) {
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
      if (logSerial_) {
        Serial.println("Same card but outside threshold");
      }
      // ok the same card but it has been over the threshold so let them in
      lastCardRead = cardHex;
      lastCardReadNow = millis();
      String username = getUsernameOfCardIfKnown(cardHex);
      return username;
    } else {
      if (logSerial_) {
        Serial.println("Same card within threshold");
      }
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
void JAG_NFC::printHex(byte * buffer, byte bufferSize) {
  for (byte i = 0; i < bufferSize; i++) {
    if (logSerial_) {
      Serial.print(buffer[i] < 0x10 ? " 0" : " ");
      Serial.print(buffer[i], HEX);
    }
  }
}
/**
   Helper routine to dump a byte array as dec values to Serial.
*/
void JAG_NFC::printDec(byte * buffer, byte bufferSize) {
  for (byte i = 0; i < bufferSize; i++) {
    if (logSerial_) {
      Serial.print(buffer[i] < 0x10 ? " 0" : " ");
      Serial.print(buffer[i], DEC);
    }
  }
}

String JAG_NFC::getUsernameOfCardIfKnown(String cardHex) {
  String username = cardHex;

  for (int i = 0; i < numberOfKnownCards; i++) {
    String thisKnownCard = arrayOfKnownCards[i];
    String only_cardHex = arrayOfKnownCards[i].substring(0, arrayOfKnownCards[i].indexOf("_"));
    String only_username = arrayOfKnownCards[i].substring(arrayOfKnownCards[i].indexOf("_") + 1);
    if (cardHex == only_cardHex) {
      username = only_username;
    }
  }

  return username;
}

// Details of the cards
void JAG_NFC::createArrayOfKnownCards() {
  arrayOfKnownCards[0] = "97e98d62_John Doe";
  arrayOfKnownCards[1] = "b3339c0d_Jane Doe";
  arrayOfKnownCards[2] = "93800317_Anonymous Person";
}

String JAG_NFC::stringHex(byte * buffer, byte bufferSize) {
  String result = "";
  String hexstring = "";
  for (byte i = 0; i < bufferSize; i++) {
    if (buffer[i] < 0x10) {
      hexstring += '0';
    }
    hexstring += String(buffer[i], HEX);
  }

  return hexstring;
}
