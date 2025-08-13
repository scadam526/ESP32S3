#include <Arduino.h>
#include <Adafruit_GFX.h>
#include <Adafruit_ST7789.h>
#include <SPI.h>
#include <driver/i2s.h>
#include <arduinoFFT.h>
#include <Fonts/FreeSans12pt7b.h>

// ===== Display Pins (Reverse TFT Feather) =====
// #define TFT_CS    42
// #define TFT_DC    40
// #define TFT_RST   41
// #define TFT_SCLK  36
// #define TFT_MOSI  35

Adafruit_ST7789 tft = Adafruit_ST7789(TFT_CS, TFT_DC, TFT_RST);

// ===== I2S Mic Pins =====
#define I2S_WS    8    // LRCLK
#define I2S_SD    5    // Data in from mic
#define I2S_SCK   9    // BCLK

// ===== Button Pins =====
#define BTN_UP    14   // D1
#define BTN_DOWN  21   // D2

// ===== Audio & FFT Settings =====
#define SAMPLE_RATE   16000
#define SAMPLES       1024  // Must be a power of 2
#define THRESH_STEP   200   // Step size for threshold changes

int FFT_THRESHOLD = 1000;   // Initial detection threshold

double vReal[SAMPLES];
double vImag[SAMPLES];

ArduinoFFT<double> FFT = ArduinoFFT<double>(vReal, vImag, SAMPLES, SAMPLE_RATE);

// ===== Piano Note Frequencies =====
const char* noteNames[] = { "A4", "A#4", "B4", "C5", "C#5", "D5", "D#5", "E5", "F5", "F#5", "G5", "G#5" };
const double noteFreqs[] = { 440.00, 466.16, 493.88, 523.25, 554.37, 587.33, 622.25, 659.26, 698.46, 739.99, 783.99, 830.61 };
const int noteCount = sizeof(noteFreqs) / sizeof(noteFreqs[0]);

// ===== I2S Setup =====
void i2s_install() {
  i2s_config_t i2s_config = {
    .mode = (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_RX),
    .sample_rate = SAMPLE_RATE,
    .bits_per_sample = I2S_BITS_PER_SAMPLE_32BIT,
    .channel_format = I2S_CHANNEL_FMT_ONLY_LEFT,
    .communication_format = I2S_COMM_FORMAT_STAND_I2S,
    .intr_alloc_flags = ESP_INTR_FLAG_LEVEL1,
    .dma_buf_count = 8,
    .dma_buf_len = 64,
    .use_apll = false,
    .tx_desc_auto_clear = false,
    .fixed_mclk = 0
  };
  i2s_driver_install(I2S_NUM_0, &i2s_config, 0, NULL);
}

void i2s_setpin() {
  i2s_pin_config_t pin_config = {
    .bck_io_num = I2S_SCK,
    .ws_io_num = I2S_WS,
    .data_out_num = I2S_PIN_NO_CHANGE,
    .data_in_num = I2S_SD
  };
  i2s_set_pin(I2S_NUM_0, &pin_config);
}

void i2s_read_samples() {
  size_t bytes_read;
  int32_t buffer[SAMPLES];
  i2s_read(I2S_NUM_0, (char*)buffer, SAMPLES * sizeof(int32_t), &bytes_read, portMAX_DELAY);
  
  for (int i = 0; i < SAMPLES; i++) {
    vReal[i] = (double)buffer[i] / 2147483648.0; // Convert to float (-1.0 to 1.0)
    vImag[i] = 0.0;
  }
}

void draw_threshold_line() {
  int y = map(FFT_THRESHOLD, 0, 5000, tft.height() - 20, 20);
  tft.drawLine(0, y, tft.width(), y, ST77XX_RED);
}

void setup() {
  Serial.begin(115200);
  delay(100);

  // TFT Init
  // SPI.begin(TFT_SCLK, -1, TFT_MOSI, -1);
  tft.init(135, 240);
  tft.setRotation(3);
  tft.fillScreen(ST77XX_BLACK);

  // Buttons
  pinMode(BTN_UP, INPUT_PULLUP);
  pinMode(BTN_DOWN, INPUT_PULLUP);

  // I2S Init
  i2s_install();
  i2s_setpin();
  i2s_start(I2S_NUM_0);

  tft.setTextColor(ST77XX_WHITE);
  tft.setTextSize(1);
  tft.setCursor(10, 10);
  tft.println("FFT Note Visualizer");
}

void loop() {
  // Check button presses
  if (digitalRead(BTN_UP) == LOW) {
    FFT_THRESHOLD += THRESH_STEP;
    delay(150); // debounce
  }
  if (digitalRead(BTN_DOWN) == LOW) {
    FFT_THRESHOLD = max(0, FFT_THRESHOLD - THRESH_STEP);
    delay(150); // debounce
  }

  i2s_read_samples();

  FFT.windowing(FFT_WIN_TYP_HAMMING, FFT_FORWARD);
  FFT.compute(FFT_FORWARD);
  FFT.complexToMagnitude();

  tft.fillScreen(ST77XX_BLACK);
  tft.setCursor(0, 0);
  tft.setTextColor(ST77XX_WHITE);
  tft.print("Threshold: ");
  tft.println(FFT_THRESHOLD);

  // Draw bars + note detection
  String detectedNotes = "";
  int barWidth = tft.width() / noteCount;

  for (int i = 0; i < noteCount; i++) {
    int bin = (int)(noteFreqs[i] * SAMPLES / SAMPLE_RATE);
    if (bin < SAMPLES / 2) {
      double amplitude = vReal[bin] * 500; // scale for display
      if (amplitude > FFT_THRESHOLD) {
        detectedNotes += noteNames[i];
        detectedNotes += " ";
      }
      // Histogram bar
      int barHeight = min((int)amplitude, tft.height() - 20);
      tft.fillRect(i * barWidth, tft.height() - 20 - barHeight, barWidth - 2, barHeight, ST77XX_BLUE);
    }
  }

  draw_threshold_line();

  // Display detected notes in one bottom line
  tft.fillRect(0, tft.height() - 15, tft.width(), 15, ST77XX_BLACK);
  tft.setCursor(0, tft.height() - 12);
  tft.setTextColor(ST77XX_GREEN);
  tft.setTextSize(1);
  tft.print(detectedNotes);
  pinMode(TFT_BACKLITE, OUTPUT);
  digitalWrite(TFT_BACKLITE, HIGH);

  delay(50);
}
