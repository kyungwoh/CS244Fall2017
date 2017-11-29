#include "Wire.h"
#include "SPI.h"
#include "MAX30105.h"
#include "ESP8266WiFi.h"
#include "ESP8266HTTPClient.h"
#include "SparkFunLIS3DH.h"
#include "FS.h"

MAX30105 ppgSensor;
LIS3DH accelSensor(SPI_MODE, 0);

int preCount = 0;
int count = 0;
int isEnd = 0;
const char *ssid = "UCInet Mobile Access"; // WiFi settings
const char *host = "169.234.18.161"; // Server host settings
const int httpPort = 80;
//const int activityNum = 2;
//const int sendSize = 40000;
const int sendSize = 25*75;

unsigned long lastMillis = millis();

//const uint8_t interruptPin = 13;

File fWrite, fRead;

unsigned long getMillis(){
  unsigned long lastlastMillis = lastMillis;
  lastMillis = millis();
  return lastMillis - lastlastMillis;
}

void sendToServer(){
  Serial.print("sendToServer Start getMillis()=");
  Serial.println(getMillis());
  //digitalWrite(BUILTIN_LED, LOW);

  fWrite.close();

  // Serial.print("host=");
  // Serial.println(host);

  // Serial.print("httpPort=");
  // Serial.println(httpPort);

  WiFiClient client;
  int return1 = client.connect(host, httpPort);
  if (!return1){ //conncect with the host
    Serial.print("connection failed getMillis()=");
    Serial.println(getMillis());
    return;
  }

  fRead = SPIFFS.open("/f.dat", "r");
  if(fRead) Serial.println("fRead OK");
  else Serial.println("fRead FAIL");
  int fileLength = fRead.size();

  Serial.print("fileLength=");
  Serial.println(fileLength);

  String request = "POST /hw7.php HTTP/1.1\r\nHost: ";
  request += host;
  request += "\r\nAccept: /\r\nContent-Type: application/json\r\nContent-Length: ";
  request += fileLength;
  request += "\r\n\r\n";
  client.print(request);

  String postData;
  int i;
  for(int pi=0; pi<fileLength;){
    postData ="";
    i=0;

    while (i<1460 && fRead.available()){
      postData += char(fRead.read());
      i++;
      pi++;
    }
    client.print(postData);
  }
  
  client.print("\r\n\r\n");

  if(client.connected()) client.stop();
  fRead.close();

  Serial.print("sendToServer End getMillis()=");
  Serial.println(getMillis());

  if(SPIFFS.format()) Serial.println("SPIFFS.format() OK");
  else Serial.println("SPIFFS.format() FAIL");

  fWrite = SPIFFS.open("/f.dat", "w");
  if (fWrite) Serial.println("fWrite OK");
  else Serial.println("fWrite FAIL");

  Serial.print("after fWrite getMillis()=");
  Serial.println(getMillis());
}

void recordSensorData(){
  //digitalWrite(interruptPin, LOW);
  if(count<7500){
    //while (ppgSensor.available() == false) //do we have new data?
    ppgSensor.check(); //Check the sensor for new data
    uint32_t red = ppgSensor.getRed();
    uint32_t ir = ppgSensor.getIR();
    ppgSensor.nextSample();

    float x = accelSensor.readFloatAccelX();
    float y = accelSensor.readFloatAccelY();
    float z = accelSensor.readFloatAccelZ();


    // Serial.print("before getMillis()=");
    // Serial.println(getMillis());

    // fWrite.print(activityNum);
    // fWrite.print(',');
    // fWrite.print(count);
    // fWrite.print(',');
    // fWrite.print(ir);
    // fWrite.print(',');
    // fWrite.print(red);
    // fWrite.print(',');
    // fWrite.print(x);
    // fWrite.print(',');
    // fWrite.print(y);
    // fWrite.print(',');
    // fWrite.println(z);

    // fWrite.write(count);
    uint8_t ir8[4], red8[4], x8[4], y8[4], z8[4];
    *(uint32_t *)ir8 = ir;
    *(uint32_t *)red8 = red;
    *(float *)x8 = x;
    *(float *)y8 = y;
    *(float *)z8 = z;
    fWrite.write(ir8[0]);
    fWrite.write(ir8[1]);
    fWrite.write(ir8[2]);
    fWrite.write(ir8[3]);
    fWrite.write(red8[0]);
    fWrite.write(red8[1]);
    fWrite.write(red8[2]);
    fWrite.write(red8[3]);
    fWrite.write(x8[0]);
    fWrite.write(x8[1]);
    fWrite.write(x8[2]);
    fWrite.write(x8[3]);
    fWrite.write(y8[0]);
    fWrite.write(y8[1]);
    fWrite.write(y8[2]);
    fWrite.write(y8[3]);
    fWrite.write(z8[0]);
    fWrite.write(z8[1]);
    fWrite.write(z8[2]);
    fWrite.write(z8[3]);

    // Serial.print("activityNum=");
    // Serial.print(activityNum);
    // Serial.print("SEQ=");
    // Serial.print(count);
    // Serial.print(", IR=");
    // Serial.print(ir);
    // Serial.print(", RED=");
    // Serial.print(red);
    // Serial.print(", X=");
    // Serial.print(x);
    // Serial.print(", Y=");
    // Serial.print(y);
    // Serial.print(", Z=");
    // Serial.print(z);
    // Serial.print(", fWrite.size()=");
    // Serial.print(fWrite.size());
    // Serial.print(", getMillis()=");
    // Serial.println(getMillis());

    count++;
    if(count%sendSize==0){
      //digitalWrite(BUILTIN_LED, HIGH);
      sendToServer();
    }
  }else{
    Serial.print("End.");
  }
}

void setup()
{
  Serial.begin(115200);
  Serial.println("Initializing...");

  // Initialize sensor 
  if (!ppgSensor.begin(Wire, I2C_SPEED_FAST)){
    Serial.println("MAX30105 was not found. Please check wiring/power. ");
    while (1);
  }
  Serial.println("Place your index finger on the sensor with steady pressure.");
  byte ledBrightness = 60; //Options: 0=Off to 255=50mA
  byte sampleAverage = 4; //Options: 1, 2, 4, 8, 16, 32
  byte ledMode = 2; //Options: 1 = Red only, 2 = Red + IR, 3 = Red + IR + Green
  int sampleRate = 100; //Options: 50, 100, 200, 400, 800, 1000, 1600, 3200
  int pulseWidth = 411; //Options: 69, 118, 215, 411
  int adcRange = 4096; //Options: 2048, 4096, 8192, 16384
  ppgSensor.setup(ledBrightness, sampleAverage, ledMode, sampleRate, pulseWidth, adcRange); //Configure sensor with these settings
  //ppgSensor.enableAFULL();
  //ppgSensor.setFIFOAlmostFull(3);

  uint8_t returnData = accelSensor.begin();
  if(( returnData != 0x00 )&&( returnData != 0xFF )){
	  Serial.println("Problem starting the accel sensor with CS @ Pin 0.");
  }else{
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


  if(SPIFFS.begin()) Serial.println("SPIFFS.begin() OK");
  else Serial.println("SPIFFS.begin() FAIL");

  if(SPIFFS.format()) Serial.println("SPIFFS.format() OK");
  else Serial.println("SPIFFS.format() FAIL");

  fWrite = SPIFFS.open("/f.dat", "w");
  if (fWrite) Serial.println("fWrite OK");
  else Serial.println("fWrite FAIL");

  // pinMode(BUILTIN_LED, OUTPUT);
  // pinMode(interruptPin, OUTPUT);
  // attachInterrupt(BUILTIN_LED, sendToServer, RISING);
  // attachInterrupt(interruptPin, recordSensorData, RISING);
 
  delay(2000);

  Serial.print("after fWrite getMillis()=");
  Serial.println(getMillis());
}

void loop()
{
  //Serial.println("recordSensorData");
  //digitalWrite(interruptPin, HIGH);
  recordSensorData();
}
