#include <WiFi.h>
#include <PubSubClient.h> 

//Lib pour AES
#include <CryptoAES_CBC.h>
#include <AES.h>
#include <string.h>

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

// Clé AES 128 bits (16 octets)
byte key[16] = {0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F};

// Message à crypter : "amine" (nous devons le remplir avec des zéros pour qu'il ait une longueur de 16 octets)
byte plaintext[16] = {'a', 'm', 'i', 'n', 'e', 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

// Tableau pour stocker le texte chiffré
byte cypher[16];

// Tableau pour stocker le texte déchiffré
byte decryptedtext[16];

// Création de l'objet AES128
AES128 aes128;


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
  // Définir la clé pour AES
  aes128.setKey(key, 16);

 }
void loop() {
  distanceCm = read_ultra(0);

  while (distanceCm > 15 ){
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
char* encryptedMessage = encryptMessage(cypher, plaintext, sizeof(cypher));

// Copy encrypted message into the 'message' array
strncpy(message, encryptedMessage, sizeof(message) - 1);
message[sizeof(message) - 1] = '\0';  // Ensure null-termination

// Don't forget to delete the dynamically allocated memory after use

  //dtostrf(distanceCm, 6, 2, message); // Convert float to string
  client.publish(topic, encryptedMessage); // Publish message
  client.publish(topic, message); // Publish message
  Serial.println("Published: " + payload);
  delay(1000); 
  delete[] encryptedMessage;

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
char* encryptMessage(byte * cypher, byte * plaintext, size_t cypherSize) {
  // Chiffrement
  aes128.encryptBlock(cypher, plaintext);
  Serial.println();
  String encoded = ""; 

  // Afficher le texte chiffré
  Serial.print("Après Chiffrement: ");
  for (int j = 0; j < cypherSize; j++) {
    Serial.write(cypher[j]);
    encoded += String((char)cypher[j]);  // Add byte to the encoded string
  }

  char* se = new char[encoded.length() + 1];  // Dynamically allocate memory for the char array
  stringToCharArray(encoded, se);

  return se;
}
void stringToCharArray(const String& str, char* charArray) {
  // Ensure the string size fits into the char array
  if (str.length() >= 256) {
    Serial.println("Error: String is too long to fit into char[256].");
    return;
  }

  // Copy the string into the char array
  memcpy(charArray, str.c_str(), str.length());

  // Null-terminate the char array
  charArray[str.length()] = '\0';
}
