#include <WiFi.h>
#include <PubSubClient.h> 
#include "AESLib.h"


const char* ssid = "Ooredoo-13244C";
const char* password = "vv86&$NM";
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
  unsigned char key[16] = {0x2b, 0x7e, 0x15, 0x16, 0x28, 0xae, 0xd2, 0xa6, 0xab, 0xf7, 0x97, 0x75, 0x46, 0x8f, 0x51, 0x8f};
  unsigned char iv[16] = {0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f};

  mbedtls_aes_context aes;
  mbedtls_aes_init(&aes);

  mbedtls_aes_setkey_enc(&aes, key, 128);

  size_t input_len = strlen(input);
  size_t padded_len = ((input_len / 16) + 1) * 16;
  unsigned char* input_padded = (unsigned char*)malloc(padded_len);
  memset(input_padded, 0, padded_len);
  memcpy(input_padded, input, input_len);

  mbedtls_aes_crypt_cbc(&aes, MBEDTLS_AES_ENCRYPT, padded_len, iv, input_padded, (unsigned char*)output);

  mbedtls_aes_free(&aes);
  free(input_padded);

  // Base64 encode the encrypted output
  size_t encoded_len = base64_encode(NULL, 0, output, padded_len);
  base64_encode(output, encoded_len, output, padded_len);
}
