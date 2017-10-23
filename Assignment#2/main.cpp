/*
  Optical Heart Rate Detection (PBA Algorithm) using the MAX30105 Breakout
  By: Nathan Seidle @ SparkFun Electronics
  Date: October 2nd, 2016
  https://github.com/sparkfun/MAX30105_Breakout

  This is a demo to show the reading of heart rate or beats per minute (BPM) using
  a Penpheral Beat Amplitude (PBA) algorithm.

  It is best to attach the sensor to your finger using a rubber band or other tightening
  device. Humans are generally bad at applying constant pressure to a thing. When you
  press your finger against the sensor it varies enough to cause the blood in your
  finger to flow differently which causes the sensor readings to go wonky.

  Hardware Connections (Breakoutboard to Arduino):
  -5V = 5V (3.3V is allowed)
  -GND = GND
  -SDA = A4 (or SDA)
  -SCL = A5 (or SCL)
  -INT = Not connected

  The MAX30105 Breakout can handle 5V or 3.3V I2C logic. We recommend powering the board with 5V
  but it will also run at 3.3V.
*/

#include <Wire.h>
#include "MAX30105.h"
#include "heartRate.h"
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>

MAX30105 particleSensor;

int preCount = 0;
int count = 0;
int isEnd = 0;
long ir[50];
long red[50];
const char *ssid = "UCInet Mobile Access"; // WiFi settings
const char* host = "169.234.47.150"; // Server host settings
const int httpPort = 80;

void setup()
{
  Serial.begin(115200);
  Serial.println("Initializing...");

  // Initialize sensor
  if (!particleSensor.begin(Wire, I2C_SPEED_FAST)) //Use default I2C port, 400kHz speed
  {
    Serial.println("MAX30105 was not found. Please check wiring/power. ");
    while (1);
  }
  Serial.println("Place your index finger on the sensor with steady pressure.");

  particleSensor.setup(); //Configure sensor with default settings

  //LED Pulse Amplitude Configuration
  //-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
  //Default is 0x1F which gets us 6.4mA
  //powerLevel = 0x02, 0.4mA - Presence detection of ~4 inch
  //powerLevel = 0x1F, 6.4mA - Presence detection of ~8 inch
  //powerLevel = 0x7F, 25.4mA - Presence detection of ~8 inch
  //powerLevel = 0xFF, 50.0mA - Presence detection of ~12 inch
  byte powerLevel = 0xFF;
  particleSensor.setPulseAmplitudeRed(powerLevel);
  particleSensor.setPulseAmplitudeIR(powerLevel);

  WiFi.begin(ssid); //, password);
  while (WiFi.status() != WL_CONNECTED){
      delay(500);
      Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
  // Print the IP address
  Serial.println(WiFi.localIP());
}

void loop()
{
  unsigned long irValue = particleSensor.getIR();
  unsigned long redValue = particleSensor.getRed();

  if(preCount<100){
    preCount++;
    //don't save
    Serial.print(preCount);
    Serial.print(",");
  }else if(count<6000){
    //save to array
    ir[count%50]=irValue;
    red[count%50]=redValue;
    count++;
    /*
    Serial.print(count);
    Serial.print(",");
    */
    Serial.print("IR=");
    Serial.print(irValue);
    Serial.print(", RED=");
    Serial.print(redValue);
    Serial.print(", count=");
    Serial.print(count);
    Serial.println();
  }else if(isEnd==0){
    isEnd=1;
  }else{
    Serial.print("E");
  }

  if(isEnd==0 && count!=0 && count%50==0){
    //make JSON
    String postData = "{\"data\":[";
    for(int i=(count-50); i<count; i++){
      postData += "{\"NUM\":4,\"SEQ\":";
      postData += i;
      postData += ",\"IR\":";
      postData += ir[i%50];
      postData += ",\"RED\":";
      postData += red[i%50];
      postData += "}";
      if(i!=(count-1)) postData += ",";
    }
    postData += "]}";
    //Serial.println(postData);

    //send to server
    WiFiClient client;
    if (!client.connect(host, httpPort)){ //conncect with the host
      Serial.println("connection failed");
      return;
    }

    String request = "POST /hw2.php HTTP/1.1\r\nHost: ";
    request += host;
    request += "\r\nAccept: /\r\nContent-Type: application/json\r\nContent-Length: ";
    request += postData.length();
    request += "\r\n\r\n";
    request += postData;

    Serial.println(postData.length());
    client.print(request);
    
    /*
    int pi;
    for(int postInpidex=0; pi<postData.length(); pi+=1000){
      client.print(postData.substring(pi,pi+1000));
    }
    client.print(postData.substring(pi));
    client.print("\r\n\r\n");*/
    //client.print("GET /hw1.php HTTP/1.1\r\nHost: 169.234.35.98\r\nConnection: close\r\nContent-Length:\r\n\r\n");
    //client.print(s);
    
    while(client.available()>0){
      String r = client.readStringUntil('\r');
      Serial.print(r);
    }
    if(client.connected()) client.stop();
    
  }
}