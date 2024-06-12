#include <SPI.h>
#include <MFRC522.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <stdio.h>
#include <string.h>
#include <SoftwareSerial.h>

SoftwareSerial espSerial(0, 1); //Pin TX,RX 0,1
LiquidCrystal_I2C lcd(0x27, 16, 2);

/*Using Hardware SPI of Arduino */
/*MOSI (11), MISO (12) and SCK (13) ar  e fixed */
/*You can configure SS and RST Pins*/
#define SS_PIN 10  /* Slave Select Pin */
#define RST_PIN 7  /* Reset Pin */

/* Create an instance of MFRC522 */
MFRC522 mfrc522(SS_PIN, RST_PIN);
/* Create an instance of MIFARE_Key */
MFRC522::MIFARE_Key key;          

/*LED setup */
int redLED = 6;
int greenLED = 5;

/* Set the block to which we want to write data */
/* Be aware of Sector Trailer Blocks */
int blockNumWallet = 2;  
int blockNumLoct = 6;  
int blockNumDate = 10;  
/* Create an array of 16 Bytes and fill it with data */
/* This is the actual data which is going to be written into the card */
byte blockDataWallet [16] = {"207340"};
byte blockDataLoct [16] = {"MuhdFaris"};
byte blockDataDate [16] = {"Akmal"};

/* Create another array to read data from Block */
/* Legthn of buffer should be 2 Bytes more than the size of Block (16 Bytes) */
byte bufferLen = 18;
byte readBlockDataWallet[18];
byte readBlockDataLoct[18];
byte readBlockDataDate[18];

MFRC522::StatusCode status;

void setup() 
{
  /* Initialize serial communications with the PC */
  Serial.begin(9600);
  espSerial.begin(9600);
  pinMode(redLED, OUTPUT);
  pinMode(greenLED, OUTPUT);
  lcd.init();         // initialize the lcd
  lcd.backlight();    // Turn on the LCD screen backlight
  /* Initialize SPI bus */
  SPI.begin();
  /* Initialize MFRC522 Module */
  mfrc522.PCD_Init();
  lcd.setCursor(0, 0);
  lcd.print("Scan Your Matric");
  lcd.setCursor(0, 1);
  lcd.print("Card Here....");
}

void loop()
{
  delay(1000);
  char name[16] = ""; 
  char lastname[16] = ""; 
  char studentid[16] = ""; 
  /* Prepare the key for authentication */
/* All keys are set to FFFFFFFFFFFFh at chip delivery from the factory */
for (byte i = 0; i < 6; i++)
{
  key.keyByte[i] = 0xFF;
}

/* Look for new cards */
while (true) { // Add a while loop here
  /* Reset the loop if no new card is present on RC522 Reader */
  if ( ! mfrc522.PICC_IsNewCardPresent()){
    continue; // Change 'return' to 'continue'
  }
  
  /* Select one of the cards */
  if ( ! mfrc522.PICC_ReadCardSerial()){
    continue; // Change 'return' to 'continue'
  }

  /* Call 'WriteDataToBlock' function, which will write data to the block */
  //  Serial.print("\n");
  //  Serial.println("Writing to Data Block...");
  //  WriteDataToBlock(blockNumWallet, blockDataWallet);
  //  WriteDataToBlock(blockNumLoct, blockDataLoct);
  //  WriteDataToBlock(blockNumDate, blockDataDate);
   
  /* Read data from the same block */
  // Serial.print("\n");
  // Serial.println("Reading from Data Block...");
   ReadDataFromBlock(blockNumWallet, readBlockDataWallet);
   ReadDataFromBlock(blockNumLoct, readBlockDataLoct);
   ReadDataFromBlock(blockNumDate, readBlockDataDate);
  /* If you want to print the full memory dump, uncomment the next line */
  // mfrc522.PICC_DumpToSerial(&(mfrc522.uid));
   
  /* Print the data read from block */
  // Serial.print("\n");
  // Serial.print("Data in Block:");
  // Serial.print(blockNumWallet);
  // Serial.print(" --> ");
  for (int j=0 ; j<16 ; j++)
  {
    // Serial.write(readBlockDataWallet[j]);
    studentid[j] = readBlockDataWallet[j];
  }
  // Serial.print("\n");
  // Serial.print("Data in Block:");
  // Serial.print(blockNumLoct);
  // Serial.print(" --> ");
  for (int j=0 ; j<16 ; j++)
  {
    // Serial.write(readBlockDataLoct[j]);
    name[j] = readBlockDataLoct[j];
  }
  
  // Serial.print("\n");
  // Serial.print("Data in Block:");
  // Serial.print(blockNumDate);
  // Serial.print(" --> ");
  for (int j=0 ; j<16 ; j++)
  {
    // Serial.write(readBlockDataDate[j]);
    lastname[j] = readBlockDataDate[j];
  }
  // Serial.print(name);
  // Serial.print(lastname);
  
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(name);
  lcd.setCursor(0, 1);
  lcd.print(String(studentid));
  delay(3000);
  lcd.clear();

  lcd.setCursor(0, 0);
  lcd.print("Scan Your Matric");
  lcd.setCursor(0, 1);
  lcd.print("Card Here....");
  espSerial.println(String(name) + ";" + String(lastname) + ";" + String(studentid));
  Serial.println(String(name) + ";" + String(lastname) + ";" + String(studentid));

  /* Halt the current card before continuing to the next card */
  mfrc522.PICC_HaltA();
  mfrc522.PCD_StopCrypto1();
} // End of while loop

}



void WriteDataToBlock(int blockNum, byte blockData[]) 
{
  /* Authenticating the desired data block for write access using Key A */
  status = mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, blockNum, &key, &(mfrc522.uid));
  if (status != MFRC522::STATUS_OK)
  {
    Serial.print("Authentication failed for Write: ");
    Serial.println(mfrc522.GetStatusCodeName(status));
    return;
  }

  
  /* Write data to the block */
  status = mfrc522.MIFARE_Write(blockNum, blockData, 16);
  if (status != MFRC522::STATUS_OK)
  {
    Serial.print("Writing to Block failed: ");
    Serial.println(mfrc522.GetStatusCodeName(status));
    return;
  }
  
}

void ReadDataFromBlock(int blockNum, byte readBlockData[]) 
{
  /* Authenticating the desired data block for Read access using Key A */
  byte status = mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, blockNum, &key, &(mfrc522.uid));

  if (status != MFRC522::STATUS_OK)
  {
      digitalWrite(redLED, HIGH);
      delay(1000);

      digitalWrite(redLED, LOW);   
      delay(1000); 
      
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Oops.. Error :(");
      lcd.setCursor(0, 1);
      lcd.print("Please Try Again");
    //  Serial.print("Authentication failed for Read: ");
    //  Serial.println(mfrc522.GetStatusCodeName(status));
     return;
  }

  /* Reading data from the Block */
  status = mfrc522.MIFARE_Read(blockNum, readBlockData, &bufferLen);
  if (status != MFRC522::STATUS_OK)
  {
    digitalWrite(redLED, HIGH);
    delay(1000);

    digitalWrite(redLED, LOW);   
    delay(1000); 
    // Serial.print("Reading failed: ");
    // Serial.println(mfrc522.GetStatusCodeName(status));
    return;
  } else {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Scanning....");
    lcd.setCursor(0, 1);
    lcd.print("Don't Move");
    digitalWrite(greenLED, HIGH);
      delay(1000);

      digitalWrite(greenLED, LOW);   
      delay(1000); 
      lcd.clear();
  }
}

// while (Serial.available() > 0) {
//     char t = Serial.read();
//   } 