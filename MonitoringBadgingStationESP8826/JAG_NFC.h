

#ifndef JAG_NFC_h
#define JAG_NFC_h

#include "Arduino.h"
#include "MFRC522.h"

class JAG_NFC
{
  public:
    JAG_NFC(bool logSerial);
    void setup();
    String loop();
  private:
    bool logSerial_;
    MFRC522 rfid;
    MFRC522::MIFARE_Key key;

    byte nuidPICC[4];
    void printHex(byte *buffer, byte bufferSize);
    void printDec(byte *buffer, byte bufferSize);
    String stringHex(byte *buffer, byte bufferSize);
    String HexString2ASCIIString(String hexstring);
    String lastCardRead;
    unsigned long lastCardReadNow;
    unsigned long threshold = 10 * 1000; // every x seconds
    int numberOfKnownCards = 3;
    String arrayOfKnownCards[3];
    
    void createArrayOfKnownCards();
    String getUsernameOfCardIfKnown(String cardHex);
};

#endif
