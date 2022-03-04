

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
    void resetLastSeenBadge();
  private:
    bool logSerial_;
    MFRC522 rfid;
    MFRC522::MIFARE_Key key;
  // Init array that will store new NUID
  byte nuidPICC[4];
    void printHex(byte *buffer, byte bufferSize);
    void printDec(byte *buffer, byte bufferSize);
    String stringHex(byte *buffer, byte bufferSize);
    String HexString2ASCIIString(String hexstring);
};

#endif
