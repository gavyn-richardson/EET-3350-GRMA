#include "DHTesp.h"
#include "Adafruit_GFX.h"
#include "Adafruit_ILI9341.h"

// Init variables
int sensorValue;
int qualityThreshold = 940;
int tempThreshold    = 23;
int humidityThreshold = 50;

DHTesp dhtSensor;

// Sensor Pins
const int MQ2pin = 28; 
const int DHTpin = 20;

// TFT Display Pins
#define TFT_DC   13
#define TFT_RST  14
#define TFT_CS   15
#define TFT_MOSI 12
#define TFT_SCK  11
#define TFT_MISO 5
Adafruit_ILI9341 tft = Adafruit_ILI9341(TFT_CS, TFT_DC, TFT_MOSI, TFT_SCK, TFT_RST, TFT_MISO);

// Output Pins (LEDs)
const int AlarmPin = 22;
const int HumidPin = 26;
const int FanPin   = 27;

void setup() {
  Serial1.begin(115200);

  // DHT22 setup
  dhtSensor.setup(DHTpin, DHTesp::DHT22);

  // TFT setup
  tft.begin();
  tft.setRotation(0);
  tft.fillScreen(ILI9341_BLACK);
  tft.setTextSize(2);
  tft.setTextColor(ILI9341_CYAN);
  tft.setCursor(5, 20);
  tft.println("Environment Monitor");

  // LED pins
  pinMode(AlarmPin, OUTPUT);
  pinMode(HumidPin, OUTPUT);
  pinMode(FanPin, OUTPUT);

  delay(200); // MQ2 warmup
}

void loop() {
  TempAndHumidity data = dhtSensor.getTempAndHumidity();
  sensorValue = analogRead(MQ2pin);

  // ===== Serial Monitor Output =====
  Serial1.println("Temp: " + String(data.temperature, 2) + "C");
  Serial1.println("Humidity: " + String(data.humidity, 1) + "%");
  Serial1.println("Quality: " + String(sensorValue));
  Serial1.println("-------------------");

  // ======== Display Output ========
  tft.fillRect(0, 70, 240, 150, ILI9341_BLACK);  // clear value area

  tft.setTextSize(3);

  // Temperature
  if (data.temperature >= tempThreshold) {
    tft.setTextColor(ILI9341_RED);
  } else {
    tft.setTextColor(ILI9341_GREEN);
  }
  tft.setCursor(10, 80);
  tft.print("Temp: ");
  tft.print(data.temperature, 1);
  tft.println("C");

  // Humidity
  if (data.humidity >= humidityThreshold) {
    tft.setTextColor(ILI9341_RED);
  } else {
    tft.setTextColor(ILI9341_GREEN);
  }
  tft.setCursor(10, 120);
  tft.print("Hum:  ");
  tft.print(data.humidity, 0);
  tft.println("%");

  // Air Quality
  if (sensorValue >= qualityThreshold) {
    tft.setTextColor(ILI9341_RED);
  } else {
    tft.setTextColor(ILI9341_GREEN);
  }
  tft.setCursor(10, 160);
  tft.print("Gas:  ");
  tft.print(sensorValue);

  // ======== LED Outputs ========
  digitalWrite(AlarmPin, sensorValue >= qualityThreshold);
  digitalWrite(FanPin,   data.temperature >= tempThreshold);
  digitalWrite(HumidPin, data.humidity >= humidityThreshold);

  delay(5000);
}
