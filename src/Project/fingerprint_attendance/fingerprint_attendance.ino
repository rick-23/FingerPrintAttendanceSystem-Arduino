#include <Wire.h> 
#include <LiquidCrystal_I2C.h>
#include <SPI.h>
#include <SD.h>
#include <Adafruit_Fingerprint.h> 

#define switchReg 4    //Registration button
#define switchAtten 5  //Attendance button  
#define ledBlue 6
#define ledRed 7 


// uncomment for uno 
//#include <SoftwareSerial.h>  
//SoftwareSerial mySerial(2, 3);  

// uncomment for Mega
#define mySerial Serial1

LiquidCrystal_I2C lcd(0x27,16,2);
Adafruit_Fingerprint finger = Adafruit_Fingerprint(&mySerial); 
 
uint8_t id;
File myFile;
int Status = 0;
const int chipSelect = 53;

void setup() {
  // put your setup code here, to run once:
  
  pinMode(switchAtten, INPUT);
  pinMode(switchReg, INPUT);
  pinMode(ledBlue, OUTPUT); 
  pinMode(ledRed, OUTPUT); 
  pinMode(chipSelect,OUTPUT);
  

  lcd.init();
  lcd.backlight();
  lcd.clear();
  
  Serial.begin(9600);

  while (!Serial);  // For Yun/Leo/Micro/Zero/...  
  delay(100);  
  Serial.println("\n\nAdafruit finger detect test");  
  
  finger.begin(57600);  
    
    
  if (finger.verifyPassword()) {  
    Serial.println("Found fingerprint sensor!");  
  } else {  
    Serial.println("Did not find fingerprint sensor :(");  
    while (1) { delay(1); }  
  }   


  Serial.print("Initializing SD card...");

  if (!SD.begin(chipSelect)) {
    Serial.println("Card failed, or not present");
    //while (1);
  }
  Serial.println("card initialized.");

}

void loop() {
  // put your main code here, to run repeatedly:
  Status = 0;
  lcd.setCursor(1,0);
  lcd.print("Attendance Proj");
  lcd.setCursor(3,1);
  lcd.print("ELG Group-7");
  
  
  int buttonStateP1 = 0; //Attendance button tracking
  int buttonStateP2 = 0; //Registration button tracking
    
  digitalWrite(ledBlue,LOW);
  digitalWrite(ledRed,LOW);
  
  
  buttonStateP1 = digitalRead(switchReg); // Listen for the start button to be pressed
  buttonStateP2 = digitalRead(switchAtten);
  
  
  //if the registration button has been pressed
  if (buttonStateP1 == HIGH ){
    
    digitalWrite(ledBlue,HIGH);
    digitalWrite(ledRed,HIGH);

    lcd.setCursor(3,0);
    lcd.print("Registration Process");

    finger.getTemplateCount();  
    Serial.print("Sensor contains "); 
    Serial.print(finger.templateCount); 
    Serial.println(" templates");  
    id = finger.templateCount + 1;
    Serial.println(id); 
    getFingerprintEnroll();
    


    buttonStateP1 = 0;
    
  //if the attendance button has been pressed  
  }else if(buttonStateP2 == HIGH){

    uint8_t userId = -1;
    lcd.print("Attendance Process");
    digitalWrite(ledBlue,LOW);
    digitalWrite(ledRed,LOW);

    Serial.println("Waiting for valid finger...");
    for(int i=0;i<1;i++)
    {
      Status = 0;
      lcd.clear();
      lcd.print("Place Finger");
      delay(2000);

      userId = getFingerprintIDez();
      if(userId >=1 && Status != 1)
      {
        digitalWrite(ledBlue,HIGH);
        lcd.clear();
        lcd.setCursor(3,0);
        lcd.print("Present");
        delay(2000);

        // Storing data in SD card

        String ID = String(userId);
 
        myFile = SD.open("test.txt", FILE_WRITE);

        // if the file opened okay, write to it:
        if (myFile) {
          
          myFile.print("User ID ");
          myFile.println(ID);
          // close the file:
          myFile.close();
          Serial.println("done.");
          } else {
          // if the file didn't open, print an error:
          Serial.println("error opening test.txt");
          } 
      }
        
    }   
     
    
    buttonStateP2 = 0;
  }

}
  

// #################### Fingerprint Enroll Function ############################
uint8_t getFingerprintEnroll() {
 
  int p = -1;
  Serial.print("Waiting for valid finger to enroll as #");
  Serial.println(id);
  lcd.clear();
  lcd.setCursor(4,0);
  lcd.print("Place the"); 
  lcd.setCursor(5,1);
  lcd.print("finger "); 
  
  while (p != FINGERPRINT_OK) {
    p = finger.getImage();
    switch (p) {
    case FINGERPRINT_OK:
      Serial.println("Image taken");
      break;
    case FINGERPRINT_NOFINGER:
      Serial.println(".");
      break;
    case FINGERPRINT_PACKETRECIEVEERR:
      Serial.println("Communication error");
      break;
    case FINGERPRINT_IMAGEFAIL:
      Serial.println("Imaging error");
      break;
    default:
      Serial.println("Unknown error");
      break;
    }
  }

  // OK success!

  p = finger.image2Tz(1);
  switch (p) {
    case FINGERPRINT_OK:
      Serial.println("Image converted");
      break;
    case FINGERPRINT_IMAGEMESS:
      Serial.println("Image too messy");
      return p;
    case FINGERPRINT_PACKETRECIEVEERR:
      Serial.println("Communication error");
      return p;
    case FINGERPRINT_FEATUREFAIL:
      Serial.println("Could not find fingerprint features");
      return p;
    case FINGERPRINT_INVALIDIMAGE:
      Serial.println("Could not find fingerprint features");
      return p;
    default:
      Serial.println("Unknown error");
      return p;
  }
  
  Serial.println("Remove finger");
  lcd.clear();
  lcd.print(" Remove finger "); 
  delay(2000);
  p = 0;
  while (p != FINGERPRINT_NOFINGER) {
    p = finger.getImage();
  }
  Serial.print("ID "); Serial.println(id);
  p = -1;
  Serial.println("Place same finger again");
  lcd.clear();
  lcd.setCursor(3,0);
  lcd.print("Place same"); 
  lcd.setCursor(3,1);
  lcd.print("finger again"); 
  
  while (p != FINGERPRINT_OK) {
    p = finger.getImage();
    switch (p) {
    case FINGERPRINT_OK:
      Serial.println("Image taken");
      break;
    case FINGERPRINT_NOFINGER:
      Serial.print(".");
      break;
    case FINGERPRINT_PACKETRECIEVEERR:
      Serial.println("Communication error");
      break;
    case FINGERPRINT_IMAGEFAIL:
      Serial.println("Imaging error");
      break;
    default:
      Serial.println("Unknown error");
      break;
    }
  }

  // OK success!

  p = finger.image2Tz(2);
  switch (p) {
    case FINGERPRINT_OK:
      Serial.println("Image converted");
      break;
    case FINGERPRINT_IMAGEMESS:
      Serial.println("Image too messy");
      return p;
    case FINGERPRINT_PACKETRECIEVEERR:
      Serial.println("Communication error");
      return p;
    case FINGERPRINT_FEATUREFAIL:
      Serial.println("Could not find fingerprint features");
      return p;
    case FINGERPRINT_INVALIDIMAGE:
      Serial.println("Could not find fingerprint features");
      return p;
    default:
      Serial.println("Unknown error");
      return p;
  }
  
  // OK converted!
  Serial.print("Creating model for #");  Serial.println(id);
  
  p = finger.createModel();
  if (p == FINGERPRINT_OK) {
    Serial.println("Prints matched!");
  } else if (p == FINGERPRINT_PACKETRECIEVEERR) {
    Serial.println("Communication error");
    return p;
  } else if (p == FINGERPRINT_ENROLLMISMATCH) {
    Serial.println("Fingerprints did not match");
    return p;
  } else {
    Serial.println("Unknown error");
    return p;
  }   
  
  Serial.print("ID "); Serial.println(id);
  p = finger.storeModel(id);
  if (p == FINGERPRINT_OK) {
    Serial.println("Stored!");
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("Stored!"); 
    lcd.setCursor(0,1);
    lcd.print("ID ");lcd.print(id); 
    delay(3000);
    
  } else if (p == FINGERPRINT_PACKETRECIEVEERR) {
    Serial.println("Communication error");
    return p;
  } else if (p == FINGERPRINT_BADLOCATION) {
    Serial.println("Could not store in that location");
    return p;
  } else if (p == FINGERPRINT_FLASHERR) {
    Serial.println("Error writing to flash");
    return p;
  } else {
    Serial.println("Unknown error");
    return p;
  }
  lcd.clear();   
}

// ##################  Fingerprint Search   ###########################

int getFingerprintIDez() {
    
    uint8_t p = finger.getImage();
    if (p != FINGERPRINT_OK){
      lcd.clear();
      lcd.setCursor(3,0);
      lcd.print("No Finger ");
      lcd.setCursor(3,1);
      lcd.print("found");
      delay(2000);
      Status = 1;
      return -1;  
    }
        

    p = finger.image2Tz();
    if (p != FINGERPRINT_OK){
      lcd.clear();
      lcd.setCursor(3,0);
      lcd.print("No Finger ");
      lcd.setCursor(3,1);
      lcd.print("found");
      delay(2000);      
      Status = 1;
      return -1;
    }

    p = finger.fingerFastSearch();
    if (p != FINGERPRINT_OK){
      Serial.println("Error Finger");
      digitalWrite(ledBlue,LOW);
      digitalWrite(ledRed,HIGH);
      lcd.clear();
      lcd.setCursor(3,0);
      lcd.print("No Finger ");
      lcd.setCursor(3,1);
      lcd.print("found");
      delay(2000);
      Status = 1;
      return -1;
      
    }

   
 
  Serial.print("Found ID #"); Serial.print(finger.fingerID); 
  Serial.print(" with confidence of "); Serial.println(finger.confidence);
  return finger.fingerID; 
}
