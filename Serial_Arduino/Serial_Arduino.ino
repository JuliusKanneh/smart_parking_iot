#include <LiquidCrystal.h>

// initialize the library by associating any needed LCD interface pin
// with the arduino pin number it is connected to
const int rs = 12, en = 11, d4 = 5, d5 = 4, d6 = 3, d7 = 2;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

 String messageSignal;

void setup() {
  // Open serial communications and wait for port to open:
  lcd.begin(16, 2);
  Serial.begin(115200);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }

  lcd.setCursor(0, 0);
  // print the number of seconds since reset:
  lcd.print("Swap Card!");
}

void loop() { // run over and over
  if(Serial.available()) {
    messageSignal = Serial.readString();

    if(messageSignal == "Paid"){
      Serial.println("Turning on Buzzer");
    }else{
      Serial.println("Turning off Buzzer");
    }
    
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.println(messageSignal);
    Serial.println(messageSignal);
    delay(1000);
  }
}
