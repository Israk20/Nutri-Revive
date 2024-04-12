#include <Arduino.h>
#include <DHT.h>
#include <NewPing.h>

#include "HX711.h"

//Tong 1
#define DHT_PIN 8
#define DHT_TYPE DHT11 
#define CALIBRATION_FACTOR 0 ///input nilai
#define TRIGGER_PIN1  12  
#define ECHO_PIN1     11  
#define TRIGGER_PIN2  10  
#define ECHO_PIN2     9
#define MAX_DISTANCE 100000000
#define AOUT_PIN A0 //Soil Moisture Sensor
#define Relay 22

const int LOADCELL_DOUT_PIN = 7;
const int LOADCELL_SCK_PIN = 6;

int value;
int reading;
HX711 scale;
DHT dht(DHT_PIN, DHT_TYPE);

String Gabung,berat,temperatur_udara,kelembaban,kepenuhan,kelembaban_tanah;
//Tong 2




void setup() {
  Serial.begin(9600); 

  //Ultra Sonic
  pinMode(TRIGGER_PIN1, OUTPUT);
  pinMode(ECHO_PIN1, INPUT);
  pinMode(TRIGGER_PIN2, OUTPUT);
  pinMode(ECHO_PIN2, INPUT);

  //DHT
  dht.begin(); 

  //Load Cell
  scale.begin(LOADCELL_DOUT_PIN, LOADCELL_SCK_PIN);
  scale.set_scale(CALIBRATION_FACTOR);
  scale.tare();    

  //Relay
  pinMode(Relay, OUTPUT);           
}

void loop() {
  // Load Cell
  scale.tare();
  if (scale.wait_ready_timeout(200)) {
    reading = round(scale.get_units());
  }
  String berat = String(reading);

  // DHT
  float humidity = dht.readHumidity();
  float temperature = dht.readTemperature();
  if (isnan(humidity) || isnan(temperature)) {
  }
  String temperatur_udara = String(temperature);
  String kelembaban = String(humidity);

  // UltraSonic
  //sensor 1
  long duration1, distance1;
  digitalWrite(TRIGGER_PIN1, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIGGER_PIN1, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIGGER_PIN1, LOW);
  duration1 = pulseIn(ECHO_PIN1, HIGH);
  distance1 = (duration1 / 2) * 0.0343;

  //sensor 2
  long duration2, distance2;
  digitalWrite(TRIGGER_PIN2, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIGGER_PIN2, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIGGER_PIN2, LOW);
  duration2 = pulseIn(ECHO_PIN2, HIGH);
  distance2 = (duration2 / 2) * 0.0343;

  if (distance1 <= 7 && distance2 <= 7){
    Serial.print("#Penuh");
  }
  else{
    Serial.print("#Belum_Penuh");
  }

  //Soil Moisture
  value = analogRead(AOUT_PIN);
  String kelembaban_tanah = String(value);

  String Gabung = "#"+berat+"#"+temperatur_udara+"#"+kelembaban+"#"+kelembaban_tanah;

  Serial.println(Gabung);

  //Relay
  if (temperature >= 55){
    digitalWrite(Relay, HIGH);
    delay(10000);
    digitalWrite(Relay, LOW);
  }
  else{
  }

  delay(5000);
}