#include <Arduino.h>
#include <Adafruit_GFX.h>
#include <Adafruit_ST7789.h>
#include <Fonts/FreeSans12pt7b.h>

Adafruit_ST7789 display = Adafruit_ST7789(TFT_CS, TFT_DC, TFT_RST);
GFXcanvas16 canvas(240, 135);

void setup() {
  delay(100);

  // initialize display
  display.init(135, 240);
  display.setRotation(3);
  canvas.setFont(&FreeSans12pt7b);
  canvas.setTextColor(ST77XX_WHITE);

  // define button pulls
  // pinMode(0, INPUT_PULLUP);
  // pinMode(1, INPUT_PULLDOWN);
  // pinMode(2, INPUT_PULLDOWN);

  pinMode(TFT_BACKLITE, OUTPUT);
  digitalWrite(TFT_BACKLITE, HIGH);
 
}


void loop() {

  // testing max refresh rate
  // get a current timestamp
  unsigned long start = millis();

  // do some drawing
  canvas.drawRect(10, 10, 100, 50, ST77XX_WHITE);
  canvas.fillScreen(ST77XX_BLACK);
  canvas.setCursor(15, 35);
  canvas.print("Hello, world! >>");

  // push the canvas to the display
  // display.drawRGBBitmap(0, 0, canvas.getBuffer(), 240, 135);
  // pinMode(TFT_BACKLITE, OUTPUT);
  // digitalWrite(TFT_BACKLITE, HIGH);

  // measure the time taken
  unsigned long duration = millis() - start;
  // print the time taken on the screen
  canvas.setCursor(15, 60);
  canvas.print("Frame duration: ");
  canvas.print(duration);
  canvas.println(" ms");
  display.drawRGBBitmap(0, 0, canvas.getBuffer(), 240, 135);
  // pinMode(TFT_BACKLITE, OUTPUT);
  // digitalWrite(TFT_BACKLITE, HIGH);
  return;
}
