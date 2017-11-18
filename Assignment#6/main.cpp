#include "Wire.h"
#include "SPI.h"
#include "MAX30105.h"
#include "ESP8266WiFi.h"
#include "ESP8266HTTPClient.h"
#include "SparkFunLIS3DH.h"

MAX30105 ppgSensor;
LIS3DH accelSensor(SPI_MODE, 0);

int preCount = 0;
int count = 0;
int isEnd = 0;
long ir[50];
long red[50];
float X[50];
float Y[50];
float Z[50];
const char *ssid = "UCInet Mobile Access"; // WiFi settings
const char* host = "169.234.38.220"; // Server host settings
const int httpPort = 80;
const int activityNum = 2;
  
void setup()
{
  Serial.begin(115200);
  Serial.println("Initializing...");

  // Initialize sensor
  if (!ppgSensor.begin(Wire, I2C_SPEED_FAST)) //Use default I2C port, 400kHz speed
  {
    Serial.println("MAX30105 was not found. Please check wiring/power. ");
    while (1);
  }
  Serial.println("Place your index finger on the sensor with steady pressure.");

  ppgSensor.setup();

  uint8_t returnData = 0;
  returnData = accelSensor.begin();
  Serial.print("accelSensor returnData=");
  Serial.println(returnData);
  if(( returnData != 0x00 )&&( returnData != 0xFF ))
  {
	  Serial.println("Problem starting the accel sensor with CS @ Pin 0.");
  }
  else
  {
	  Serial.println("Accel sensor with CS @ Pin 0 started.");
  }

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
  long irValue = ppgSensor.getIR();
  long redValue = ppgSensor.getRed();
  float xValue = accelSensor.readFloatAccelX();
  float yValue = accelSensor.readFloatAccelY();
  float zValue = accelSensor.readFloatAccelZ();

  if(preCount<50*5){
    preCount++;
    //don't save
    Serial.print(preCount);
    Serial.print(",");
  }else if(count<50*60*10){
    //save to array
    ir[count%50] = irValue;
    red[count%50] = redValue;
    X[count%50] = xValue;
    Y[count%50] = yValue;
    Z[count%50] = zValue;
    count++;

    /*Serial.print("activityNum=");
    Serial.print(activityNum);
    Serial.print(", SEQ=");
    Serial.print(count);
    Serial.print(", IR=");
    Serial.print(irValue);
    Serial.print(", RED=");
    Serial.print(redValue);
    Serial.print(", X=");
    Serial.print(xValue);
    Serial.print(", Y=");
    Serial.print(yValue);
    Serial.print(", Z=");
    Serial.println(zValue);*/
    //Serial.println(count);
  }else if(isEnd==0){
    isEnd=1;
  }else{
    Serial.print("End.");
  }

  if(isEnd==0 && count!=0 && count%50==0){
    //make JSON
    String postData = "{\"data\":[";
    for(int i=(count-50); i<count; i++){
      postData += "[";
      postData += activityNum;
      postData += ",";
      postData += i;
      postData += ",";
      postData += ir[i%50];
      postData += ",";
      postData += red[i%50];
      postData += ",";
      postData += X[i%50];
      postData += ",";
      postData += Y[i%50];
      postData += ",";
      postData += Z[i%50];
      postData += "]";
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

    String request = "POST /hw6.php HTTP/1.1\r\nHost: ";
    request += host;
    request += "\r\nAccept: /\r\nContent-Type: application/json\r\nContent-Length: ";
    request += postData.length();
    request += "\r\n\r\n";
    request += postData;

    //Serial.print("POST length="); //<=2600 is ok
    //Serial.println(postData.length());
    client.print(request);
    /*
    while(client.available()>0){
      String r = client.readStringUntil('\r');
      Serial.print(r);
    }*/
    if(client.connected()) client.stop();
    
  }
}