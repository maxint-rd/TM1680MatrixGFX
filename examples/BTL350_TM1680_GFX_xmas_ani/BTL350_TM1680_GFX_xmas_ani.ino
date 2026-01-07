/* 
 * Holiday animation for BTL-350
 *-+
 * Arduino example sketch to demonstrate the Adafruit GFX library on 
 * the TM1680 driven LED Matrix of the BTL-350 bluetooth speaker by Denver.
 * 
 * Made using Gemini AI: gemini.google.com/share/d011efddf326
 * (only minor modifications afterwards)
 * 
 * Confirmed to work in the following environments:
 *     - CH32V006, CH32 Arduino core 1.0.4 with updates (main branche, PR#200 and others)
 *       21912 bytes (34%) flash, 1092 bytes (13%) RAM when compiled with debug info and symbols.
 * 
 * For more information see TM1680MatrixGFX library (https://github.com/maxint-rd/TM1680MatrixGFX
 * For BTL-350 matrix connections see earlier research: https://github.com/maxint-rd/TM16xx/issues/2
 */

#include <Adafruit_GFX.h>
#include "TM1680MatrixGFX.h"   // TODO: make BTL350 specific derived class

TM1680MatrixGFX matrix = TM1680MatrixGFX(0x73, 11, 11);

// Snow variables
int8_t snowX[] = {2, 8, 4}; 
float snowY[] = {0, 3, 6};
float snowSpeed = 0.3; 
int frameCounter = 0;

void setup() {
  matrix.begin();
  matrix.setTextWrap(false);
  matrix.setTextSize(1);
}

void loop() {
  static bool toggle = false;
  static bool fShowPresents = false;
  drawChristmasScene(toggle ? GFX_MATRIX_YELLOW : GFX_MATRIX_WHITE, toggle, fShowPresents);
  toggle = !toggle;
  frameCounter++;

  if (frameCounter >= 80) {
    scrollMessage("Fijne feestdagen!");
    frameCounter = 0; 
    fShowPresents=!fShowPresents;
  }
  delay(200); 
}

void scrollMessage(const char* msg) {
  int16_t msgWidth = strlen(msg) * 6;
  for (int16_t x = 11; x >= -msgWidth; x--) {
    matrix.fillScreen(GFX_MATRIX_BLACK);
    matrix.setCursor(x, 2);
    matrix.setTextColor(x/8 % 2 ? GFX_MATRIX_RED : GFX_MATRIX_GREEN);
    matrix.print(msg);
    matrix.write();
    delay(85);
  }
  matrix.fillScreen(GFX_MATRIX_BLACK);
  matrix.write();
}

void drawChristmasScene(uint8_t starCore, bool toggle, bool fShowPresents) {
  matrix.fillScreen(GFX_MATRIX_BLACK);

/*
  // 1. SNOWFALL
  for (int i = 0; i < 3; i++) {
    matrix.drawPixel(snowX[i], (int)snowY[i], GFX_MATRIX_WHITE);
    snowY[i] += snowSpeed;
    if (snowY[i] > 10) { snowY[i] = 0; snowX[i] = random(0, 11); }
  }
*/  

  // 2. GROUND & STAR
  matrix.drawFastHLine(0, 10, 11, GFX_MATRIX_WHITE);
  matrix.drawPixel(9, 1, starCore);
  matrix.drawPixel(9, 0, GFX_MATRIX_WHITE);
  matrix.drawPixel(8, 1, GFX_MATRIX_WHITE);
  matrix.drawPixel(10, 1, GFX_MATRIX_WHITE);
  matrix.drawPixel(9, 2, GFX_MATRIX_WHITE);

  // 3. THE TREE & TRUNK
  matrix.fillTriangle(5, 1, 3, 3, 7, 3, GFX_MATRIX_GREEN);
  matrix.fillTriangle(5, 3, 2, 6, 8, 6, GFX_MATRIX_GREEN);
  matrix.fillTriangle(5, 5, 1, 8, 9, 8, GFX_MATRIX_GREEN);
  matrix.drawPixel(5, 9, GFX_MATRIX_YELLOW); 

  // 4. GIFT BOXES
  if(fShowPresents) {
    matrix.fillRect(0, 7, 3, 3, GFX_MATRIX_RED); // Large Box
    matrix.drawFastHLine(0, 8, 3, GFX_MATRIX_YELLOW);
    matrix.drawFastVLine(1, 7, 3, GFX_MATRIX_YELLOW);

    matrix.fillRect(7, 8, 2, 2, GFX_MATRIX_BLUE); // Small Box
    matrix.drawPixel(7, 8, GFX_MATRIX_CYAN);
  }

  // 5. ORNAMENTS (With the requested spacing adjustments)
  if (toggle) {
    matrix.drawPixel(4, 3, GFX_MATRIX_RED);
    matrix.drawPixel(6, 5, GFX_MATRIX_BLUE); // Moved UP from (6,6)
    matrix.drawPixel(3, 8, GFX_MATRIX_BLUE); 
  } else {
    matrix.drawPixel(6, 3, GFX_MATRIX_CYAN);
    matrix.drawPixel(4, 6, GFX_MATRIX_MAGENTA);
    matrix.drawPixel(6, 7, GFX_MATRIX_RED);  // Moved UP/LEFT from (7,8)
  }

  // 1. SNOWFALL
  for (int i = 0; i < 3; i++) {
    matrix.drawPixel(snowX[i], (int)snowY[i], GFX_MATRIX_WHITE);
    snowY[i] += snowSpeed;
    if (snowY[i] > 10) { snowY[i] = 0; snowX[i] = random(0, 11); }
  }

  matrix.write(); 
}