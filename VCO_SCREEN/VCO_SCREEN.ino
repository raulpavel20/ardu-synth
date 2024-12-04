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

enum scrollActions { OSC_TYPE,
                     OSC_INDEX };
enum oscTypes { SIN,
                SQR,
                TRI };

typedef struct {
  double x;
  double y;
} Point;
typedef struct {
  Point a;
  Point b;
  Point c;
} Triangle;

Triangle triangle1 = {
  .a = { 33.00, 20.00 },
  .b = { 43.39, 38.00 },
  .c = { 22.61, 38.00 }
};

Triangle triangle2 = {
  .a = { 95.00, 20.00 },
  .b = { 105.39, 38.00 },
  .c = { 84.61, 38.00 }
};
enum scrollActions crtAction = OSC_INDEX;
enum oscTypes osc0Type = TRI;
enum oscTypes osc1Type = TRI;
int selectedOsc = 0;

enum oscTypes incrementOsc(enum oscTypes currentOsc) {
  if (currentOsc == TRI) {
    return SIN;
  }
  return static_cast<oscTypes>(static_cast<int>(currentOsc) + 1);
}

enum oscTypes decrementOsc(enum oscTypes currentOsc) {
  if (currentOsc == SIN) {
    return TRI;
  }
  return static_cast<oscTypes>(static_cast<int>(currentOsc) - 1);
}

void setup() {
  Serial.begin(9600);
  pinMode(BUTTON_PIN, INPUT_PULLUP);

  if (!display.begin(SSD1306_SWITCHCAPVCC, OLED_ADDR)) {
    Serial.println("SSD1306 allocation failed");
    for (;;)
      ;
  }
  delay(100);  // let the display initialize
  Serial.println("SSD1306 initialized");

  refreshScreen();
}

void loop() {
  if (digitalRead(BUTTON_PIN) == LOW) {
    if (crtAction == OSC_TYPE) {
      crtAction = OSC_INDEX;
    } else {
      crtAction = OSC_TYPE;
    }
    refreshScreen();
    delay(300);
  }

  long newEncPosition = mainEnc.readAndReset();
  if (newEncPosition == 0) {
    return;
  }

  if (crtAction == OSC_INDEX) {
    if (newEncPosition == 1) {
      if (selectedOsc == 0) {
        selectedOsc = 1;
        refreshScreen();
      }
    } else if (newEncPosition == -1) {
      selectedOsc = 0;
      refreshScreen();
    }
  } else {
    if (selectedOsc == 0) {
      osc0Type = newEncPosition == 1 ? incrementOsc(osc0Type) : newEncPosition == -1 ? decrementOsc(osc0Type)
                                                                                     : osc0Type;
    } else {
      osc1Type = newEncPosition == 1 ? incrementOsc(osc1Type) : newEncPosition == -1 ? decrementOsc(osc1Type)
                                                                                     : osc1Type;
    }
    refreshScreen();
    delay(50);
  }
}

const int rectWidth = (SCREEN_WIDTH - 3 * PADDING) / 2;
const int rectHeight = SCREEN_HEIGHT - 2 * PADDING;
const int symbolWidth = 12;
int rect1CenterX = PADDING + rectWidth / 2;
int rect2CenterX = PADDING * 2 + rectWidth + rectWidth / 2;
int rectCenterY = PADDING + rectHeight / 2;

void refreshScreen() {
  display.clearDisplay();

  if (selectedOsc == 0) {
    if (crtAction == OSC_INDEX) {
      display.fillRect(PADDING, PADDING, rectWidth, rectHeight, SSD1306_WHITE);
      display.fillRect(PADDING + 4, PADDING + 4, rectWidth - 8, rectHeight - 8, SSD1306_BLACK);
    } else {
      display.fillRect(PADDING, PADDING, rectWidth, rectHeight, SSD1306_WHITE);
    }
    display.drawRect(PADDING * 2 + rectWidth, PADDING, rectWidth, rectHeight, SSD1306_WHITE);
  } else {
    display.drawRect(PADDING, PADDING, rectWidth, rectHeight, SSD1306_WHITE);
    if (crtAction == OSC_INDEX) {
      display.fillRect(PADDING * 2 + rectWidth, PADDING, rectWidth, rectHeight, SSD1306_WHITE);
      display.fillRect(PADDING * 2 + rectWidth + 4, PADDING + 4, rectWidth - 8, rectHeight - 8, SSD1306_BLACK);
    } else {
      display.fillRect(PADDING * 2 + rectWidth, PADDING, rectWidth, rectHeight, SSD1306_WHITE);
    }
  }

  if (crtAction == OSC_INDEX) {
    drawOscTypeSymbol(osc0Type, rect1CenterX, true, 0);
    drawOscTypeSymbol(osc1Type, rect2CenterX, true, 1);
  } else {
    if (selectedOsc == 0) {
      drawOscTypeSymbol(osc0Type, rect1CenterX, false, 0);
      drawOscTypeSymbol(osc1Type, rect2CenterX, true, 1);
    } else {
      drawOscTypeSymbol(osc0Type, rect1CenterX, true, 0);
      drawOscTypeSymbol(osc1Type, rect2CenterX, false, 1);
    }
  }

  display.display();
}

void drawOscTypeSymbol(enum oscTypes type, int center, bool filled, int index) {
  int color = filled ? SSD1306_WHITE : SSD1306_BLACK;
  switch (type) {
    case SIN:
      display.fillCircle(center, rectCenterY, symbolWidth, color);
      break;
    case SQR:
      display.fillRect(center - symbolWidth, rectCenterY - symbolWidth, symbolWidth * 2, symbolWidth * 2, color);
      break;
    case TRI:
      if (index == 0) {
        display.fillTriangle(triangle1.a.x, triangle1.a.y, triangle1.b.x, triangle1.b.y, triangle1.c.x, triangle1.c.y, color);
      } else {
        display.fillTriangle(triangle2.a.x, triangle2.a.y, triangle2.b.x, triangle2.b.y, triangle2.c.x, triangle2.c.y, color);
      }
      break;
    default:
      break;
  }
}
