#include <emulatetag.h>
#include <NdefMessage.h>
#include <SPI.h>
#include <PN532_SPI.h>
#include <PN532.h>
#include <NfcAdapter.h>

PN532_SPI pn532spi(SPI, 10);
EmulateTag nfc(pn532spi);
bool debug = true;
int ledRed = 4;
int ledGreen = 2;

uint8_t mockAuthentication[25] = { 0x02, 0x65, 0x6E, 0x61, 0x75, 0x74, 0x68, 0x65, 0x6E, 0x74, 0x69, 0x63, 0x61, 0x74, 0x69, 0x6F, 0x6E, 0x52, 0x65, 0x73, 0x70, 0x6F, 0x6E, 0x73, 0x65 }; // .enauthenticationResponse
uint8_t ndefBuf[120];
int messageSize;
   
void setup() {

  pinMode(ledRed, OUTPUT);
  pinMode(ledGreen, OUTPUT);

  Serial.begin(115200);
  if (debug) {
    Serial.println("------- Set Emulate Tag --------");
  }

  resetNdef();
  nfc.init(); 
}

void loop() {
  nfc.emulate();

  if (nfc.writeOccured()) {

    uint8_t* tag_buf;
    uint16_t length;
  
    nfc.getContent(&tag_buf, &length);
    NdefMessage msg = NdefMessage(tag_buf, length);

    // msg.getRecord(0).print();
    NdefRecord record = msg.getRecord(0);
    
    
    int payloadLength = record.getPayloadLength();
    byte payload[payloadLength];
    record.getPayload(payload);
    
    String payloadAsString = "";
    for (int c = 0; c < payloadLength; c++) {
      payloadAsString += (char)payload[c];
    }

    String uid = record.getId();
    if (uid != "") {
    Serial.print("  ID: ");Serial.println(uid);
    }
    if (debug) {
    Serial.println("\nWrite occurred!");
    Serial.print("  TNF: ");Serial.println(record.getTnf());
    Serial.print("  Type: ");Serial.println(record.getType());
    // Print the Hex and Printable Characters
    Serial.print("  Payload (HEX): ");
    PrintHexChar(payload, payloadLength);
    Serial.print("  Payload (as String): ");
    Serial.println(payloadAsString);
    }

    if (payloadAsString == mockAuthentication) {
      successResponse();
      // resetNdef();
    } else {
      failResponse();
    }
      resetNdef();

    delay(2000);
    if (debug) {
      Serial.println("\nBack to read mode");    
    }
  }

  delay(1000);
}

void successResponse() {
    Serial.print("\n===============================================");
    Serial.print("\n============ Compra autoritzada! ==============");
    Serial.print("\n===============================================");

    // Light green led 5 seconds
    
    digitalWrite(ledGreen, HIGH);
    delay(1000);
    digitalWrite(ledGreen, LOW);
}

void failResponse() {
    Serial.print("\n===============================================");
    Serial.print("\n=================  No CAFE!  ==================");
    Serial.print("\n===============================================");
    // Blink red led 1 second
    digitalWrite(ledRed, HIGH);
    delay(1000);
    digitalWrite(ledRed, LOW);
}

void resetNdef() {  
  NdefMessage message;
  NdefRecord newNdef;  
  // Set ndef lenght
  // "{"uid":"333","val":1,"msg":"valid validation"}"
  uint8_t newPayload[46] = { 0x7B, 0x22, 0x75, 0x69, 0x64, 0x22, 0x3A, 0x22, 0x33, 0x33, 0x33, 0x22, 0x2C, 0x22, 0x76, 0x61, 0x6C, 0x22, 0x3A, 0x31, 0x2C, 0x22, 0x6D, 0x73, 0x67, 0x22, 0x3A, 0x22, 0x76, 0x61, 0x6C, 0x69, 0x64, 0x20, 0x76, 0x61, 0x6C, 0x69, 0x64, 0x61, 0x74, 0x69, 0x6F, 0x6E, 0x22, 0x7D }; 
  uint8_t type[] = { 'T', 'E', 'X', 'T' };  // "TEXT"
  uint8_t id[] = { 0x28, 0x5E, 0x2D, 0x5E, 0x29 };  // (^-^)

  newNdef.setTnf(TNF_WELL_KNOWN);
  newNdef.setId(id, sizeof(id));
  newNdef.setType(type, sizeof(type));
  newNdef.setPayload(newPayload, sizeof(newPayload));

  message = NdefMessage();
  if (debug) {
    Serial.print("\nmessage: ");
    message.print();
  }

  message.addRecord(newNdef);
  messageSize = message.getEncodedSize();
  if (messageSize > sizeof(ndefBuf)) {
    Serial.println("ndefBuf is too small");
    while (1) {}
  }

  if (debug) {
    Serial.print("Ndef encoded message size: ");
    Serial.println(messageSize);
  }  

  message.encode(ndefBuf);

  nfc.setNdefFile(ndefBuf, messageSize);
  nfc.setUid(id);

  Serial.print("Restart Tag\n");
}