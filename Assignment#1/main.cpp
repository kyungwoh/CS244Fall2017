#include "Arduino.h"
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>


String deviceName = "CS244";

// WiFi settings
const char *ssid = "UCInet Mobile Access";

// Server host settings
const char* host = "169.234.13.13";

void printMacAddress()
{
    byte mac[6];
    WiFi.macAddress(mac);

    char MAC_char[18]="";
    for (int i = 0; i < sizeof(mac); ++i)
    {
        sprintf(MAC_char, "%s%02x:", MAC_char, mac[i]);
    }

    Serial.print("Mac address : ");
    Serial.print(MAC_char); //print MAC address

    WiFi.begin(ssid); //, password);
    while (WiFi.status() != WL_CONNECTED)
    {
        delay(500);
        Serial.print(".");
    }
    Serial.println("");
    Serial.println("WiFi connected");
    // Print the IP address
    Serial.println(WiFi.localIP());

}


void setup()
{
  // initialize LED digital pin as an output.
  pinMode(LED_BUILTIN, OUTPUT);
    
  Serial.begin(115200);

  delay(10000); //because of Serial Monitor buffer
  Serial.println("Program started");
  printMacAddress();
  
}

void loop()
{
  // You can write your logic here

  WiFiClient client;
  const int httpPort = 80;
  if (!client.connect(host, httpPort)){ //conncect with the host
    Serial.println("connection failed");
    return;
  }


  //send data to server
  Serial.println();
  Serial.println("Sending Data 555");
  
  client.print("GET /hw1.php?input=555 HTTP/1.1\r\nHost: 169.234.13.13\r\nConnection: close\r\n\r\n");
  delay(1000);
  
  // Below are the LED samples from the hands-on

  // turn the LED on (HIGH is the voltage level)
  digitalWrite(LED_BUILTIN, HIGH);
  // wait for a second
  delay(1000);
  // turn the LED off by making the voltage LOW
  digitalWrite(LED_BUILTIN, LOW);
   // wait for a second
  delay(1000);
  
}