#include "DHTesp.h"
#include "Adafruit_GFX.h"
#include "Adafruit_ILI9341.h"

// Touchscreen pin setup and init based on other wokwi example
#define TFT_DC   13
#define TFT_RST  14
#define TFT_CS   15
#define TFT_MOSI 12
#define TFT_SCK  11
#define TFT_MISO 5

Adafruit_ILI9341 tft = Adafruit_ILI9341(TFT_CS, TFT_DC, TFT_MOSI, TFT_SCK, TFT_RST, TFT_MISO);

// Sensor and output pin setup
DHTesp dhtSensor;
const int MQ2pin = 28;
const int DHTpin = 20;

const int AlarmPin = 22;
const int HumidPin = 26;
const int FanPin   = 27;

// Threshold and snensor variable setup
int tempThreshold = 23;
int humidityThreshold = 50;
int qualityThreshold = 940;

int sensorValue;

// Button class
struct Button { int x, y, w, h; };
const int COL_W = 80; // Set column width

// Temperature buttons
Button T_up   = {0,   200, COL_W, 40}; // Set button dimensions and location
Button T_down = {0,   280, COL_W, 40};

// Humidity buttons
Button H_up   = {80,  200, COL_W, 40};
Button H_down = {80,  280, COL_W, 40};

// Gas buttons
Button G_up   = {160, 200, COL_W, 40};
Button G_down = {160, 280, COL_W, 40};


// Draw functions
void drawButton(Button b, const char* label) {
  tft.fillRect(b.x, b.y, b.w, b.h, ILI9341_BLUE); // Fills button at button location
  tft.drawRect(b.x, b.y, b.w, b.h, ILI9341_WHITE);  // Draw border around button
  tft.setTextColor(ILI9341_WHITE); 
  tft.setTextSize(2);

  // Setup cursor for + or - 
  int textX = b.x + (b.w / 2) - 5;
  int textY = b.y + 12;
  tft.setCursor(textX, textY);
  tft.print(label);
}


void drawThresholdValues() {
  // Clear value areas
  tft.fillRect(0,   240, COL_W, 40, ILI9341_BLACK);
  tft.fillRect(80,  240, COL_W, 40, ILI9341_BLACK);
  tft.fillRect(160, 240, COL_W, 40, ILI9341_BLACK);

  tft.setTextColor(ILI9341_YELLOW);
  tft.setTextSize(3);

  // Center text 
  tft.setCursor(20, 250);
  tft.print(tempThreshold);

  tft.setCursor(100, 250);
  tft.print(humidityThreshold);

  tft.setCursor(170, 250);
  tft.print(qualityThreshold);
}

void drawAllButtons() {
  drawButton(T_up, "+");
  drawButton(T_down, "-");

  drawButton(H_up, "+");
  drawButton(H_down, "-");

  drawButton(G_up, "+");
  drawButton(G_down, "-");

  drawThresholdValues();
}

// Setup
void setup() {
  Serial.begin(115200);

  dhtSensor.setup(DHTpin, DHTesp::DHT22);

  tft.begin();
  tft.setRotation(0);
  tft.fillScreen(ILI9341_BLACK);

  tft.setTextSize(2);
  tft.setTextColor(ILI9341_CYAN);
  tft.setCursor(5, 20);
  tft.println("Environment Monitor");

  tft.setTextSize(3);
  tft.setTextColor(ILI9341_YELLOW);
  tft.setCursor(10, 60);
  tft.print("Temp: ");
  tft.setCursor(10, 100);
  tft.print("Hum: ");
  tft.setCursor(10, 140);
  tft.print("Gas: ");

  pinMode(AlarmPin, OUTPUT);
  pinMode(HumidPin, OUTPUT);
  pinMode(FanPin,   OUTPUT);

  drawAllButtons();
}


// Main Loop
void loop() {
  // Read sensors
  TempAndHumidity data = dhtSensor.getTempAndHumidity();
  sensorValue = analogRead(MQ2pin);

  // Display Sesnor Readings
  tft.fillRect(100, 50, 240, 150, ILI9341_BLACK); // Clear old readings
  tft.setTextSize(3);

  // Temperature
  if (data.temperature >= tempThreshold) tft.setTextColor(ILI9341_RED);
  else tft.setTextColor(ILI9341_GREEN);

  tft.setCursor(100, 60);
  tft.print(data.temperature, 1);
  tft.println("C");

  // Humidity
  if (data.humidity >= humidityThreshold) tft.setTextColor(ILI9341_RED);
  else tft.setTextColor(ILI9341_GREEN);

  tft.setCursor(100, 100);
  tft.print(data.humidity, 0);
  tft.println("%");

  // Air Quality
  if (sensorValue >= qualityThreshold) tft.setTextColor(ILI9341_RED);
  else tft.setTextColor(ILI9341_GREEN);

  tft.setCursor(100, 140);
  tft.print(sensorValue);

  // LED Outputs
  digitalWrite(AlarmPin, sensorValue >= qualityThreshold);
  digitalWrite(FanPin,   data.temperature >= tempThreshold);
  digitalWrite(HumidPin, data.humidity >= humidityThreshold);

  delay(400);
}
