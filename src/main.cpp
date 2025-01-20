#include <Adafruit_GFX.h>
#include <Adafruit_SH1106.h>
#include <Arduino.h>
#include <Fonts/FreeMono9pt7b.h>
#include <MFRC522.h>
#include <SPI.h>
#include <Wire.h>

/**
 *
 * RFID-RC522 shield
 *
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

#define SS_PIN 15
#define RST_PIN 5
MFRC522 mfrc522(SS_PIN, RST_PIN);

/**
 * oled display
 *
 */

#define OLED_SDA 21
#define OLED_SCL 22
#define i2C_ADDRESS 0x3C
Adafruit_SH1106 display;

#define pinRelay 2  // Pin to control the relay
String cardUID;      // Variable to store the UID of the read card

/**
 * @brief Get the byte array object from the buffer
 *
 * @param buffer
 * @param bufferSize
 * @return String
 */
String get_byte_array(byte *buffer, byte bufferSize) {
    String data;

    for (byte i = 0; i < bufferSize; i++) {
        data += String(buffer[i], HEX);
    }

    return data;
}

/**
 * @brief Display the card UID on the OLED display
 *
 * @param cardUID
 */
void displayCardUID(String cardUID) {
    display.clearDisplay();
    display.setCursor(0, 10);
    display.println("Card UID:");
    display.println(cardUID);
    display.display();
}

/**
 * @brief main setup function
 *
 */
void setup() {
    Serial.begin(115200);
    SPI.begin();

    pinMode(pinRelay, OUTPUT);
    digitalWrite(pinRelay, LOW);

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

    Serial.println("Started, waiting for card ...");
    Serial.println();
}

void loop() {
    if (!mfrc522.PICC_IsNewCardPresent())
        return;

    // Select one of the cards
    if (!mfrc522.PICC_ReadCardSerial())
        return;

    String newCardUID = get_byte_array(mfrc522.uid.uidByte, mfrc522.uid.size);

    if (newCardUID != cardUID) {  // If new card is found ...
        cardUID = newCardUID;
        displayCardUID(cardUID);
        Serial.print("new card UID: " + cardUID);
        Serial.println();
        Serial.print(F("PICC type: "));
        MFRC522::PICC_Type piccType = mfrc522.PICC_GetType(mfrc522.uid.sak);
        Serial.println(mfrc522.PICC_GetTypeName(piccType));

        // Always open relay for 1 second when a new card is detected
        digitalWrite(pinRelay, HIGH);
        delay(1000);
        digitalWrite(pinRelay, LOW);
    }
}