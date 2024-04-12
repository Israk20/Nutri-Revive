#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <Firebase_ESP_Client.h>
#include "SoftwareSerial.h"

const int trigPin = D1; // Connect to TRIG pin on HC-SR04
const int echoPin = D2; // Connect to ECHO pin on HC-SR04

// Provide the token generation process info.
#include "addons/TokenHelper.h"
// Provide the RTDB payload printing info and other helper functions.
#include "addons/RTDBHelper.h"

// Insert your network credentials
#define WIFI_SSID "Blue_House"
#define WIFI_PASSWORD "--------"

// Insert Firebase project API Key
#define API_KEY "AIzaSyCxJh_CeQUpWWI0aSnD0DwT4WRkq1ifb-c"

// Insert RTDB URL
#define DATABASE_URL "https://nutri-revive-default-rtdb.asia-southeast1.firebasedatabase.app/"

// Define Firebase Data object
FirebaseData fbdo;

FirebaseAuth auth;
FirebaseConfig config;

unsigned long sendDataPrevMillis = 0;
bool signupOK = false;
String pesan;
String h_data[12];
int serialStat;

bool ParsePlease = false; 

void setup() {
  Serial.begin(9600);

  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);

  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting to Wi-Fi");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(300);
  }
  Serial.println();
  Serial.print("Connected with IP: ");
  Serial.println(WiFi.localIP());
  Serial.println();

  /* Assign the api key (required) */
  config.api_key = API_KEY;

  /* Assign the RTDB URL (required) */
  config.database_url = DATABASE_URL;

  /* Sign up */
  if (Firebase.signUp(&config, &auth, "", "")) {
    Serial.println("ok");
    signupOK = true;
  }
  else {
    Serial.printf("%s\n", config.signer.signupError.message.c_str());
  }

  /* Assign the callback function for the long running token generation task */
  config.token_status_callback = tokenStatusCallback; // See addons/TokenHelper.h

  Firebase.begin(&config, &auth);
  Firebase.reconnectWiFi(true);
}

void loop() {
  
  serialStat = Serial.available();
  if (serialStat > 0) {
    Serial.println("connect");
    pesan = Serial.readString();
    Serial.println(pesan);
    ParsePlease = true;
  }
  else{
    Serial.println("ga connect");
  }

  if (ParsePlease) {
    Serial.println("masuk");
    int x = 0;
    for (int i = 0; i < 12; i++) {
      h_data[i] = "";
    }
    for (int i = 0; i < pesan.length(); i++) {
      if (pesan[i] == '#') {
        x++; 
        h_data[x] = "";
      }
      else {
        h_data[x] = h_data[x] + pesan[i];
      }
    }

    String kondisi = h_data[1];
    String berat = h_data[2];
    String suhu = h_data[3];
    String kelembaban_udara = h_data[4];
    String kelembaban_tanah = h_data[5];
    
    if (Firebase.RTDB.setString(&fbdo, "Kepenuhan", kondisi)) {
      Serial.println("PASSED");
    } else {
      Serial.println("FAILED");
      Serial.println("REASON: " + fbdo.errorReason());
    }

    if (Firebase.RTDB.setInt(&fbdo, "Berat", berat.toInt())) {
      Serial.println("PASSED");
    } else {
      Serial.println("FAILED");
      Serial.println("REASON: " + fbdo.errorReason());
    }

    if (Firebase.RTDB.setFloat(&fbdo, "Suhu", suhu.toInt())) {
      Serial.println("PASSED");
    } else {
      Serial.println("FAILED");
      Serial.println("REASON: " + fbdo.errorReason());
  }

    if (Firebase.RTDB.setInt(&fbdo, "Kelembaban_udara", kelembaban_udara.toFloat())) {
      Serial.println("PASSED");
    } else {
      Serial.println("FAILED");
      Serial.println("REASON: " + fbdo.errorReason());
    }

    if (Firebase.RTDB.setFloat(&fbdo, "Kelembaban_tanah", kelembaban_tanah.toFloat())) {
      Serial.println("PASSED");
    } else {
      Serial.println("FAILED");
      Serial.println("REASON: " + fbdo.errorReason());
  }

  ParsePlease = false;
}
}
