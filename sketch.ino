#include <SPI.h>
#include <MFRC522.h>
#include <LiquidCrystal_I2C.h>
#include <EEPROM.h> // Handles internal permanent storage

#define RST_PIN         9
#define SS_PIN          10
#define BUZZER_PIN      5

MFRC522 mfrc522(SS_PIN, RST_PIN);
LiquidCrystal_I2C lcd(0x27, 16, 2);

// Firmware States for State Machine Architecture
enum SystemState {
  NORMAL_MODE,
  PROGRAMMING_MODE
};

SystemState currentState = NORMAL_MODE;

// Master Card definition (Hardcoded Admin Token)
const byte masterCard[4] = {0x01, 0x02, 0x03, 0x04};
// Memory address allocation for the user card
const int EEPROM_START_ADDR = 10; 

void setup() {
  SPI.begin();
  mfrc522.PCD_Init();
  lcd.init();
  lcd.backlight();
  pinMode(BUZZER_PIN, OUTPUT);
  
  displayReadyMessage();
}

void loop() {
  // Look for new RFID tags
  if (!mfrc522.PICC_IsNewCardPresent() || !mfrc522.PICC_ReadCardSerial()) {
    return;
  }
  
  byte scannedUID[4];
  for (byte i = 0; i < 4; i++) {
    scannedUID[i] = mfrc522.uid.uidByte[i];
  }
  
  // STATE MACHINE LOGIC
  if (currentState == NORMAL_MODE) {
    if (checkCardMatch(scannedUID, masterCard)) {
      // Master card switches system state
      currentState = PROGRAMMING_MODE;
      triggerAdminAlert();
    } 
    else if (checkStoredUserCard(scannedUID)) {
      executeAccessGranted();
    } 
    else {
      executeAccessDenied();
    }
  } 
  else if (currentState == PROGRAMMING_MODE) {
    // In programming mode, the next card scanned becomes the authorized user
    if (checkCardMatch(scannedUID, masterCard)) {
      // If master card is scanned again, cancel/exit programming mode
      currentState = NORMAL_MODE;
      displayReadyMessage();
    } else {
      writeCardToEEPROM(scannedUID);
      currentState = NORMAL_MODE;
      displayReadyMessage();
    }
  }
  
  mfrc522.PICC_HaltA();
}

// --- HELPER ARDUINO FUNCTIONS ---

bool checkCardMatch(byte a[], const byte b[]) {
  for (byte i = 0; i < 4; i++) {
    if (a[i] != b[i]) return false;
  }
  return true;
}

bool checkStoredUserCard(byte scanned[]) {
  for (byte i = 0; i < 4; i++) {
    // Read directly from permanent byte registers
    if (scanned[i] != EEPROM.read(EEPROM_START_ADDR + i)) {
      return false;
    }
  }
  return true;
}

void writeCardToEEPROM(byte newCard[]) {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("SAVING TO EEPROM");
  
  for (byte i = 0; i < 4; i++) {
    EEPROM.write(EEPROM_START_ADDR + i, newCard[i]);
  }
  
  lcd.setCursor(0, 1);
  lcd.print("SUCCESSFULLY LOADED");
  tone(BUZZER_PIN, 1500, 500);
  delay(2000);
}

void triggerAdminAlert() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("[ ADMIN MODE ]");
  lcd.setCursor(0, 1);
  lcd.print("SCAN NEW USER...");
  
  // Distinct multi-tone chirp indicating state machine shift
  tone(BUZZER_PIN, 800, 100); delay(150);
  tone(BUZZER_PIN, 1000, 100); delay(150);
  tone(BUZZER_PIN, 1200, 100);
}

void executeAccessGranted() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("ACCESS GRANTED");
  lcd.setCursor(0, 1);
  lcd.print("SYSTEM UNLOCKED");
  tone(BUZZER_PIN, 1000, 200); delay(250);
  tone(BUZZER_PIN, 1200, 200);
  delay(3000);
  displayReadyMessage();
}

void executeAccessDenied() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("ACCESS DENIED!");
  lcd.setCursor(0, 1);
  lcd.print("UNAUTHORIZED");
  for(int i = 0; i < 3; i++) {
    tone(BUZZER_PIN, 300, 300);
    delay(400);
  }
  displayReadyMessage();
}

void displayReadyMessage() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(" SECURE SYSTEM ");
  lcd.setCursor(0, 1);
  lcd.print(" SCAN YOUR CARD ");
}
