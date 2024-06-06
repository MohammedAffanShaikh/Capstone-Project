#include <SPI.h>
#include <MFRC522.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

#define SS_PIN 10
#define RST_PIN 9
#define BUZZER_PIN 4 // Connect the positive leg of the buzzer to pin 4

MFRC522 mfrc522(SS_PIN, RST_PIN);   // Create MFRC522 instance

// Valid UIDs of students
byte validUIDs[][4] = {
  {0x73, 0x91, 0xFC, 0x24},  // UID of student 1
  {0xE3, 0xF3, 0x22, 0x0E},  // UID of student 2
    
};

// Student details
struct Student {
  const char* name;
  const char* branch;
};

// Define student details
Student students[] = {
  {"Affan", "E.X.T.C."},
  {"Aqeel", "I.T."},
  {"ELON MUSK", "Extc"},
  {"ARIJIT SINGH", "Extc"}
};

// LCD I2C display
LiquidCrystal_I2C lcd(0x27, 16, 2);  // Adjust the address and size according to your display

unsigned long startTime = 0; // Variable to store program start time

void setup() {
  Serial.begin(9600);   // Initialize serial communication
  SPI.begin();          // Initiate SPI bus
  mfrc522.PCD_Init();   // Initiate MFRC522
  lcd.begin(16, 2);     // Initialize the LCD display
  lcd.init();
  lcd.backlight();      // Turn on the backlight
  lcd.clear();
  lcd.setCursor(2, 0);
  lcd.print("Scan ID card");
  Serial.println("Ready to read RFID cards");

  pinMode(BUZZER_PIN, OUTPUT); // Set buzzer pin as output

  startTime = millis(); // Record the start time
}

void loop() {
  // Look for new cards
  if (mfrc522.PICC_IsNewCardPresent()) {
    if (mfrc522.PICC_ReadCardSerial()) {
      // Show UID number on serial monitor
      Serial.print("UID Number: ");
      String content = "";
      byte letter;
      for (byte i = 0; i < mfrc522.uid.size; i++) {
        if (mfrc522.uid.uidByte[i] < 0x10) content += "0";
        content += String(mfrc522.uid.uidByte[i], HEX);
      }
      content.toUpperCase();
      Serial.println(content);

      // Display UID tag on serial monitor
      Serial.print("UID: ");
      for (byte i = 0; i < mfrc522.uid.size; i++) {
        Serial.print("0x");
        if (mfrc522.uid.uidByte[i] < 0x10) Serial.print("0");
        Serial.print(mfrc522.uid.uidByte[i], HEX);
        if (i < mfrc522.uid.size - 1) Serial.print(", ");
      }
      Serial.println();

      // Check if the UID matches any of the valid UIDs
      bool uidMatched = false;
      int studentIndex = -1;
      for (int i = 0; i < sizeof(validUIDs) / sizeof(validUIDs[0]); i++) {
        if (memcmp(mfrc522.uid.uidByte, validUIDs[i], mfrc522.uid.size) == 0) {
          uidMatched = true;
          studentIndex = i;
          break;
        }
      }

      // Perform actions based on UID match
      if (uidMatched) {
        // UID matches, perform attendance action for the corresponding student
        updateAttendance(studentIndex + 1);
        tone(BUZZER_PIN, 1000, 500); // Emit a 1 kHz tone for 500 milliseconds
        delay(3000);  // Display attendance status for 3 seconds
      } else {
        // UID doesn't match, perform other actions (e.g., display error message)
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("ID doesn't match");
        delay(2000);
      }

      // Clear the LCD display and reset for the next scan
      lcd.clear();
      lcd.setCursor(2, 0);
      lcd.print("Scan ID card");
      delay(1000);   // Delay to avoid reading the card multiple times in a short period
    }
    mfrc522.PICC_HaltA();   // Stop reading
    mfrc522.PCD_StopCrypto1();  // Stop encryption on PCD
  }
}

// Function to update attendance in the database
void updateAttendance(int studentID) {
  // Display the name of the student on the LCD
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Name: ");
  lcd.print(students[studentID - 1].name);  // -1 to adjust for array indexing
  delay(1000); // Delay to display the name for 1 second

  // Display branch below the name
  lcd.setCursor(0, 1);
  lcd.print("Branch: ");
  lcd.print(students[studentID - 1].branch);


  delay(1000); // Delay for 1 second after displaying branch

  // Display UID number below "Present" on the LCD
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Present");
  lcd.setCursor(0, 1);
  lcd.print("UID : ");
  for (byte i = 0; i < mfrc522.uid.size; i++) {
    if (mfrc522.uid.uidByte[i] < 0x10) lcd.print("0"); // Add leading zero for values less than 0x10
    lcd.print(mfrc522.uid.uidByte[i], HEX);
  }
  
  delay(1000); // Delay for 1 second after displaying UID number
  lcd.clear();
  lcd.setCursor(2, 0);
  lcd.print("Scan ID card");
  delay(1000); // Delay for 1 second after displaying "Scan ID card"

  // Display name, branch, UID number, and UID tag in the Serial Monitor
  Serial.println();
  Serial.print("Name: ");
  Serial.println(students[studentID - 1].name);
  Serial.print("Branch: ");
  Serial.println(students[studentID - 1].branch);
  Serial.print("UID Number: ");
  for (byte i = 0; i < mfrc522.uid.size; i++) {
    if (mfrc522.uid.uidByte[i] < 0x10) Serial.print("0"); // Add leading zero for values less than 0x10
    Serial.print(mfrc522.uid.uidByte[i], HEX);
  }
  Serial.println();
  Serial.print("UID Tag: ");
  for (byte i = 0; i < mfrc522.uid.size; i++) {
    Serial.print("0x");
    if (mfrc522.uid.uidByte[i] < 0x10) Serial.print("0");
    Serial.print(mfrc522.uid.uidByte[i], HEX);
    if (i < mfrc522.uid.size - 1) Serial.print(", ");
  }
  Serial.println();
}
