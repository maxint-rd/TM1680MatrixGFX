/*
 * TM16xxMatrixGFX_MadFly
 * 
 * Arduino example sketch to demonstrate the Adafruit GFX library on a TM1680 driven BTL-350 LED Matrix
 * 
 * Based on MadFly.pde example of the MAX72xxPanel library
 * 
 * Confirmed to work in the following environments:
 *     - CH32V006, CH32 Arduino core 1.0.4 with updates (main branche, PR#200 and others)
 * 
 * For more information see  https://github.com/maxint-rd/TM1680MatrixGFX
 */

#include <Adafruit_GFX.h>
#include "TM1680MatrixGFX.h"   // TODO: make BTL350 specific derived class

TM1680MatrixGFX matrix = TM1680MatrixGFX(0x73, 11, 11);

int pinRandom = A0;

int wait = 20; // In milliseconds

void setup() {
  matrix.begin();
  matrix.setIntensity(1); // Set brightness [between 0 and 16]
  randomSeed(analogRead(pinRandom)); // Initialize random generator
}

void loop() {
  static int x = matrix.width() / 2;
  static int y = matrix.height() / 2;
  int xNext, yNext;
  
  matrix.setIntensity((millis()/1000)%7);
  matrix.drawPixel(x, y, 1+(millis()/1000)%7);
  matrix.write(); // Send bitmap to display

  delay(wait);

  matrix.drawPixel(x, y, GFX_MATRIX_BLACK); // Erase the old position of our dot

  do {
    switch ( random(4) ) {
      case 0: xNext = constrain(x + 1, 0, matrix.width() - 1); yNext = y; break;
      case 1: xNext = constrain(x - 1, 0, matrix.width() - 1); yNext = y; break;
      case 2: yNext = constrain(y + 1, 0, matrix.height() - 1); xNext = x; break;
      case 3: yNext = constrain(y - 1, 0, matrix.height() - 1); xNext = x; break;
    }
  }
  while ( x == xNext && y == yNext ); // Repeat until we find a new coordinate

  x = xNext;
  y = yNext;
}
