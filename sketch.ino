#include <SPI.h>
#include <MFRC522.h>
#include <LiquidCrystal_I2C.h>

// Pin definitions
#define RST_PIN         9
#define SS_PIN          10
#define BUZZER_PIN      5

MFRC522 mfrc522(SS_PIN, RST_PIN);   // Create MFRC522 instance
LiquidCrystal_I2C lcd(0x27, 16, 2); // Create LCD instance

// Updated with your exact card's UID code!
byte masterCardUID[] = {0x01, 0x02, 0x03, 0x04}; 

void setup() {
  SPI.begin();         // Init SPI bus
  mfrc522.PCD_Init();  // Init MFRC522 reader
  
  lcd.init();
  lcd.backlight();
  
  pinMode(BUZZER_PIN, OUTPUT);
  
  // Ready Message
  lcd.setCursor(0, 0);
  lcd.print(" SECURE SYSTEM ");
  lcd.setCursor(0, 1);
  lcd.print(" SCAN YOUR CARD ");
}

void loop() {
  // Look for new cards
  if ( ! mfrc522.PICC_IsNewCardPresent()) {
    return;
  }

  // Select one of the cards
  if ( ! mfrc522.PICC_ReadCardSerial()) {
    return;
  }
  
  // Check if the scanned card matches your Master Card
  bool accessGranted = true;
  for (byte i = 0; i < 4; i++) {
    if (mfrc522.uid.uidByte[i] != masterCardUID[i]) {
      accessGranted = false;
      break;
    }
  }
  
  lcd.clear();
  
  if (accessGranted) {
    // Access Granted Logic
    lcd.setCursor(0, 0);
    lcd.print("ACCESS GRANTED");
    lcd.setCursor(0, 1);
    lcd.print("SYSTEM UNLOCKED");
    
    // Play happy confirmation tone
    tone(BUZZER_PIN, 1000, 200);
    delay(250);
    tone(BUZZER_PIN, 1200, 200);
    delay(3000);          // Keep system unlocked on screen for 3 seconds
    
  } else {
    // Access Denied Logic
    lcd.setCursor(0, 0);
    lcd.print("ACCESS DENIED!");
    lcd.setCursor(0, 1);
    lcd.print("UNAUTHORIZED");
    
    // Play warning alarm tone
    for(int i = 0; i < 3; i++) {
      tone(BUZZER_PIN, 300, 300);
      delay(400);
    }
  }
  
  // Reset display back to waiting mode
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(" SECURE SYSTEM ");
  lcd.setCursor(0, 1);
  lcd.print(" SCAN YOUR CARD ");
  
  mfrc522.PICC_HaltA(); // Halt PICC
}
