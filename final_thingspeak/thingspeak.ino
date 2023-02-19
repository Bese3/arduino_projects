#include <SoftwareSerial.h>
#include<DHT.h>       
SoftwareSerial espSerial(10, 11);      
#define DEBUG true
#define red  7
#define green 6
#define buzzer 5
#define DHTTYPE DHT22
String mySSID = "Codak";       // WiFi SSID
String myPWD = "lemlem064"; // WiFi Password
String myAPI = "WK6HT6APPUC8ZVAS";   // API Key
String myHOST = "api.thingspeak.com";
String myPORT = "80";
String myFIELD = "field1"; 
int sendVal;



int sensorvalue;
int sensorvalue2 = 10; //address
int tempsensor = 2;//address
int watersensor = 0;//value
int vibration_state = 0;
const int analogPin = A0;//address
DHT dht(tempsensor , DHTTYPE);

//analoge  value of  MQ 135 sensor are fed in to A0 pin
//sensorvalue is directly correlated to Co2 value of the room
//sensorvalue2 is correlated to the vibration level of the bed
//buzzer holds the data of the alarming system 
//red and green are the pin that control the LED outputs
//tempsensor reads the value of the temperature
//watersensor holds information about the level of water content in the bed



void setup()
{
  Serial.begin(9600);
  espSerial.begin(115200);
  pinMode(sensorvalue2 , INPUT);
    dht.begin(); //initiating the sensor
  
  espData("AT+RST", 1000, DEBUG);                      //Reset the ESP8266 module
  espData("AT+CWMODE=1", 1000, DEBUG);                 //Set the ESP mode as station mode
  espData("AT+CWJAP=\""+ mySSID +"\",\""+ myPWD +"\"", 1000, DEBUG);   //Connect to WiFi network
  /*while(!esp.find("OK")) 
  {          
      //Wait for connection
  }*/
pinMode(red , OUTPUT);
pinMode(green , OUTPUT);
pinMode(buzzer , OUTPUT);  
  delay(1000);
  
}

  void loop()
  {
  

//sensorvalue = analogRead(A0);


float tempc = dht.readTemperature();
if(isnan(tempc))
        Serial.print("failed to read DHT 22\n");

        
else
{
  
    Serial.print("Temperature: ");
    Serial.print(tempc);
    Serial.print("°C ~ ");
}
if (tempc >= 33)
{
Serial.print("Temprature Level Rising to: ");
Serial.print(tempc);
Serial.print("°C");
}
vibration_state = digitalRead(sensorvalue2);
if (vibration_state == 1)
{
 digitalWrite(red , HIGH);
 digitalWrite(green , LOW);
 Serial.print("sensing some Vibration\n");
delay(1000); 

}
else
{
digitalWrite(red , LOW);
digitalWrite(green ,HIGH);
Serial.print("No Vibration\n");

}
watersensor = analogRead(analogPin);
Serial.print(watersensor);
Serial.print("\n");
if(watersensor >=200)
{
    digitalWrite(red,HIGH);
    delay(100);

}
 
    
    sendVal = tempc; // Send a random number between 1 and 1000
    String sendData = "GET /update?api_key="+ myAPI +"&"+ myFIELD +"="+String(sendVal);
    espData("AT+CIPMUX=1", 1000, DEBUG);       //Allow multiple connections
    espData("AT+CIPSTART=0,\"TCP\",\""+ myHOST +"\","+ myPORT, 1000, DEBUG);
    espData("AT+CIPSEND=0," +String(sendData.length()+4),1000,DEBUG);  
      if (sendVal > 600)
    {
      Serial.println("high temprature");
    }
    espSerial.find(">"); 
    espSerial.println(sendData);
    Serial.print("Value to be sent: ");
    Serial.println(sendVal);
     
    espData("AT+CIPCLOSE=0",1000,DEBUG);
    delay(10000);
  }

  String espData(String command, const int timeout, boolean debug)
{
  Serial.print("AT Command ==> ");
  Serial.print(command);
  Serial.println("     ");
  
  String response = "";
  espSerial.println(command);
  long int time = millis();
  while ( (time + timeout) > millis())
  {
    while (espSerial.available())
    {
      char c = espSerial.read();
      response += c;
    }
  }
  if (debug)
  {
    // Serial.print(response);
  }
  return response;
}