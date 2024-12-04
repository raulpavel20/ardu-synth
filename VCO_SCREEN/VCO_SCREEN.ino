#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <SPI.h>
#include <Wire.h>
#include <Encoder.h>
#include <math.h>

#define ENCODER_PIN_A 2
#define ENCODER_PIN_B 3
#define BUTTON_PIN 4

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_ADDR 0x3C
#define OLED_RESET -1

#define PADDING 4

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
Encoder mainEnc(ENCODER_PIN_A, ENCODER_PIN_B);

enum oscTypes { SIN,
                SQR,
                TRI,
                SAW };

enum oscTypes oscType = SIN;
int centerX = SCREEN_WIDTH / 2;
int centerY = SCREEN_HEIGHT / 2;

enum oscTypes incrementOsc(enum oscTypes currentOsc) {
  if (currentOsc == SAW) {
    return SIN;
  }
  return static_cast<oscTypes>(static_cast<int>(currentOsc) + 1);
}

enum oscTypes decrementOsc(enum oscTypes currentOsc) {
  if (currentOsc == SIN) {
    return SAW;
  }
  return static_cast<oscTypes>(static_cast<int>(currentOsc) - 1);
}

void setup() {
  Serial.begin(9600);

  pinMode(BUTTON_PIN, INPUT_PULLUP);

  if (!display.begin(SSD1306_SWITCHCAPVCC, OLED_ADDR)) {
    for (;;)
      ;
  }
  delay(100);  // let the display initialize

  refreshScreen();
}

void loop() {
  long newEncPosition = mainEnc.readAndReset();
  if (newEncPosition == 0) {
    return;
  }

  oscType = newEncPosition == 1 ? incrementOsc(oscType) : newEncPosition == -1 ? decrementOsc(oscType)
                                                                               : oscType;
  refreshScreen();
  Serial.println(oscType);
  delay(100);
}

void refreshScreen() {
  display.clearDisplay();
  drawOscTypeSymbol();
  drawVolumeBar();
  display.display();
}

void drawOscTypeSymbol() {
  int borderWidth = SCREEN_WIDTH - 32;
  int borderHeight = SCREEN_HEIGHT - 2 * PADDING;

  display.drawRect(PADDING, PADDING, borderWidth, borderHeight, SSD1306_WHITE);

  int centerX = PADDING + borderWidth / 2;
  int centerY = PADDING + borderHeight / 2;
  int symbolWidth = borderWidth - 20;
  int symbolHeight = borderHeight - 20;

  switch (oscType) {
    case SIN:
      for (int x = -symbolWidth / 2; x <= symbolWidth / 2; x++) {
        int y = (int)(sin(x * 0.1) * symbolHeight / 2);
        display.drawPixel(centerX + x, centerY - y, SSD1306_WHITE);
      }
      break;

    case SQR:
      display.drawLine(centerX - symbolWidth / 2, centerY - symbolHeight / 2, centerX, centerY - symbolHeight / 2, SSD1306_WHITE);          // Top line
      display.drawLine(centerX, centerY - symbolHeight / 2, centerX, centerY + symbolHeight / 2, SSD1306_WHITE);                            // Right edge
      display.drawLine(centerX, centerY + symbolHeight / 2, centerX + symbolWidth / 2, centerY + symbolHeight / 2, SSD1306_WHITE);          // Bottom line
      display.drawLine(centerX + symbolWidth / 2, centerY + symbolHeight / 2, centerX + symbolWidth / 2, centerY - symbolHeight / 2, SSD1306_WHITE);
      break;

    case TRI:
      display.drawLine(centerX - symbolWidth / 2, centerY + symbolHeight / 2, centerX, centerY - symbolHeight / 2, SSD1306_WHITE);  // Upward slope
      display.drawLine(centerX, centerY - symbolHeight / 2, centerX + symbolWidth / 2, centerY + symbolHeight / 2, SSD1306_WHITE);  // Downward slope
      break;

    case SAW:
      int step = symbolWidth / 2;
      for (int x = -symbolWidth / 2; x < symbolWidth / 2; x += step) {
        display.drawLine(centerX + x, centerY + symbolHeight / 2, centerX + x + step, centerY - symbolHeight / 2, SSD1306_WHITE);         // Upward slope
        display.drawLine(centerX + x + step, centerY - symbolHeight / 2, centerX + x + step, centerY + symbolHeight / 2, SSD1306_WHITE);  // Vertical drop
      }
      break;

    default:
      break;
  }
}

void drawVolumeBar() {
  int barX = SCREEN_WIDTH - 20;
  int barY = PADDING;
  int barWidth = 16;
  int barHeight = SCREEN_HEIGHT - 2 * PADDING;
  int volumeLevel = 50;

  display.drawRect(barX, barY, barWidth, barHeight, SSD1306_WHITE);

  int filledHeight = (volumeLevel * barHeight) / 100;
  display.fillRect(barX + 2, barY + barHeight - filledHeight, barWidth - 4, filledHeight, SSD1306_WHITE);
}
