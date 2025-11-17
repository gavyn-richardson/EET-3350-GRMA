#include "DHTesp.h"

int sensorValue;  //variable to store sensor value
int qualityThreshold;
int tempThreshold;
int humidityThreshold;

const int MQ2pin = 28; 
const int DHTpin = 20;

const int AlarmPin = 22;
const int HumidPin = 26;
const int FanPin = 27;

DHTesp dhtSensor;

void setup() {
  Serial1.begin(115200);
  
  dhtSensor.setup(DHTpin, DHTesp::DHT22);
  Serial1.println("MQ2 warming up");

  pinMode(AlarmPin, OUTPUT);
  pinMode(HumidPin, OUTPUT);
  pinMode(FanPin, OUTPUT);
	
  qualityThreshold = 940;
  tempThreshold = 23;
  humidityThreshold = 50;

	delay(200); // allow the MQ2 to warm up
}

void loop() {
  TempAndHumidity  data = dhtSensor.getTempAndHumidity();
	sensorValue = analogRead(MQ2pin); 
  Serial1.println("Temp: " + String(data.temperature, 2) + "°C");
  Serial1.println("Humidity: " + String(data.humidity, 1) + "%");
  Serial1.println("Quality: " + String(sensorValue));
  Serial1.println("---");

	if (sensorValue >= qualityThreshold) {
		digitalWrite(AlarmPin, HIGH);
	} else {
		digitalWrite(AlarmPin, LOW);
	}

if (data.temperature >= tempThreshold) {
		digitalWrite(FanPin, HIGH);
	} else {
		digitalWrite(FanPin, LOW);
	}

  if (data.humidity >= humidityThreshold) {
		digitalWrite(HumidPin, HIGH);
	} else {
		digitalWrite(HumidPin, LOW);
	}

  delay(2000); // Wait for a new reading from the sensor (DHT22 has ~0.5Hz sample rate)
}