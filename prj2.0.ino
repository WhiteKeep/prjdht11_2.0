#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <DHT_U.h>
#include <SPI.h>
#include <SD.h>


#define DHTPIN A3
#define DHTTYPE    DHT11
#define but 2
#define led A1


DHT_Unified dht(DHTPIN, DHTTYPE);

uint32_t delayMS;
const int chipSelect = 4;


float data;
int writeDelay = 1000;
boolean writeState = 0;
unsigned long last01;
unsigned long last02;


void setup() {
  Serial.begin(9600);


  pinMode(led, OUTPUT);
  pinMode(but, INPUT_PULLUP);

  digitalWrite(led, LOW);

  dht.begin();
  sensor_t sensor;
  dht.temperature().getSensor(&sensor);
  dht.humidity().getSensor(&sensor);
  delayMS = sensor.min_delay / 1000;

  SD.begin(chipSelect);
}

void loop() {

  delay(delayMS);

  sensors_event_t event;
  dht.temperature().getEvent(&event);
  if (isnan(event.temperature)) {
    Serial.println(F("Error reading temperature!"));
  }
  else {
    Serial.print(F("Temperature: "));
    Serial.print(event.temperature);
    Serial.println(F("°C"));
  }
  dht.humidity().getEvent(&event);
  if (isnan(event.relative_humidity)) {
    Serial.println(F("Error reading humidity!"));
  }
  else {
    Serial.print(F("Humidity: "));
    Serial.print(event.relative_humidity);
    Serial.println(F("%"));
    if (writeState) {
      data = event.relative_humidity;
      sdwriter();
    }
  }


  if (writeState) {
    digitalWrite(led, HIGH);
  } else {
    digitalWrite(led, LOW);
  }

  if (!digitalRead(but) == HIGH && millis() - last02 > 300) {
    writeState = !writeState;
    last02 = millis();
    Serial.println(F("press"));
  }

  Serial.println(writeState);

}

void sdwriter() {

  String datastr = "";
  datastr += String(millis());
  datastr += ' ';
  datastr += String(data);

  SD.begin(chipSelect);


  File dataFile = SD.open("log.txt", FILE_WRITE);

  if (dataFile) {
    dataFile.println(datastr);
    dataFile.close();
    Serial.println(datastr);
  }
  else {
    Serial.println("error opening log.txt");
  }

}
