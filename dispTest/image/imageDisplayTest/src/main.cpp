#include <Arduino.h>
#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_ST7789.h>
#include "FS.h"
#include "SPIFFS.h"

Adafruit_ST7789 tft(TFT_CS, TFT_DC, TFT_RST);

// --- Forward declarations for helpers used by drawBMP ---
static uint16_t read16(File &f);
static uint32_t read32(File &f);


// Reads a 24-bit uncompressed BMP from SPIFFS and displays it using drawRGBBitmap()
// Only suitable for small images that fit in RAM
void drawBMP(const char *filename, int16_t x, int16_t y) {
  File bmpFile = SPIFFS.open(filename, "r");
  if (!bmpFile) {
    // Serial.printf("File not found: %s\n", filename);
    return;
  }

  // BMP Header
  if (read16(bmpFile) != 0x4D42) {
    // Serial.println("Not a BMP");
    bmpFile.close();
    return;
  }
  (void)read32(bmpFile); // file size
  (void)read32(bmpFile); // reserved
  uint32_t bmpImageoffset = read32(bmpFile); // start of pixel data
  uint32_t headerSize = read32(bmpFile); // DIB header size
  int32_t bmpWidth  = (int32_t)read32(bmpFile);
  int32_t bmpHeight = (int32_t)read32(bmpFile);
  if (read16(bmpFile) != 1) { // planes
    // Serial.println("Invalid BMP planes");
    bmpFile.close();
    return;
  }
  uint16_t bmpDepth = read16(bmpFile); // bits per pixel
  uint32_t compression = read32(bmpFile);

  if (bmpDepth != 24 || compression != 0) {
    // Serial.println("Unsupported BMP: need 24-bit uncompressed");
    bmpFile.close();
    return;
  }

  uint32_t rowSize = (bmpWidth * 3 + 3) & ~3; // row padded to 4 bytes
  bool flip = true;
  if (bmpHeight < 0) {
    bmpHeight = -bmpHeight;
    flip = false;
  }

  // Allocate buffer for the whole image (RGB565)
  uint16_t *bitmap = (uint16_t*)malloc(bmpWidth * bmpHeight * sizeof(uint16_t));
  if (!bitmap) {
    // Serial.println("Out of memory for bitmap");
    bmpFile.close();
    return;
  }

  // Read pixel data
  for (int row = 0; row < bmpHeight; row++) {
    int bmpRow = flip ? (bmpHeight - 1 - row) : row;
    bmpFile.seek(bmpImageoffset + bmpRow * rowSize);
    for (int col = 0; col < bmpWidth; col++) {
      uint8_t b = bmpFile.read();
      uint8_t g = bmpFile.read();
      uint8_t r = bmpFile.read();
      bitmap[row * bmpWidth + col] = tft.color565(r, g, b);
    }
  }
  bmpFile.close();

  // Draw the image
  tft.drawRGBBitmap(x, y, bitmap, bmpWidth, bmpHeight);
  free(bitmap);
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

void listSPIFFSFiles() {
  File root = SPIFFS.open("/");
  if (!root) {
    // Serial.println("Failed to open root directory");
    return;
  }
  if (!root.isDirectory()) {
    // Serial.println("Root is not a directory");
    return;
  }

  File file = root.openNextFile();
  while (file) {
    // Serial.print("FILE: ");
    // Serial.print(file.name());
    // Serial.print("\tSIZE: ");
    // Serial.println(file.size());
    file = root.openNextFile();
  }
}

void setup() {
  // Serial.begin(115200);
  // delay(3000);

  // Mount SPIFFS partition labeled "storage" at mount point "/storage"
  // begin(formatOnFail, basePath, maxOpenFiles, partitionLabel)
  if (!SPIFFS.begin(false, "/storage", 10, "storage")) {
    // Serial.println("SPIFFS mount failed (partition 'storage').");
    for(;;) delay(10);
  }
  listSPIFFSFiles();

  // Init display
  tft.init(135, 240);
  tft.setRotation(3);
  pinMode(TFT_BACKLITE, OUTPUT);
  digitalWrite(TFT_BACKLITE, HIGH);   // backlight on
  tft.fillScreen(ST77XX_BLACK);

  // Draw your BMP (must be 24-bit, uncompressed)
  drawBMP("/nuggets.bmp", 0, 0);
}

void loop() {}
