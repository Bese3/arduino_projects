#include <MQ135.h>

#include <SoftwareSerial.h>
#include <DHT.h>

SoftwareSerial espSerial(13, 12);   //Pin 2 and 3 act as RX and TX. Connect them to TX and RX of ESP8266
#define DEBUG true
#define red  7
#define green 6
#define buzzer 5
#define DHTTYPE DHT22

String mySSID = "Codak";       // WiFi SSID
String myPWD = "lemlem0646"; // WiFi Password
String myAPI = "FZQHE2NYUZZBJ5CO";   // API Key
String myHOST = "api.thingspeak.com";
String myPORT = "80";
String myFIELD = "field1";
String myFIELD2 = "field2";

int valSensor = 1;
int tempsensor = 2;
int vibsensor = 3;
int MQsensor = A0;
int watersensor = A1;
MQ135 gas_state = MQ135(MQsensor);

DHT dht(tempsensor , DHTTYPE);

void setup() {
  Serial.begin(9600);
  espSerial.begin(115200);

  dht.begin();

  espData("AT+RST", 1000, DEBUG);                      //Reset the ESP8266 module
  espData("AT+CWMODE=1", 1000, DEBUG);                 //Set the ESP mode as station mode
  espData("AT+CWJAP=\"" + mySSID + "\",\"" + myPWD + "\"", 1000, DEBUG);   //Connect to WiFi network


  pinMode(vibsensor , INPUT);
  pinMode(tempsensor , INPUT);
  pinMode(red , OUTPUT);
  pinMode(green , OUTPUT);
  pinMode(buzzer , OUTPUT);
  delay(1000);
}

void loop() {
  normal();
  String getData = "GET /update?api_key=" + myAPI + "&" + myFIELD + "=" + getTemperatureValue() + "&" + myFIELD2 + "=" + getHumidityValue();

  espData("AT+CIPMUX=1", 1000, DEBUG);       //Allow multiple connections
  espData("AT+CIPSTART=0,\"TCP\",\"" + myHOST + "\"," + myPORT, 1000, DEBUG);
  String sendCmd = "AT+CIPSEND=0," + String(getData.length() + 2);
  espData(sendCmd, 1000, DEBUG);
  espSerial.println(getData);
  delay(1000);
}

String getTemperatureValue() {
  float tempc = dht.readTemperature();
  Serial.print("Temperature(C) = ");
  int temp = tempc;
  Serial.println(temp);
  Serial.print("\n");
  if (temp > 35)
       danger();
    else
    normal();
  
  delay(50);
  return String(temp);
}

String getHumidityValue() {
  float humid = dht.readHumidity();
  Serial.print("Humidity in % = ");
  int humidity = humid;
  Serial.println(humidity);
  Serial.print("\n");
  delay(50);
  return String(humidity);
}
String getVibrationValue(){

int vibration = analogRead(vibsensor);
float mapper = map(vibration , 0 , 213 , 0 , 1);

  if (map == 1)
          danger();
  else
      normal();
    mapper = 100*mapper;
    Serial.print("percent of vibration:");
    Serial.print(mapper);
    Serial.print("\n");
    delay(50);

    return String(vibration);
}
String getMqgasValue(){
  int ppm = gas_state.getPPM();
  Serial.print("Rate:");
  Serial.println(ppm);
  Serial.print("PPM\n");
  if (ppm > 800)
        danger();
  else 
      normal();
        delay(50);
    return String(ppm);

}
String getwaterValue(){

int water = analogRead(watersensor);
float mapper = map(water , 0 , 213 , 0 , 1);
if (map == 1)
  danger();
  else
  normal();
  mapper = 100*mapper;
  Serial.print("Percentage Conductivity of Water:");
  Serial.print(mapper);
  Serial.print("\n");
  delay(50);

  return String(water); 
}
void danger(){

  digitalWrite(red , HIGH);
  digitalWrite(green , LOW);
  digitalWrite(buzzer , HIGH);
}
void normal(){
  digitalWrite(green , LOW);
  digitalWrite(red , LOW);
  digitalWrite(buzzer , HIGH);
}

String espData(String command, const int timeout, boolean debug) {
  Serial.print("AT Command ==> ");
  Serial.print(command);
  Serial.println("     ");

  String response = "";
  espSerial.println(command);
  long int time = millis();
  while ((time + timeout) > millis()) {
    while (espSerial.available()) {
      char c = espSerial.read();
      response += c;
    }
  }
  if (debug) {
    Serial.print(response);
  }
  return response;
}
