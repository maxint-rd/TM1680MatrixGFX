/*

TM16xxMatrixGFX.h - Adafruit GFX LED Matrix library for TM16xx.

Made by Maxint R&D. See https://github.com/maxint-rd/

*/
#include <Adafruit_GFX.h>
#include "TM1680MatrixGFX.h"

#define TM1680MatrixGFX_swap(a, b) { int16_t t = a; a = b; b = t; }

#include <Wire.h>

TM1680MatrixGFX::TM1680MatrixGFX(byte nAddressI2C, byte nColumns, byte nRows) : Adafruit_GFX(nRows, nColumns)
{ // TM1680 can use either 0x73 or 0x71
	_nColumns=nColumns;
	_nRows=nRows;
	_fMirrorX=false;
	_fMirrorY=false;
  _nAddressI2C=nAddressI2C;
	
/*
	// An offscreen bitmap is required to set an individual pixel, while retaining the others.
  // TM1680/TM1681 can drive 8x32 up to 16x24 leds; requiring up to 48 bytes of data (addressed as 96 nibbles).
  fillScreen(0);
*/
}

void TM1680MatrixGFX::begin()
{
  //Serial.println("Start TM1680 init");
  
  Wire.begin();             // join i2c bus
  Wire.setClock(400000UL);  // we want 400khz?


  // Init the TM1680, same sequence as captured by logic analyczer:
  Wire.beginTransmission(_nAddressI2C);  // transmit to device 73
  Wire.write(TM1680_SYS_DIS);              // SYS DIS
  Wire.write(TM1680_COM_01);     // * 01=16-com NMOS -> 24SEG x 16COM => max 24 digits on a 15-segment CA LED display (tested 5241BS dual digit, 14-seg + dot)
  Wire.write(TM1680_RC_1);              // RC Master Mode 1
  Wire.write(TM1680_SYS_EN);              // SYS EN
  Wire.write(TM1680_LED_ON);              // LED ON
  Wire.write(TM1680_PWM_DUTY_0);              // PWM Duty Min (BF Duty max)
  Wire.write(TM1680_BLINK_OFF);              // Blink Off
  Wire.endTransmission();        // stop transmitting

  //Serial.println("Done with TM1680 init");
}

void TM1680MatrixGFX::setIntensity(byte intensity)
{	// set the intensity
  Wire.beginTransmission(_nAddressI2C);  // transmit to device 73
  Wire.write(TM1680_PWM_DUTY_0 | intensity%16);              // PWM Duty Min (BF Duty max)
  Wire.endTransmission();        // stop transmitting
}

void TM1680MatrixGFX::setMirror(bool fMirrorX, bool fMirrorY)	// fMirrorX=false, fMirrorY=false
{
	_fMirrorX=fMirrorX;
	_fMirrorY=fMirrorY;
}

/*
void TM1680MatrixGFX::fillScreen(uint16_t color)
{	// set the offscreen bitmap to the specified color
  memset(bitmap, color ? 0xff : 0, bitmapSize);
}
*/

bool TM1680MatrixGFX::convertToMemPos(int16_t &x, int16_t &y)
{	// Convert x/y coordinates to bitmap memory position (array with rows of 8 pixels per byte)
	// Given coordinates are passed by reference and changed to the required range
	// Returns false if coordinates fall outside of canvas after processing rotation
	if (rotation)
	{
		// Implement Adafruit's rotation.
		byte tmp;
		if ( rotation >= 2 ) {										// rotation == 2 || rotation == 3
			x = _width - 1 - x;
		}

		if ( rotation == 1 || rotation == 2 ) {		// rotation == 1 || rotation == 2
			y = _height - 1 - y;
		}

		if ( rotation & 1 ) {     								// rotation == 1 || rotation == 3
			tmp = x; x = y; y = tmp;
		}
	}

	if ( x < 0 || x >= WIDTH || y < 0 || y >= HEIGHT ) {
		// Ignore pixels outside the canvas.
		return(false);
	}

/*		// TODO?: support for different module orientaton and layout? (currently only left-top to right-bottom)
	// Translate the x, y coordinate according to the layout of the
	// displays. They can be ordered and rotated (0, 90, 180, 270).

	byte display = matrixPosition[(x >> 3) + hDisplays * (y >> 3)];
	x &= 0b111;
	y &= 0b111;

	byte r = matrixRotation[display];
	if ( r >= 2 ) {										   // 180 or 270 degrees
		x = 7 - x;
	}
	if ( r == 1 || r == 2 ) {				     // 90 or 180 degrees
		y = 7 - y;
	}
	if ( r & 1 ) {     								   // 90 or 270 degrees
		tmp = x; x = y; y = tmp;
	}

	byte d = display / hDisplays;
	x += (display - d * hDisplays) << 3; // x += (display % hDisplays) * 8
	y += d << 3;												 // y += (display / hDisplays) * 8

	// Update the color bit in our bitmap buffer.
	byte *ptr = bitmap + x + WIDTH * (y >> 3);
	byte val = 1 << (y & 0b111);

	if ( color ) {
		*ptr |= val;
	}
	else {
		*ptr &= ~val;
	}
*/

	// mirror display (fMirrorX true for WeMOS mini matrix)
	if(_fMirrorX)
		x=WIDTH-x-1;
	if(_fMirrorY)
		y=HEIGHT-y-1;

	return(true);
}

void TM1680MatrixGFX::drawPixel(int16_t xx, int16_t yy, uint16_t color)
{	// set the specified pixel as wanted in the memory
	// Operating in bytes is faster and takes less code to run. We don't
	// need values above 200, so switch from 16 bit ints to 8 bit unsigned
	// ints (bytes).
	//int8_t x = xx;
	//int8_t  y = yy;
  int16_t x=xx; int16_t y=yy;
	if (rotation)
	{
		// Implement Adafruit's rotation.
		byte tmp;
		if ( rotation >= 2 ) {										// rotation == 2 || rotation == 3
			x = _width - 1 - x;
		}

		if ( rotation == 1 || rotation == 2 ) {		// rotation == 1 || rotation == 2
			y = _height - 1 - y;
		}

		if ( rotation & 1 ) {     								// rotation == 1 || rotation == 3
			tmp = x; x = y; y = tmp;
		}
	}
  
  	// mirror display (fMirrorX true for WeMOS mini matrix)
	if(_fMirrorX)
		x=_width-x-1;
	if(_fMirrorY)
		y=_height-y-1;

  if(x>=0 && x<_width && y>=0 && y<_height)
    setPixel(x, y, color);
}

 // required for scroll support as implemented by Adafruit GFX pull request #60
uint16_t TM1680MatrixGFX::getPixel(int16_t x, int16_t y)
{
  (void) x;  // suppress warning for unused parameter
  (void) y;  // suppress warning for unused parameter
/*
  if ((x < 0) || (x >= _width) || (y < 0) || (y >= _height))
    return 0;

	if(!convertToMemPos(x, y))
		return 0;

  return (bitmap[y+ (x/8)*WIDTH] >> (x%8)) & 0x1;
*/
  return(0);
}

void TM1680MatrixGFX::write()
{	// write the memory to the display
  sendfullarray();
}

uint16_t TM1680MatrixGFX::color565(byte nR, byte nG, byte nB)
{  // return RGB color value
  return((nR?bit(GFX_MATRIX_RED):0)|(nG?bit(GFX_MATRIX_GREEN):0)|(nB?bit(GFX_MATRIX_BLUE):0));
}


void TM1680MatrixGFX::testBlinkAll(unsigned int uWait)
{
  //Blink while waiting for some time

  //Turn on all:
  for (int i = 0; i < 48; i++) {
    Wire.beginTransmission(_nAddressI2C);  // transmit to device 73
    Wire.write(i*2);               // Address for two nibbles
    Wire.write(0xFF);              // both nibbles full on
    Wire.endTransmission();        // stop transmitting
  }
  delay(100);

  Wire.beginTransmission(_nAddressI2C);  // transmit to device 73
  Wire.write(TM1680_BLINK_2HZ);              // Blink 0.5Hz
  Wire.endTransmission();        // stop transmitting
  delay(uWait);
  Wire.beginTransmission(_nAddressI2C);  // transmit to device 73
  Wire.write(TM1680_BLINK_OFF);              // Blink Off
  Wire.endTransmission();        // stop transmitting
}

void TM1680MatrixGFX::sendfullarray()
{ // Send the full frame of display memory array
  // Arduino has a 32byte limit, so split the send in 2:
  Wire.beginTransmission(_nAddressI2C);  // transmit to device 73
  Wire.write(0);                 // Start on 00
  for (unsigned int i = 0; i < 24; i++)
    Wire.write(_bitmap[i]);
  Wire.endTransmission();  // stop transmitting

  Wire.beginTransmission(_nAddressI2C);  // transmit to device 73
  Wire.write(48);                // Start on 48 (24x2 send above)
  for (unsigned int i = 24; i < sizeof _bitmap / sizeof _bitmap[0]; i++)
    Wire.write(_bitmap[i]);

  Wire.endTransmission();  // stop transmitting
}

void TM1680MatrixGFX::testAllPixels()
{
  for (unsigned int i = 0; i < sizeof _bitmap / sizeof _bitmap[0]; i++) {
    byte s = 0x01;  //init with 1 bit set
    for (uint8_t j = 0; j < 8; j++) {
      _bitmap[i] = s;
      Serial.print(i);
      Serial.print(".");
      Serial.println(_bitmap[i], BIN);
      sendfullarray();
      delay(1);
      s = s << 1;  //bitshift
    }
    _bitmap[i] = 0x00;  //reset to 0
  }
}

uint16_t TM1680MatrixGFX::getConv(byte x, byte y, byte c)
{ // Get a bit position for a specific pixel color led (color led bit 2=red, 1=green, 0=blue)
  return(_convarray[x*3+y*11*3+c]);
}

void TM1680MatrixGFX::setPixel(byte x, byte y, byte color)
{ // Set the color of a specific pixel.
  // x, y: 0-10. [0,0]=left-top, [10,10]=bottom-right.
  // color: 3 bit RGB: 00000RGB, values 0-7:
  //   Off: 0=Off
  //   Primary colors: 1=Red, 2=Green, 4=Blue. 
  //   Mixed colors:   3=Yellow, 5=Purple, 6=Cyan, 7=White

  // Convert pixel position to frame array bit position for each LED in the pixel
  color=color%8;    // color is limited to three bit values: 0-7
  uint16_t nConvR=getConv(x, y, 0);       // bit-address of red LED
  uint16_t nConvG=getConv(x, y, 1);       // bit-address of green LED
  uint16_t nConvB=getConv(x, y, 2);       // bit-address of blue LED

/*
  Serial.print(x);
  Serial.print(",");
  Serial.print(y);
  Serial.print(" c");
  Serial.print(color);
  Serial.print(" bit R");
  Serial.print(nConvR);
  Serial.print("x");
  Serial.print(nConvR,HEX);
  Serial.print(" bytes R");
  Serial.print(nConvR/8);
  Serial.print("/G");
  Serial.print(nConvG/8);
  Serial.print("/B");
  Serial.print(nConvB/8);
  Serial.print(" --- R");
  Serial.print(_bitmap[nConvR/8], BIN);   // Note: print RED memory content before
*/
  // update pixel, colors can be separated across multiple RAM positions
  bitWrite(_bitmap[(nConvR/8)%48], nConvR%8, color&bit(0) ? 1 : 0);
  bitWrite(_bitmap[(nConvG/8)%48], nConvG%8, color&bit(1) ? 1 : 0);
  bitWrite(_bitmap[(nConvB/8)%48], nConvB%8, color&bit(2) ? 1 : 0);

/*
  Serial.print("=>");
  Serial.println(_bitmap[nConvR/8], BIN);   // Note: print RED memory content after
*/
}