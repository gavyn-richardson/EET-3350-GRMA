#include "DHTesp.h"
#include "Adafruit_GFX.h"
#include "Adafruit_ILI9341.h"
#include <XPT2046_Touchscreen.h>

// ===== TOUCHSCREEN SETUP =====
#define TOUCH_CS  15   // Change if needed

XPT2046_Touchscreen ts(TOUCH_CS);

// ===== TFT Pins =====
#define TFT_DC   13
#define TFT_RST  14
#define TFT_CS   15
#define TFT_MOSI 12
#define TFT_SCK  11
#define TFT_MISO 5

Adafruit_ILI9341 tft = Adafruit_ILI9341(TFT_CS, TFT_DC, TFT_MOSI, TFT_SCK, TFT_RST, TFT_MISO);

// ===== THRESHOLDS =====
int tempThreshold = 23;
int humidityThreshold = 50;
int qualityThreshold = 940;

// ===== SENSOR =====
DHTesp dhtSensor;
const int MQ2pin = 28;
const int DHTpin = 20;

// ===== OUTPUTS =====
const int AlarmPin = 22;
const int HumidPin = 26;
const int FanPin   = 27;

// ===== READINGS =====
int sensorValue;

// ===== BUTTON STRUCT =====
struct Button { int x, y, w, h; };
const int COL_W = 80;

// TEMP buttons
Button T_up   = {0,   200, COL_W, 40};
Button T_down = {0,   280, COL_W, 40};

// HUM buttons
Button H_up   = {80,  200, COL_W, 40};
Button H_down = {80,  280, COL_W, 40};

// GAS buttons
Button G_up   = {160, 200, COL_W, 40};
Button G_down = {160, 280, COL_W, 40};


// ===== DRAWING =====
void drawButton(Button b, const char* label) {
  tft.fillRect(b.x, b.y, b.w, b.h, ILI9341_BLUE);
  tft.drawRect(b.x, b.y, b.w, b.h, ILI9341_WHITE);
  tft.setTextColor(ILI9341_WHITE);
  tft.setTextSize(2);

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

  // Center text manually
  tft.setCursor(20, 250);
  tft.print(tempThreshold);

  tft.setCursor(100, 250);
  tft.print(humidityThreshold);

  tft.setCursor(170, 250);
  tft.print(qualityThreshold);
}

bool touched(Button b, int x, int y) {
  return (x > b.x && x < b.x + b.w && y > b.y && y < b.y + b.h);
}

void checkTouch() {
  if (!ts.touched()) return;

  TS_Point p = ts.getPoint();

  int x = map(p.x, 200, 3900, 0, 240);
  int y = map(p.y, 200, 3900, 0, 320);

  // TEMP
  if (touched(T_up,   x, y)) tempThreshold++;
  if (touched(T_down, x, y)) tempThreshold--;

  // HUMIDITY
  if (touched(H_up,   x, y)) humidityThreshold++;
  if (touched(H_down, x, y)) humidityThreshold--;

  // GAS
  if (touched(G_up,   x, y)) qualityThreshold += 5;
  if (touched(G_down, x, y)) qualityThreshold -= 5;

  drawThresholdValues();
  delay(220);
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


// ====== SETUP ======
void setup() {
  Serial.begin(115200);

  ts.begin();
  ts.setRotation(1);

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


// ====== MAIN LOOP ======
void loop() {
  checkTouch();  // Handle threshold button input

  // ===== Read Sensors =====
  TempAndHumidity data = dhtSensor.getTempAndHumidity();
  sensorValue = analogRead(MQ2pin);

  // ======= Display Current Readings (Top Section) =======
  tft.fillRect(100, 50, 240, 150, ILI9341_BLACK); // clear old readings
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

  // ======= LED Outputs =======
  digitalWrite(AlarmPin, sensorValue >= qualityThreshold);
  digitalWrite(FanPin,   data.temperature >= tempThreshold);
  digitalWrite(HumidPin, data.humidity >= humidityThreshold);

  delay(400);
}
