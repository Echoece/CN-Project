
#include <Arduino.h>
#if defined(ESP32)
  #include <WiFi.h>
#elif defined(ESP8266)
  #include <ESP8266WiFi.h>
#endif
#include <Firebase_ESP_Client.h>

//Provide the token generation process info.
#include "addons/TokenHelper.h"
//Provide the RTDB payload printing info and other helper functions.
#include "addons/RTDBHelper.h"

// Insert your network credentials
#define WIFI_SSID "Rafiqul"
#define WIFI_PASSWORD "echo6798"

// Insert Firebase project API Key
#define API_KEY "AIzaSyC7oe0SdU6qsvsBau_Jefev1zNBBo7YVuk"

// Insert RTDB URLefine the RTDB URL */
#define DATABASE_URL "https://esp-firebase-demo-50d97-default-rtdb.asia-southeast1.firebasedatabase.app/" 

//define sound velocity in cm/uS
#define SOUND_VELOCITY 0.034

const int trigPin = 4;
const int echoPin = 16;

//Define Firebase Data object
FirebaseData fbdo;
FirebaseAuth auth;
FirebaseConfig config;

// variable for firebase signup 
bool signupOK = false;

// variables for calculatnig distance
long duration;
float distanceCm;
float distanceInch;

void setup(){
  pinMode(trigPin, OUTPUT); // Sets the trigPin as an Output
  pinMode(echoPin, INPUT); // Sets the echoPin as an Input
  Serial.begin(9600);
  connectToWifi();
  connectToFireBase();
}

void loop(){
  // pulseIn() returns length of the pulse in microseconds
  // millis() returns the number of milli second the machine is started, 
  if (Firebase.ready() && signupOK){
    float distance = getDistance();
    writeData(distance);
  }
}

void writeData(float data){
  // Write an Float number on the database path test/float
    if (Firebase.RTDB.setFloat(&fbdo, "test/float", data)){
      Serial.println("PASSED");
      Serial.println("PATH: " + fbdo.dataPath());
      Serial.println("TYPE: " + fbdo.dataType());
    }
    else {
      Serial.println("FAILED");
      Serial.println("REASON: " + fbdo.errorReason());
    }
}

void connectToFireBase(){
  /* Assign the api key (required) */
  config.api_key = API_KEY;

  /* Assign the RTDB URL (required) */
  config.database_url = DATABASE_URL;

  /* Sign up */
  if (Firebase.signUp(&config, &auth, "", "")){
    Serial.println("ok");
    signupOK = true;
  }
  else{
    Serial.printf("%s\n", config.signer.signupError.message.c_str());
  }

  /* Assign the callback function for the long running token generation task */
  config.token_status_callback = tokenStatusCallback; //see addons/TokenHelper.h
  
  Firebase.begin(&config, &auth);
  Firebase.reconnectWiFi(true);  
}

void connectToWifi(){
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting to Wi-Fi");
  while (WiFi.status() != WL_CONNECTED){
    Serial.print(".");
    delay(300);
  }
  Serial.println();
  Serial.print("Connected with IP: ");
  Serial.println(WiFi.localIP());
  Serial.println();
}


float getDistance(){
  // Clears the trigPin
  digitalWrite(trigPin, LOW);
  delayMicroseconds(5);
  
  // Sets the trigPin on HIGH state for 10 micro seconds
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  
  digitalWrite(trigPin, LOW);
  
  // Reads the echoPin, returns the sound wave travel time in microseconds
  duration = pulseIn(echoPin, HIGH);
  
  // Calculate the distance
  distanceCm = duration * SOUND_VELOCITY/2;
  
  // Prints the distance on the Serial Monitor
  Serial.print("Distance (cm): ");
  Serial.println(distanceCm);


  delay(1000);
  return distanceCm;
}


