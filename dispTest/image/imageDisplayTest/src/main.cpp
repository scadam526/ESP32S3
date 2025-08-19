#include <Arduino.h>
#include <Adafruit_GFX.h>
#include <Adafruit_ST7789.h>
#include <SPIFFS_ImageReader.h>
#include <LittleFS.h>

#define D2 2 // Define pin D2 for button input
#define FSYS LittleFS

Adafruit_ST7789 display = Adafruit_ST7789(TFT_CS, TFT_DC, TFT_RST);
SPIFFS_ImageReader reader;
SPIFFS_Image img;

void setup() {
  delay(2000);
  Serial.begin(115200);
  delay(4000);
  pinMode(D2, INPUT_PULLDOWN); // Set D2 as input with pull-down resistor
  Serial.println("Serial started");

  if (!FSYS.begin(true)) {
    Serial.println("LittleFS mount failed!");
    return;
  }
  Serial.println("LittleFS mounted OK!");

  // list contents
  File root = FSYS.open("/");
  File file = root.openNextFile();
  while (file) {
    Serial.printf("File: %s (%d bytes)\n", file.name(), file.size());
    file = root.openNextFile();
  }

  // define button pulls
  pinMode(0, INPUT_PULLUP);
  pinMode(1, INPUT_PULLDOWN);
  pinMode(2, INPUT_PULLDOWN);

  display.init(135, 240); // Initialize display with width and height
  display.setRotation(3); // Set display rotation
  display.fillScreen(ST77XX_BLACK); // Clear the screen
  Serial.println("Display initialized");
  pinMode(TFT_BACKLITE, OUTPUT);
  digitalWrite(TFT_BACKLITE, HIGH);
  Serial.println("Backlight enabled");

  Serial.println("Opening BMP from LittleFS...");
  File bmpFile = FSYS.open("/nuggets.bmp", "r");
  if (!bmpFile) {
    Serial.println("Failed to open BMP file!");
    while (1);
  }

  // ImageReturnCode stat = reader.readBMP(bmpFile, img);
  ImageReturnCode stat = reader.drawBMP("/nuggets.bmp", display, 0, 0, true);
  bmpFile.close();

  if (stat != IMAGE_SUCCESS) {
    Serial.print("Image read failed: ");
    Serial.println((int)stat);
    while (1);
  }

  img.draw(display, 0, 0); // Draw at x=0, y=0
  Serial.println("Image displayed!");
}

int i = 0;

void loop() {
 
  //if button D2 is pressed perform a soft reset
  // Serial.println(digitalRead(D2));
  if (digitalRead(D2) == HIGH) {
    Serial.println(i);
    i++;
    return;
  }
  // delay(100);
  // return;
}
