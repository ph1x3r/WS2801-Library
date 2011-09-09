#include "WS2801.h"

/*********************************************************
 * sketch for driving the LED Pixel strings from Adafruit *
 *********************************************************/
 
// Choose which 2 pins you will use for output.
// Can be any valid output pins.
//
// This is now defined in the header file because of speed improvement in
// the output function.
//
// The colors of the wires may be totally different so
// BE SURE TO CHECK YOUR PIXELS TO SEE WHICH WIRES TO USE!
int clockPin = cPIN;      // 'blue' wire
int dataPin = dPIN;       // 'green' wire
// Don't forget to connect 'yellow' to ground and 'red' to +5V

// Set the first variable to the NUMBER of pixels. 25 = 25 pixels in a row
// int pixLength=87;

// Set the size of the matrix
int boxWidth=11;
int boxHeight=15;

// If you only want to use the string as a string, then use this function
// WS2801 strip = WS2801(25, dataPin, clockPin);

// or set the number of LEDs as a matrix of x and y size and the library
// figures the rest out for you
WS2801 strip = WS2801(boxWidth, boxHeight, dataPin, clockPin);

void setup() {
  
  // initialize the library - sets all pixels to '0' (no color)
  strip.begin();

  // Update the strip, to start they are all 'off' afer initialization
  strip.show();
}


void loop() {
  
  // Some example procedures showing how to display to the pixels
  
  // since we keep looping back here, then clear the display
  strip.clear();
 
  // original adafruit function. 
  // run the length of the strand and set the color as defined
  // Usage:  colorWipe(color, wait)
  // color is the 24bit value and wait is the time between pixels
  colorWipe(strip.Color(255, 0, 0), 10);
  strip.clear();
  colorWipe(strip.Color(0, 255, 0), 10);
  strip.clear();
  colorWipe(strip.Color(0, 0, 255), 10);
  strip.clear();
  colorWipe(strip.Color(255, 255, 255), 10);
  delay( 50);

  // display a shrinking box
  int steps = abs((boxWidth+boxHeight)/4);
  int xx; 
  for(xx=0; xx<steps; xx++)
  {
    int x1=0;
    int y1=0;
    int x2=boxWidth-1;
    int y2=boxHeight-1;
    strip.box(x1+xx, y1+xx, x2-xx, y2-xx, strip.Color(0, 0, 255));
    strip.show();
    delay(500);
    strip.box(x1+xx, y1+xx, x2-xx, y2-xx, strip.Color(0, 0, 0));
  }
  
    // different color
  for(xx=0; xx<steps; xx++)
  {
    int x1=0;
    int y1=0;
    int x2=boxWidth-1;
    int y2=boxHeight-1;
    strip.box(x1+xx, y1+xx, x2-xx, y2-xx, strip.Color(128, 128, 0));
    strip.show();
    delay(500);
    strip.box(x1+xx, y1+xx, x2-xx, y2-xx, strip.Color(0, 0, 0));
  }
      // different color
  for(xx=0; xx<steps; xx++)
  {
    int x1=0;
    int y1=0;
    int x2=boxWidth-1;
    int y2=boxHeight-1;
    strip.box(x1+xx, y1+xx, x2-xx, y2-xx, strip.Color(256, 128, 63));
    strip.show();
    delay(500);
    strip.box(x1+xx, y1+xx, x2-xx, y2-xx, strip.Color(0, 0, 0));
  }
  
  // blink all the lights in random colors
  strip.Twinkle(30, 50);
  

      //Do some wipes of color on the grid.
      strip.MoveLinesV(5, strip.Color(128,0,0));
      strip.MoveLinesRV(5, strip.Color(128,0,0));
      strip.MoveLinesH(5, strip.Color(128,0,0));
      strip.MoveLinesRH(5, strip.Color(128,0,0));
      
      strip.MoveLinesV(5, strip.Color(64,64,0));
      strip.MoveLinesRV(5, strip.Color(64,64,0));
      strip.MoveLinesH(5, strip.Color(64,64,0));
      strip.MoveLinesRH(5, strip.Color(64,64,0));
      
      strip.MoveLinesV(5, strip.Color(0,254,0));
      strip.MoveLinesRV(5, strip.Color(0,254,0));
      strip.MoveLinesH(5, strip.Color(0,254,0));
      strip.MoveLinesRH(5, strip.Color(0,254,0));

      strip.MoveLinesV(5, strip.Color(0,254,254));
      strip.MoveLinesRV(5, strip.Color(0,254,254));
      strip.MoveLinesH(5, strip.Color(0,254,254));      
      strip.MoveLinesRH(5, strip.Color(0,254,254));

      strip.MoveLinesV(5, strip.Color(0,0,254));
      strip.MoveLinesRV(5, strip.Color(0,0,254));
      strip.MoveLinesH(5, strip.Color(0,0,254));
      strip.MoveLinesRH(5, strip.Color(0,0,254));
      
      strip.MoveLinesV(5, strip.Color(254,0,254));
      strip.MoveLinesRV(5, strip.Color(254,0,254));
      strip.MoveLinesH(5, strip.Color(254,0,254));
      strip.MoveLinesRH(5, strip.Color(254,0,254));  

  strip.clear();

  // draw a pixel moving along the strand with a 'tail' as it travels
  strip.comet(1, 254, 0, 0, 1);
  strip.comet(1, 0, 254, 0, 1);
  strip.comet(1, 254, 254, 254, 1);

  strip.clear();
  for(xx=0; xx<255; xx+=25)
  {
    strip.SpinLine(50, strip.Color(128, xx, 32));
  }

  // original adafruit calls for rainbow display on the strand
  rainbow(5);
  strip.clear();
  
  rainbowCycle(5);
}

// original adafruit code for 'rainbow' effect on the strand
void rainbow(uint8_t wait) {
  int i, j;
   
  for (j=0; j < 256 *3; j++) {     // 3 cycles of all 256 colors in the wheel
    for (i=0; i < strip.numPixels(); i++) {
      strip.setPixelColor(i, Wheel( (i + j) % 255));
    }  
    strip.show();   // write all the pixels out
    delay(wait);
  }
}

// Slightly different, this one makes the rainbow wheel equally distributed 
// along the chain
void rainbowCycle(uint8_t wait) {
  int i, j;
  
  for (j=0; j < 256 * 5; j++) {     // 5 cycles of all 256 colors in the wheel
    for (i=0; i < strip.numPixels(); i++) {
      // tricky math! we use each pixel as a fraction of the full 256-color wheel
      // (thats the i / strip.numPixels() part)
      // Then add in j which makes the colors go around per pixel
      // the % 256 is to make the wheel cycle around
      strip.setPixelColor(i, Wheel( ((i * 256 / strip.numPixels()) + j) % 256) );
    }  
    strip.show();   // write all the pixels out
    delay(wait);
  }
}

// fill the dots one after the other with said color
// good for testing purposes
void colorWipe(uint32_t c, uint8_t wait) {
  int i;
  
  for (i=0; i < strip.numPixels(); i++) {
      strip.setPixelColor(i, c);
      strip.show();
      delay(wait);
  }
}

/* Helper functions */

//Input a value 0 to 255 to get a color value.
//The colours are a transition r - g -b - back to r
uint32_t Wheel(byte WheelPos)
{
  if (WheelPos < 85) {
   return strip.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
  } else if (WheelPos < 170) {
   WheelPos -= 85;
   return strip.Color(255 - WheelPos * 3, 0, WheelPos * 3);
  } else {
   WheelPos -= 170; 
   return strip.Color(0, WheelPos * 3, 255 - WheelPos * 3);
  }
}
