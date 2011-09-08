#include "WS2801.h"

/*********************************************************
 * sketch for driving the LED strings from Stellarscapes *
 *********************************************************/
 
// Choose which 2 pins you will use for output.
// Can be any valid output pins.
// defined in header file because of speed improvement in output function
// The colors of the wires may be totally different so
// BE SURE TO CHECK YOUR PIXELS TO SEE WHICH WIRES TO USE!
int clockPin = cPIN;      // 'blue' wire
int dataPin = dPIN;       // 'green' wire
// Don't forget to connect 'yellow' to ground and 'red' to +5V

// Set the first variable to the NUMBER of pixels. 25 = 25 pixels in a row
// WS2801 strip = WS2801(25, dataPin, clockPin);

// or set the number of LEDs as a matrix of x and y size
WS2801 strip = WS2801(17, 10, dataPin, clockPin);

void setup() {

  Serial.begin(9600);
  
  // initialize the library  
  strip.begin();

  // Update the strip, to start they are all 'off' afer initialization
  strip.show();
  delay(2000);
}


void loop() {
  
  // Some example procedures showing how to display to the pixels
  
  colorWipe(strip.Color(255, 0, 0), 0);
  //colorWipe(strip.Color(0, 255, 0), 0);
  //colorWipe(strip.Color(0, 0, 255), 0);
  
  strip.clear();
  int xx;
/*  for(xx=0; xx<255; xx++)
  {
    strip.setRange(0, strip.numPixels()-1, strip.Color(xx, xx, xx));
    strip.show();
    delay(1);
  }
  for(xx=0; xx<255; xx++)
  {
    strip.setRange(0, strip.numPixels()-1, strip.Color(0, xx, 0));
    strip.show();
    delay(1);
  }
  for(xx=0; xx<255; xx++)
  {
    strip.setRange(0, strip.numPixels()-1, strip.Color(0, 0, xx));
    strip.show();
    delay(1);
  }
  for(xx=0; xx<255; xx++)
  {
    strip.setRange(0, strip.numPixels()-1, strip.Color(xx, 0, 0));
    strip.show();
    delay(1);
  }
  */
  
  for(xx=0; xx<20; xx++)
  {
    unsigned long time0, time1, time2;
    
    time0 = micros();
    time1=time0;
    strip.All(strip.Color(128, 0, 0));
    strip.show();
    time2 = micros();
    Serial.print("LED Send Time 1: ");
    Serial.println(time2 - time1);
    
    delay(50);
    time1=micros();
    strip.All(strip.Color(0, 0, 0));
    strip.show();
    time2 = micros();
    Serial.print("LED Send Time 2: ");
    Serial.println(time2 - time1);
    
    delay(50);
  }
  // clear the pixels
  strip.clear();
  strip.show();
  delay(50);
/*  
  strip.line(0, 0, 5, 0, strip.Color(0, 0, 255));
  strip.show();
  delay(500);
*/
  strip.Twinkle(30, 50);
  
/*
      //Do some wipes of color on the grid.
      strip.MoveLinesV(50, strip.Color(254,0,0));
      strip.MoveLinesH(50, strip.Color(254,0,0));
      strip.MoveLinesRV(50, strip.Color(254,0,0));
      strip.MoveLinesRH(50, strip.Color(254,0,0));
      
      strip.MoveLinesV(50, strip.Color(254,254,0));
      strip.MoveLinesRV(50, strip.Color(254,254,0));
      strip.MoveLinesH(50, strip.Color(254,254,0));
      strip.MoveLinesRH(50, strip.Color(254,254,0));
      
      strip.MoveLinesV(50, strip.Color(0,254,0));
      strip.MoveLinesH(50, strip.Color(0,254,0));
      strip.MoveLinesRV(50, strip.Color(0,254,0));
      strip.MoveLinesRH(50, strip.Color(0,254,0));

      strip.MoveLinesV(50, strip.Color(0,254,254));
      strip.MoveLinesH(50, strip.Color(0,254,254));      
      strip.MoveLinesRV(50, strip.Color(0,254,254));
      strip.MoveLinesRH(50, strip.Color(0,254,254));

      strip.MoveLinesV(50, strip.Color(0,0,254));
      strip.MoveLinesH(50, strip.Color(0,0,254));
      strip.MoveLinesRV(50, strip.Color(0,0,254));
      strip.MoveLinesRH(50, strip.Color(0,0,254));
      
      strip.MoveLinesV(50, strip.Color(254,0,254));
      strip.MoveLinesH(50, strip.Color(254,0,254));
      strip.MoveLinesRV(50, strip.Color(254,0,254));
      strip.MoveLinesRH(50, strip.Color(254,0,254));
*/     

/*  int j;
  for(j=0; j<255; j++)
  {
    strip.All(Wheel(j));
    strip.show();
    delay(5);
  }
*/
  // debug this function
  //strip.comet(10, 254, 0, 0, 1);
  //strip.comet(10, 0, 254, 0, 1);
  //strip.comet(10, 0, 0, 254, 1);

  //rainbow(20);
  rainbowCycle(50);
}

void rainbow(uint8_t wait) {
  int i, j;
   
  for (j=0; j < 256; j++) {     // 3 cycles of all 256 colors in the wheel
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
  
  for (j=0; j < 256 * 5; j++) {     // 5 cycles of all 25 colors in the wheel
    for (i=0; i < strip.numPixels(); i++) {
      // tricky math! we use each pixel as a fraction of the full 96-color wheel
      // (thats the i / strip.numPixels() part)
      // Then add in j which makes the colors go around per pixel
      // the % 96 is to make the wheel cycle around
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
