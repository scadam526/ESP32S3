#include <Arduino.h>

#define D2 2 // Define pin D2 for button input

void setup() {
  delay(0);
  Serial.begin(115200);
  delay(4000);
  pinMode(D2, INPUT_PULLDOWN); // Set D2 as input with pull-down resistor
  Serial.println("Starting...");
}

void loop() {
 
  //if button D2 is pressed perform a soft reset
  Serial.println(digitalRead(D2));
  delay(100);
  // return;
}
