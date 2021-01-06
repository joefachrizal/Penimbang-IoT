#include <ESP8266WiFi.h>
#include <FirebaseArduino.h>
#include <DHT.h>
#include "HX711.h"

#define FIREBASE_HOST "my-room-iot.firebaseio.com"
#define FIREBASE_AUTH "D9zz030ZzObNz5JOxLjIJQgFYYXVZQohMOgPIPoT"

#define WIFI_SSID "adaWifi"
#define WIFI_PASSWORD "satusampailima"

#define calibration_factor_kg 297700.00 //This value is obtained using the SparkFun_HX711_Calibration sketch
#define calibration_factor_gr 283

#define pinLamp D0
#define pinFan D1
int lampState = LOW;
int fanState = LOW;

#define DHTPIN D2                                                                       // Digital pin connected to DHT11
#define DHTTYPE DHT11                                                                   // Initialize dht type as DHT11
DHT dht(DHTPIN, DHTTYPE);

#define DOUT D3
#define CLK  D4
HX711 scale;

#define indFull D7
#define indMed D6
#define indLow D5

const long interHX711 = 2000;
const long interDHT = 5000;
const long interLamp = 3000;
const long interFan = 4000;
unsigned long preMillis1 = 0;
unsigned long preMillis2 = 0;
unsigned long preMillis3 = 0;
unsigned long preMillis4 = 0;

const long interHX711RT = 250;
unsigned long preMillisRT = 0;

void setup() {
  Serial.begin(115200);
  delay(500);

  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting to ");
  Serial.print(WIFI_SSID);
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }

  pinMode(pinLamp, OUTPUT);
  pinMode(pinFan, OUTPUT);
  pinMode(indFull, OUTPUT);
  pinMode(indMed, OUTPUT);
  pinMode(indLow, OUTPUT);

  Serial.println();
  Serial.print("Connected to ");
  Serial.println(WIFI_SSID);
  Serial.print("IP Address is : ");
  Serial.println(WiFi.localIP());                                                       // Will print local IP address
  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);                                         // connect to firebase

  dht.begin();                                                                          //Start reading dht sensor

  scale.begin(DOUT, CLK);
  scale.set_scale(calibration_factor_gr); //This value is obtained by using the SparkFun_HX711_Calibration sketch
  scale.tare(); //Assuming there is no weight on the scale at start up, reset the scale to 0
}

void loop() {
  if (millis() - preMillis1 >= interDHT) {
    preMillis1 = millis();

    float h = dht.readHumidity();                                                       // Reading Humidity
    float t = dht.readTemperature();                                                    // Read temperature as Celsius

    if (isnan(h) || isnan(t)) {                                                         // Check if any reads failed.
      Serial.println(F("Failed to read from DHT sensor!"));
      return;
    }

    Serial.print("Humidity: ");  Serial.print(h);
    String fireHumid = String(h) + String("%");                                         //convert integer humidity to string humidity
    Serial.print("%  Temperature: ");  Serial.print(t);  Serial.println("°C ");
    String fireTemp = String(t) + String("°C");                                         //convert integer temperature to string temperature
    delay(5000);

    Firebase.setString("sensors/DHT11/humidity", fireHumid);                                   //setup path and send readings
    Firebase.setString("/DHT11/temperature", fireTemp);                                 //setup path and send readings
    if (Firebase.failed()) {
      Serial.print("update /logs failed:");
      Serial.println(Firebase.error());
      return;
    }
  }

  if (millis() - preMillis2 >= interLamp) {
    preMillis2 = millis();
    int lamp = Firebase.getInt("/control/status_lamp");
    if (Firebase.failed()) {
      Serial.print("update /logs failed:");
      Serial.println(Firebase.error());
      return;
    }

    if (lamp == 0) {
      lampState = LOW;
    } else {
      lampState = HIGH;
    }

    digitalWrite(pinLamp, lampState);
  }

  if (millis() - preMillis3 >= interFan) {
    preMillis3 = millis();
    int fan = Firebase.getInt("/control/status_fan");
    if (Firebase.failed()) {
      Serial.print("update /logs failed:");
      Serial.println(Firebase.error());
      return;
    }

    if (fan == 0) {
      fanState = LOW;
    } else {
      fanState = HIGH;
    }

    digitalWrite(pinFan, fanState);
  }

  if (millis() - preMillis4 >= interHX711) {
    preMillis4 = millis();
    String avWeOne = String(scale.get_units(10), 1) + String(" gr");

    Firebase.setString("sensors//HX711/beratSatu", avWeOne);                                   //setup path and send readings
    if (Firebase.failed()) {
      Serial.print("update /logs failed:");
      Serial.println(Firebase.error());
      return;
    }
  }

  if (millis() - preMillisRT >= interHX711RT) {
    preMillisRT = millis();
    String w = String(scale.get_units(10), 1);
    String avWO = w + String(" gr");

    Serial.print("\t| average:\t");
    Serial.println(avWO);

    if (w.toInt() < 1050) {
      if (w.toInt() < 1050 && w.toInt() > 701) {
        digitalWrite(indFull, HIGH);
        digitalWrite(indMed, HIGH);
        digitalWrite(indLow, HIGH);
      } else {
        digitalWrite(indFull, LOW);
        digitalWrite(indMed, LOW);
        digitalWrite(indLow, HIGH);
      }
      //      if (weight.toInt() < 700 && weight.toInt() > 351) {
      //        digitalWrite(indFull, LOW);
      //        digitalWrite(indMed, HIGH);
      //        digitalWrite(indLow, HIGH);
      //      }
      //      if (weight.toInt() < 350 && weight.toInt() > 0) {
      //        digitalWrite(indFull, LOW);
      //        digitalWrite(indMed, LOW);
      //        digitalWrite(indLow, HIGH);
      //      }
    } else {
      digitalWrite(indFull, LOW);
      digitalWrite(indMed, LOW);
      digitalWrite(indLow, LOW);
    }
  }
}
