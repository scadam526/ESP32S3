#include <Arduino.h>
#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_ST7789.h>
#include "FS.h"
#include "SPIFFS.h"

// --- Board pins (Adafruit ESP32-S3 Reverse TFT Feather) ---
// #define TFT_CS   -1   // CS not used (wired)
// #define TFT_DC    7   // Data/Command
// #define TFT_RST  -1   // RST handled on-board
// #define TFT_BL   45   // Backlight

Adafruit_ST7789 tft(TFT_CS, TFT_DC, TFT_RST);

// --- Forward declarations for helpers used by drawBMP ---
static uint16_t read16(File &f);
static uint32_t read32(File &f);

void drawBMP(const char *filename, int16_t x, int16_t y) {
  File bmpFile = SPIFFS.open(filename, "r");
  if (!bmpFile) {
    Serial.printf("File not found: %s\n", filename);
    return;
  }

  if (read16(bmpFile) != 0x4D42) { // 'BM'
    Serial.println("Not a BMP");
    bmpFile.close();
    return;
  }

  (void)read32(bmpFile);                // file size
  (void)read32(bmpFile);                // reserved
  uint32_t bmpImageoffset = read32(bmpFile); // start of pixel data
  (void)read32(bmpFile);                // DIB header size
  int32_t bmpWidth  = (int32_t)read32(bmpFile);
  int32_t bmpHeight = (int32_t)read32(bmpFile);
  if (read16(bmpFile) != 1) {           // planes
    Serial.println("Invalid BMP planes");
    bmpFile.close();
    return;
  }
  uint16_t bmpDepth = read16(bmpFile);  // bits per pixel
  uint32_t compression = read32(bmpFile);

  if (bmpDepth != 24 || compression != 0) {
    Serial.println("Unsupported BMP: need 24-bit uncompressed");
    bmpFile.close();
    return;
  }

  uint32_t rowSize = (bmpWidth * 3 + 3) & ~3; // row padded to 4 bytes

  bool flip = true;
  if (bmpHeight < 0) {
    bmpHeight = -bmpHeight;
    flip = false;
  }

  // --- Compute scale factor to fit display ---
  float xScale = (float)tft.width()  / bmpWidth;
  float yScale = (float)tft.height() / bmpHeight;
  float scale = (xScale < yScale) ? xScale : yScale; // preserve aspect ratio

  int scaledW = bmpWidth * scale;
  int scaledH = bmpHeight * scale;

  int xOffset = (tft.width()  - scaledW) / 2;
  int yOffset = (tft.height() - scaledH) / 2;

  Serial.printf("BMP %dx%d scaled to %dx%d\n", bmpWidth, bmpHeight, scaledW, scaledH);

  // Buffer for one row of target screen width
  uint16_t *rowbuf = (uint16_t*)malloc(scaledW * sizeof(uint16_t));
  if (!rowbuf) {
    Serial.println("Out of memory for row buffer");
    bmpFile.close();
    return;
  }

  // --- Render each scaled row ---
  for (int row = 0; row < scaledH; row++) {
    // Which row in source BMP does this map to?
    int srcRow = flip ? (bmpHeight - 1 - (int)(row / scale))
                      : (int)(row / scale);

    uint32_t pos = bmpImageoffset + srcRow * rowSize;
    bmpFile.seek(pos);

    // Read one source row (but we’ll skip cols as we scale)
    for (int col = 0; col < scaledW; col++) {
      int srcCol = (int)(col / scale);
      bmpFile.seek(pos + srcCol * 3);

      uint8_t b = bmpFile.read();
      uint8_t g = bmpFile.read();
      uint8_t r = bmpFile.read();

      rowbuf[col] = tft.color565(r, g, b);
    }

    // Draw the scaled row
    tft.startWrite();
    tft.setAddrWindow(xOffset, yOffset + row, scaledW, 1);
    tft.writePixels(rowbuf, scaledW, true);
    tft.endWrite();
  }

  free(rowbuf);
  bmpFile.close();
}


// --- Helpers ---
static uint16_t read16(File &f) {
  uint16_t r;
  ((uint8_t*)&r)[0] = f.read(); // LSB
  ((uint8_t*)&r)[1] = f.read(); // MSB
  return r;
}

static uint32_t read32(File &f) {
  uint32_t r;
  ((uint8_t*)&r)[0] = f.read(); // LSB
  ((uint8_t*)&r)[1] = f.read();
  ((uint8_t*)&r)[2] = f.read();
  ((uint8_t*)&r)[3] = f.read(); // MSB
  return r;
}

void setup() {
  Serial.begin(115200);
  delay(300);

  // Mount SPIFFS partition labeled "storage" at mount point "/storage"
  // begin(formatOnFail, basePath, maxOpenFiles, partitionLabel)
  if (!SPIFFS.begin(false, "/storage", 10, "storage")) {
    Serial.println("SPIFFS mount failed (partition 'storage').");
    for(;;) delay(10);
  }

  // Init display
  tft.init(240, 135);
  tft.setRotation(3);
  pinMode(TFT_BACKLITE, OUTPUT);
  digitalWrite(TFT_BACKLITE, HIGH);   // backlight on
  tft.fillScreen(ST77XX_BLACK);

  // Draw your BMP (must be 24-bit, uncompressed)
  drawBMP("/test.bmp", 0, 0);
}

void loop() {}
