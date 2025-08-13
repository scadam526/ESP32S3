#include <Adafruit_GFX.h>
#include <Adafruit_ST7789.h>
#include <SPI.h>

// --- Pin configuration for Reverse TFT Feather ---
#define TFT_CS    42
#define TFT_DC    40
#define TFT_RST   41
#define TFT_SCLK  36
#define TFT_MOSI  35

// --- Create display instance ---
Adafruit_ST7789 tft = Adafruit_ST7789(TFT_CS, TFT_DC, TFT_RST);

void setup() {
  Serial.begin(115200);
  delay(500);  // Wait for USB serial to connect

  // Initialize SPI manually with correct pins
  SPI.begin(TFT_SCLK, -1, TFT_MOSI, -1);

  // Initialize display
  tft.init(135, 240);  // ST7789 on Reverse TFT is 135x240
  tft.setRotation(3);  // Landscape, USB on the left
  tft.fillScreen(ST77XX_BLACK);

  tft.setCursor(10, 30);
  tft.setTextColor(ST77XX_GREEN);
  tft.setTextSize(2);
  tft.println("Hello from");
  tft.println("Adafruit Feather!");
  tft.setTextSize(1);
  tft.println("ESP32-S3 Reverse TFT");

  delay(2000);
}

void loop() {
  // Animate a test rectangle
  static uint16_t color = ST77XX_RED;
  tft.fillRect(50, 100, 60, 30, color);
  color = (color == ST77XX_RED) ? ST77XX_BLUE : ST77XX_RED;
  delay(500);
  printf("alive");
}
