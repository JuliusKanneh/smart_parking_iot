//#include <ESP8266WiFi.h>
//#include <ESP8266HTTPClient.h>
//#include <WiFiClient.h>

#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>

#define SERVER_IP "192.168.1.65:8000" // PC address with emulation on host

#include <SoftwareSerial.h>
 
#include <SPI.h>  
#include <MFRC522.h>  
#include <Servo.h>  
#include <Wire.h>   
   
#define SS_PIN 4
#define RST_PIN 5 

//************** Global Varibles ****************
int trigPin =0; //Trigger
int echoPin =16; //Echo
long duration,cm;
int dis;
int paymentStatus = 0;
  //int buzzer = 2;
String str;
String content;

const char* serverName = "http://192.168.1.156:9191/push/1";
//************** Global Varibles End ****************

 
MFRC522 mfrc522(SS_PIN, RST_PIN);  // Create MFRC522 instance.  
Servo myServo; //define servo name  

//************** Wifi Credentials ****************
#ifndef STASSID
#define STASSID "GEEKS-TEAM"
#define STAPSK  "com.keypass@rw-lib2022"
#endif
//************** Varibles End ****************


unsigned long lastTime = 0;
unsigned long timerDelay = 5000;

void setup()   
{  
  Serial.begin(115200);
  Serial1.begin(115200);
  WiFi.begin(STASSID, STAPSK);
  SPI.begin();
  
  connectWifi();
  
  mfrc522.PCD_Init();  // Initiate MFRC522  
  myServo.attach(15); //servo pin  
  // myServo.write(0); //servo start position  

  Serial.println("Put your card to the reader...");  
  Serial.println();  

  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT); 

//  pinMode(buzzer, OUTPUT);
 } 

  
void loop()   
{  
  dis = distance();
  Serial.println(dis);
  
  // Look for new cards
  if ( ! mfrc522.PICC_IsNewCardPresent()){
//    Serial.println();
//    Serial.println("*****************");
//    Serial.println("Swap your card");
//    Serial.println("*****************");
//    Serial.println();
    controllGate();
    return;  
  }
  
  // Select one of the cards  
  if ( ! mfrc522.PICC_ReadCardSerial()){  
    Serial.println();
    Serial.println("*****************");  
    Serial.println("Please Swap the right kind of card");
    Serial.println("*****************");  
    Serial.println();
//   return; 
  }
  
  //Show UID on serial monitor
  Serial.print("UID tag :");
  content= "";
  byte letter;
  for (byte i = 0; i < mfrc522.uid.size; i++)
  {
    Serial.print(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " ");  
    Serial.print(mfrc522.uid.uidByte[i], HEX);
    content.concat(String(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " "));
    content.concat(String(mfrc522.uid.uidByte[i], HEX));
  }
  
  Serial.println();  
  Serial.println("Message : ");  
  content.toUpperCase();
  Serial.println(content);


  //Integrating here
  httpPostRequest();


    
  if(content.substring(1) == "19 98 99 C1"){
    paymentStatus = 1;
    String access = "Paid";
    Serial1.print(access);
  }else{
    paymentStatus = 0;
    String denied = "Not-Paid";
    Serial1.print(denied);
  }

  Serial.print("Payment Status : ");
  Serial.println(paymentStatus);
  
  controllGate();

  // Halt PICC
  mfrc522.PICC_HaltA();

  // Stop encryption on PCD
  mfrc522.PCD_StopCrypto1();
}  

//-------------------------------------------------------Additional Funtions---------------------------------------------------------------
 int distance(){
  // The sensor is triggered by a HIGH pulse of 10 or more microseconds.
// Give a short LOW pulse beforehand to ensure a clean HIGH pulse:
   digitalWrite(trigPin, LOW);
   delayMicroseconds(5);
   digitalWrite(trigPin, HIGH);
   delayMicroseconds(10);
   digitalWrite(trigPin, LOW);  

  // Read the signal from the sensor: a HIGH pulse whose
// duration is the time (in microseconds) from the sending
// of the ping to the reception of its echo off of an object.

  pinMode(echoPin, INPUT);
  duration = pulseIn(echoPin, HIGH);

  //convert the time into a distance
  cm = (duration / 2) * 0.0343; // divide by 29.1 or multiply by 0.0343

  Serial.print(cm);
  Serial.print("cm");
  Serial.println();

  delay(250);
  return cm;
 }

void DoorOpen(){
  Serial.println("Authorized access");
  Serial.println();
  Serial.println("Door Open");
  myServo.write(0);
   /*digitalWrite(buzzer, HIGH);
   delay(500);
   digitalWrite(buzzer, LOW);*/
   delay(4000);
   myServo.write(180);
   paymentStatus = 0;
   content = "";
}

 void DoorLocked(){
//  Serial.println("CARD IS INVALID");  
  Serial.println("Door Locked"); 
    /* digitalWrite(buzzer, HIGH);
    delay(500); 
    digitalWrite(buzzer, LOW);
    delay(500); 
    digitalWrite(buzzer, HIGH);
    delay(500); 
    digitalWrite(buzzer, LOW);
    delay(500);
    digitalWrite(buzzer, HIGH);
    delay(500);
    digitalWrite(buzzer, LOW);*/
 }

 void connectWifi(){
  WiFi.begin(STASSID, STAPSK);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected! IP address: ");
  Serial.println(WiFi.localIP());

}

void controllGate(){
  if (paymentStatus == 1){   
    if(dis <= 25){
      DoorOpen();
//      paymentStatus = 0;
    }else{
      Serial.println();
      Serial.println("Start Going!");
    } 
  }else{  
//    String denied = "Access Denied";
//    Serial1.println(denied);
    Serial.println("Please Pay");
    paymentStatus = 0;
    DoorLocked();
  }
}


void httpPostRequest(){
  if ((WiFi.status() == WL_CONNECTED)) {

    WiFiClient client;
    HTTPClient http;

    Serial.print("[HTTP] begin...\n");
    // configure traged server and url
    http.begin(client, "http://" SERVER_IP "/api/payment_id/");
    http.addHeader("Content-Type", "application/json");

    Serial.print("[HTTP] POST...\n");

    // Data to send with HTTP POST
      String httpRequestData = "amount=200";  
      httpRequestData += "&status=1";  
      httpRequestData += "&uid_id="; 
      httpRequestData += content; 
      
      Serial.println(httpRequestData);     
    
    // start connection and send HTTP header and body
//    int httpCode = http.POST("{\"amount\":\"200\", \"status\":\"1\", \"uid_id\":\"101\"}");
    int httpCode = http.POST(httpRequestData);
    
    // httpCode will be negative on error
    if (httpCode > 0) {
      // HTTP header has been send and Server response header has been handled
      Serial.printf("[HTTP] POST... code: %d\n", httpCode);

      // file found at server
      if (httpCode == HTTP_CODE_OK) {
        const String& payload = http.getString();
        Serial.println("received payload:\n<<");
        Serial.println(payload);
        Serial.println(">>");
      }
    } else {
      Serial.printf("[HTTP] POST... failed, error: %s\n", http.errorToString(httpCode).c_str());
    }
    
    http.end();
  }
}
