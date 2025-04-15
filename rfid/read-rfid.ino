/*
RC522 	    ESP32
SDA	    ->  GPIO 5
SCK	    ->  GPIO 18
MOSI	  ->  GPIO 23
MISO	  ->  GPIO 19
IRQ	    ->  No conectado
GND	    ->  GND
RST	    ->  GPIO 22
3.3V	  ->  3.3V
*/

#include <SPI.h>
#include <MFRC522.h>

#define SS_PIN 5 
#define RST_PIN 22

MFRC522 mfrc522(SS_PIN, RST_PIN);

void setup() {
  Serial.begin(115200);
  SPI.begin();
  mfrc522.PCD_Init();
  Serial.println("Acerque su tarjeta RFID...");
}

void loop() {
  if ( !mfrc522.PICC_IsNewCardPresent() || !mfrc522.PICC_ReadCardSerial() ) {
    return;
  }

  Serial.print("UID:");
  for (byte i = 0; i < mfrc522.uid.size; i++) {
    Serial.print(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " ");
    Serial.print(mfrc522.uid.uidByte[i], HEX);
  }
  Serial.println();

  mfrc522.PICC_HaltA();
}
