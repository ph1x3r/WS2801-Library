/* includes for driving the LED pixel string
 * This WS2801 library uses the SPI hardware and the pins
 * are determined by the hardware design of the board
 *
 * SS   - Digital 10  (pin 0 [PB0] on Teensy) not used
 * SCLK - Digital 13  (pin 1 [PB1] on Teensy)
 * MOSI - Digital 11  (pin 2 [PB2] on Teensy)
 * MISO - Digital 12  (pin 3 [PB3] on Teensy) not used
 */
#include <SPI.h>
#include "WS2801.h"

/* includes for talking to the LCD panel
 * adafruit LCD backpack uses the I2C port and the pins
 * are determined by the hardware design of the board
 *
 * CLK to Analog #5 (pin #5 on Teensy)
 * DAT to Analog #4 (pin #6 on Teensy)
 */
#include <Wire.h>
#include <LiquidTWI.h>


/* Set the size of the matrix that the LED string is
 * configured as
 */
int boxWidth=11;
int boxHeight=15;

/* set the number of LEDs as a matrix of x and y size and the library
 * figures the rest out for you
 */
WS2801 strip = WS2801(boxWidth, boxHeight);

// Connect to LCD display via i2c, default address #0 (A0-A2 not jumpered)
LiquidTWI lcd(0);

void setup() {
  
  // initialize serial port to feed data back to the host
  Serial.begin(38400);
  
  // initialize LCD and set up number of rows and columns: 
  lcd.begin(20, 4);
  // turn on the backlight
  lcd.setBacklight(HIGH);
  // clear the screen
  lcd.clear();
  
  // initialize the library - sets all pixels to '0' (no color)
  strip.begin();
  // Update the strip, to start they are all 'off' afer initialization
  strip.show();
  
  // wait for start command key, or until timeout
  establishContact();
}

void establishContact() {
  int xx=0;
  lcd.setCursor(0, 0);
  lcd.print("Wait for Start CMD");
  lcd.setCursor(0, 3);
  lcd.print("(C) ph1x3r@gmail.com");
  // timeout and just get on with the display if no key pressed
  while (Serial.available() <= 0 && xx<10) {
    Serial.print('A', BYTE);   // send a capital A
      lcd.setCursor(0, 1);
      lcd.print(xx++);
    delay(500);
  }
}


void loop() {

  // Some example procedures showing how to display to the pixels

  // original adafruit function. 
  // run the length of the strand and set the color as defined
  //
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Color Wipe");

  // Usage:  colorWipe(color, wait)
  // color is the 24bit RGB value and wait is the time between pixels
  
  strip.clear();
  strip.bgAll(0x000008);  // set background color
  colorWipe(strip.Color(255, 0, 0), 10);

  strip.clear();
  strip.bgAll(0x080000);  // set background color
  colorWipe(strip.Color(0, 255, 0), 10);

  strip.clear();
  strip.bgAll(0x000800);  // set background color
  colorWipe(strip.Color(0, 0, 255), 10);




  // demonstrate new function for drawing fast horizontal and vertical lines
  //
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Fast Lines");

  // Usage:  HVLine(x, y, length, direction, color)
  // x,y is the start of the line.
  // length is the length of the line
  // direction  0=right, 1=down, 2=left, 3=up
  // color is the 24bit RGB value

  strip.clear();
  strip.bgAll(0x080808);  // set background color
  strip.HVLine(5, 7, 5, 0, strip.Color(0, 255, 0));
  strip.show();
  delay(500);

  strip.HVLine(5, 7, 5, 1, strip.Color(255, 255, 0));
  strip.show();
  delay(500);

  strip.HVLine(5, 7, 5, 2, strip.Color(0, 255, 255));
  strip.show();
  delay(500);

  strip.HVLine(5, 7, 5, 3, strip.Color(0, 0, 255));
  strip.show();
  delay(500);

  // draw a growing circle in the center of the grid
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Circles");

  // Usage:  circle(x, y, radius, color)
  strip.bgAll(0x050505);  // set background color
  strip.clear();
  for(int xx=0; xx < 6; xx++)
  {
    strip.circle(5,7,xx, 0x8f008f);
    strip.show();
    delay(500);
    strip.clear();
  }

  // this will clear the background color and leave
  // the large circle intact
  strip.bgAll(0x0);
  strip.show();
  delay(2000);

  // draw triangles
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Triangles");

  // Usage:  triangle(x0,y0,  x1,y1,  x2,y2,  color)
  strip.clear();
  strip.triangle(5,0, 0,5, 5,5, 0x00ff00);
  strip.show();
  delay(500);
  strip.triangle(4,8, 5,14, 8,14, 0xff0000);
  strip.show();
  delay(500);
  
  // display a shrinking box
  // this will erase the triangles as it shrinks
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Shrinking Box");

  // rough average for steps from edge to center
  int steps = abs((boxWidth+boxHeight)/4);
  for(int xx=0; xx<steps; xx++)
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
  
  // shrinking box, but now it is filled in.
  lcd.setCursor(0, 0);
  lcd.print("Shrink Fill");
  lcd.clear();

  // display a shrinking filled box
  for(int xx=0; xx<steps; xx++)
  {
    int x1=0;
    int y1=0;
    int x2=boxWidth-1;
    int y2=boxHeight-1;
    strip.boxFill(x1+xx, y1+xx, x2-xx, y2-xx, strip.Color(0, 0, 255));
    strip.show();
    delay(500);
    //strip.boxFill(x1+xx, y1+xx, x2-xx, y2-xx, strip.Color(0, 0, 0));
    strip.clear();
  }

  // repeat with a different color
  for(int xx=0; xx<steps; xx++)
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

  // repeat with a different color
  for(int xx=0; xx<steps; xx++)
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
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Twinkle Lights");

  strip.clear();
  strip.Twinkle(30, 50);

  // draw a spinning line on the display
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Spining Line");
  strip.clear();

  for(int xx=0; xx<255; xx+=25)
  {
    strip.SpinLine(20, strip.Color(128, xx, 32));
  }

  // do a mix of horizontal and vertical wipes
  // across the display
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Moving Lines");

  strip.clear();
  //Do some wipes of color on the grid.
  strip.MoveLinesV(50, strip.Color(128,0,0));
  strip.MoveLinesRV(50, strip.Color(128,0,0));
  strip.MoveLinesH(50, strip.Color(128,0,0));
  strip.MoveLinesRH(50, strip.Color(128,0,0));

  strip.MoveLinesV(50, strip.Color(64,64,0));
  strip.MoveLinesRV(50, strip.Color(64,64,0));
  strip.MoveLinesH(50, strip.Color(64,64,0));
  strip.MoveLinesRH(50, strip.Color(64,64,0));

  strip.MoveLinesV(50, strip.Color(0,254,0));
  strip.MoveLinesRV(50, strip.Color(0,254,0));
  strip.MoveLinesH(50, strip.Color(0,254,0));
  strip.MoveLinesRH(50, strip.Color(0,254,0));

  strip.MoveLinesV(50, strip.Color(0,254,254));
  strip.MoveLinesRV(50, strip.Color(0,254,254));
  strip.MoveLinesH(50, strip.Color(0,254,254));      
  strip.MoveLinesRH(50, strip.Color(0,254,254));

  strip.MoveLinesV(50, strip.Color(0,0,254));
  strip.MoveLinesRV(50, strip.Color(0,0,254));
  strip.MoveLinesH(50, strip.Color(0,0,254));
  strip.MoveLinesRH(50, strip.Color(0,0,254));

  // draw a pixel along the display with a 'tail'
  // of decreasing brightness
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Comet Tail");

  strip.clear();
  // draw a pixel moving along the strand with a 'tail' as it travels
  // Usage:  comet(delay, reg, green, blue, length)
  strip.comet(20, 254, 0, 0, 5);
  strip.comet(20, 0, 254, 0, 10);
  strip.comet(20, 254, 254, 254, 20);


  // original adafruit calls for rainbow display on the strand
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Rainbow One");

  strip.clear();    
  rainbow(10);
  

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Rainbow Two");

  strip.clear();
  rainbowCycle(10);
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
  } 
  else if (WheelPos < 170) {
    WheelPos -= 85;
    return strip.Color(255 - WheelPos * 3, 0, WheelPos * 3);
  } 
  else {
    WheelPos -= 170; 
    return strip.Color(0, WheelPos * 3, 255 - WheelPos * 3);
  }
}

