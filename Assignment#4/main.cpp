#include "SparkFunLIS3DH.h"
#include "Wire.h"
#include "SPI.h"
#include "heartRate.h"
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>

LIS3DH myIMU; //Default constructor is I2C, addr 0x19.

int preCount = 0;
int count = 0;
int isEnd = 0;
float X[50];
float Y[50];
float Z[50];
const char *ssid = "UCInet Mobile Access"; // WiFi settings
const char* host = "169.234.56.144"; // Server host settings
const int httpPort = 80;

void setup() {
  Serial.begin(115200);
  Serial.println("Initializing...");
  
  WiFi.begin(ssid); //, password);
  while (WiFi.status() != WL_CONNECTED){
      delay(500);
      Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
  // Print the IP address
  Serial.println(WiFi.localIP());

  delay(1000); //relax...
  Serial.println("Processor came out of reset.\n");
  
  //Call .begin() to configure the IMU
  myIMU.begin();
}

void loop()
{  
  if(preCount<5000){
    preCount++;
    //don't save
    Serial.print(preCount);
    Serial.print(",");
  }else if(count<1000){
    //save to array
    X[count%50]=myIMU.readFloatAccelX();
    Y[count%50]=myIMU.readFloatAccelY();
    Z[count%50]=myIMU.readFloatAccelZ();
    count++;
    Serial.print(count);
    Serial.print(",");
    //Get all parameters
    // Serial.print("\nAccelerometer:\n");
    // Serial.print(" X = ");
    // Serial.println(myIMU.readFloatAccelX(), 4);
    // Serial.print(" Y = ");
    // Serial.println(myIMU.readFloatAccelY(), 4);
    // Serial.print(" Z = ");
    // Serial.println(myIMU.readFloatAccelZ(), 4);
  }else if(isEnd==0){
    isEnd=1;
  }else{
    delay(500);
    Serial.print("E");
  }

  if(isEnd==0 && count!=0 && count%50==0){
    //make JSON
    String postData = "{\"data\":[";
    for(int i=(count-50); i<count; i++){
      postData += "{\"NAME\":\"CIR\",\"SEQ\":";
      postData += i;
      postData += ",\"X\":";
      postData += X[i%50];
      postData += ",\"Y\":";
      postData += Y[i%50];
      postData += ",\"Z\":";
      postData += Z[i%50];
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

    String request = "POST /hw4.php HTTP/1.1\r\nHost: ";
    request += host;
    request += "\r\nAccept: /\r\nContent-Type: application/json\r\nContent-Length: ";
    request += postData.length();
    request += "\r\n\r\n";
    request += postData;

    Serial.println(postData.length());
    client.print(request);
        
    while(client.available()>0){
      String r = client.readStringUntil('\r');
      Serial.print(r);
    }
    if(client.connected()) client.stop();
    
  }
}