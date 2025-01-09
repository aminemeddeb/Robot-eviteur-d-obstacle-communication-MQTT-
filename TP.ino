#include <WiFi.h>
#include <PubSubClient.h> 
#include "AESLib.h"


const char* ssid = "ssid";
const char* password = "password";
const char* mqttServer = "192.168.1.29";
const int mqttPort = 1883;
const char* topic = "distance";
//__________
#include <ESP32Servo.h>
#define PIN_SG90 23 // Output pin used

#define BR 15 // Output pin used
#define BL 21 // Output pin used
#define FR 2 // Output pin used
#define FL 12 // Output pin used

Servo sg90;



const int trigPin = 5;
const int echoPin = 18;

WiFiClient espClient;
PubSubClient client(espClient);

//define sound speed in cm/uS
#define SOUND_SPEED 0.034

long duration;
float distanceCm;
float distanceCmr;
float distanceCml;
float distanceInch;
int pos ;

void setup() {
  sg90.setPeriodHertz(50); // PWM frequency for SG90
  sg90.attach(PIN_SG90, 15, 2400); // Minimum and maximum pulse width (in µs) to go from 0° to 180

  Serial.begin(115200); // Starts the serial communication
  pinMode(trigPin, OUTPUT); // Sets the trigPin as an Output
  pinMode(echoPin, INPUT); // 

  pinMode(BL, OUTPUT); // HB
  pinMode(BR, OUTPUT); // HB
  pinMode(FL, OUTPUT); // HB
  pinMode(FR, OUTPUT); // HB

  pinMode(12, OUTPUT); // SERVO

  sg90.write(90); 
  delay(1000);
  
  //_________________
   WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");
  client.setServer(mqttServer, mqttPort);
  while (!client.connected()) {
    Serial.println("Connecting to MQTT...");
    if (client.connect("ESP32Client")) {
      Serial.println("Connected to MQTT");
    } else {
      Serial.print("Failed, rc=");
      Serial.println(client.state());
      delay(2000);
    }
  }
 }
void loop() {
  distanceCm = read_ultra(0);

  while (distanceCm > 10 ){
    forward() ; 
    stop();
    distanceCm= read_ultra(0);
  } 
  sg90.write(0); 
  delay(1000);
  distanceCml = read_ultra(-1);
  sg90.write(178); 
  delay(1000);
  distanceCmr= read_ultra(1);
  sg90.write(90); 
  delay(1000);
  if (distanceCmr >=distanceCml)left();
  else right() ; 
  stop();
 String payload = String(float (distanceCm),2);
  char message[256];
   encryptMessage(payload.c_str(), message, sizeof(message));

  dtostrf(distanceCm, 6, 2, message); // Convert float to string
  client.publish(topic, message); // Publish message
  Serial.println("Published: " + payload);
  delay(1000); 
}
void forward(){
  digitalWrite(FL,100);
  digitalWrite(FR,100);
  digitalWrite(BL,0);
  digitalWrite(BR,0);
  delay(60);
}
void right(){
  digitalWrite(FL,100);
  digitalWrite(FR,0);
  digitalWrite(BL,0);
  digitalWrite(BR,100);
  delay(200);
}
void left(){
  digitalWrite(FL,0);
  digitalWrite(FR,100);
  digitalWrite(BL,100);
  digitalWrite(BR,0);
  delay(200);
}
void stop(){
  digitalWrite(FL,0);
  digitalWrite(FR,0);
  digitalWrite(BL,0);
  digitalWrite(BR,0);
  delay(50);
}

float read_ultra(int a ){
  // Clears the trigPin
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  // Sets the trigPin on HIGH state for 10 micro seconds
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  
  // Reads the echoPin, returns the sound wave travel time in microseconds
  duration = pulseIn(echoPin, HIGH);
  
  // Calculate the distance
  distanceCm = duration * SOUND_SPEED/2;
  
  // Prints the distance in the Serial Monitor
  if ( a ==0 ){
Serial.print("Distance (cm): ");
  Serial.println(distanceCm);
 
  }
  else if ( a == -1 )
{
Serial.print("DistanceL (cm): ");
  Serial.println(distanceCm);
 
}
  else {
  Serial.print("DistanceR (cm): ");
  Serial.println(distanceCm);
 
  }
  
  delay(1000);
  return distanceCm;

}
void encryptMessage(const char* input, char* output, size_t output_size) {
  
}
