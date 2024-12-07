#include <Arduino.h>

/**
 * Pin layout used:
 * ------------------------------------
 *             MFRC522      ESP32
 * Signal      Pin          Pin
 * ------------------------------------
 * RST/Reset   RST          5
 * SPI SS      SDA(NSS)     15
 * SPI MOSI    MOSI         13
 * SPI MISO    MISO         19
 * SPI SCK     SCK          18
 *
 */

#include <Adafruit_GFX.h>
#include <Adafruit_SH1106.h>
#include <Fonts/FreeMono9pt7b.h>
#include <MFRC522.h>
#include <SPI.h>
#include <Wire.h>

#define SS_PIN 15
#define RST_PIN 5
MFRC522 mfrc522(SS_PIN, RST_PIN);

String cardUID;

#define OLED_SDA 21
#define OLED_SCL 22
#define i2C_ADDRESS 0x3C
Adafruit_SH1106 display;

#define pinRelais 2

String get_byte_array(byte *buffer, byte bufferSize) {
    String data;

    for (byte i = 0; i < bufferSize; i++) {
        data += String(buffer[i], HEX);
    }

    return data;
}

void displayCardUID(String cardUID) {
    display.clearDisplay();
    display.setCursor(0, 10);
    display.println("Card UID:");
    display.println(cardUID);
    display.display();
}

void setup() {
    Serial.begin(115200);
    SPI.begin();

    pinMode(pinRelais, OUTPUT);
    digitalWrite(pinRelais, LOW);

    // Init MFRC522
    mfrc522.PCD_Init();

    // init oled
    display.begin(SH1106_SWITCHCAPVCC, i2C_ADDRESS);
    display.clearDisplay();
    display.setFont(&FreeMono9pt7b);
    display.setTextSize(1);
    display.setTextColor(WHITE);
    display.setCursor(0, 10);
    display.println("RFID Reader");
    display.display();

    Serial.println("Approach your reader card...");
    Serial.println();
}

void loop() {
    if (!mfrc522.PICC_IsNewCardPresent())
        return;

    // Select one of the cards
    if (!mfrc522.PICC_ReadCardSerial())
        return;

    String newCardUID = get_byte_array(mfrc522.uid.uidByte, mfrc522.uid.size);

    if (newCardUID != cardUID) {
        cardUID = newCardUID;
        displayCardUID(cardUID);
        digitalWrite(pinRelais, HIGH);
        delay(1000);
        digitalWrite(pinRelais, LOW);
        Serial.print("new card UID: " + cardUID);
        Serial.println();
        Serial.print(F("PICC type: "));
        MFRC522::PICC_Type piccType = mfrc522.PICC_GetType(mfrc522.uid.sak);
        Serial.println(mfrc522.PICC_GetTypeName(piccType));
    }
}