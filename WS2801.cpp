#include "WS2801.h"

#include <pins_arduino.h>
#include <SPI.h>

// Example to control WS2801-based RGB LED Modules in a strand or strip
// Written by Adafruit - MIT license
/*****************************************************************************
 * Additions made by ph1x3r (ph1x3r@gmail.com) for manipulating the LED
 * pixels when they are organized as a grid
 *
 * Assumes LEDS are layed out in a zig zag manner eg for a 3x3:
 * 0 5 6
 * 1 4 7
 * 2 3 8
 *
 * Some grid code (c) 2010 by Ben Moyes, Bliptronics, http:www.bliptronics.com
 * ported into this library by ph1x3r (ph1x3r@gmail.com)
 *****************************************************************************/

// used in the line drawing algorithm
#define swap(a, b) { int16_t t = a; a = b; b = t; }

// SPI ports are part of the hardware design. Use defaults.
// pin names from the pins_arduino.h file
// only DATAOUT and SPICLOCK are used, the rest are ignored
// =====+=====+========
// pin  | Uno | Teensy2
// =====+=====+========
// MOSI | 11  | 2
// MISO | 12  | 3
// SCLK | 13  | 1
// SS   | 10  | 0

/*
Data for setting the SPI control register - informational only
SPCR
| 7    | 6    | 5    | 4    | 3    | 2    | 1    | 0    |
| SPIE | SPE  | DORD | MSTR | CPOL | CPHA | SPR1 | SPR0 |

SPIE - Enables the SPI interrupt when 1
SPE  - Enables the SPI when 1
DORD - Sends data least Significant Bit First when 1, most Significant Bit first
MSTR - Sets the Arduino in master mode when 1, slave mode when 0
CPOL - Sets the data clock to be idle when high if set to 1, idle when low if se
CPHA - Samples data on the falling edge of the data clock when 1, rising edge wh
SPR1 and SPR0 - Sets the SPI speed, 00 is fastest (4MHz) 11 is slowest (250KHz)
*/

//-----
// initialize with the length of the string
// set height and width to a resonable number just in case one of the
// grid functions is called
//-----
WS2801::WS2801(uint16_t n)
{
    gridWidth=n;
    gridHeight=1;
    numLEDs = n;

    pixels = (uint32_t *)malloc(numLEDs);
    for (uint16_t i=0; i< numLEDs; i++)
    {
        setPixelColor(i, 0, 0, 0);
    }
}


//-----
// ph1x3r - initialize with array size
// set all the pixels to zero to start with
//-----
WS2801::WS2801(uint16_t x, uint16_t y)
{
    gridWidth=x;
    gridHeight=y;
    numLEDs = x * y;

    pixels = (uint32_t *)malloc(numLEDs);
    for (uint16_t i=0; i< numLEDs; i++)
    {
        setPixelColor(i, 0, 0, 0);
    }
}


//-----
// set up the pins on the arduino
//-----
void WS2801::begin(void)
{
  // set up SPI port pins
  // function sets SS to HIGH as part of initialization
  SPI.begin();

  // SPCR = 01010001
  //interrupt disabled,spi enabled,msb 1st,master,clk low when idle,
  //sample on leading edge of clk,system clock/8 rate (01)
  SPCR = (1<<SPE) | (1<<MSTR) | (1<<SPR0);

  // junk any data in the status register or data input register
  byte clr=SPSR;
  clr=SPDR;
}

//-----
// helper function to allow external programs to retrieve the number of LEDs
//-----
uint16_t WS2801::numPixels(void)
{
    return numLEDs;
}


//---
// ph1x3r - digitalWrite is way too slow for sending pixels.
// Use Arduino SPI hardware to increase speed
// now running at 28uS per pixel
void WS2801::show(void)
{
    // my SPI protocol analyzer needs SS to be toggled
    // enabling this makes very little difference in the frame rate
    // but it can be commented out without impact
    digitalWrite(SS,LOW);

    // send all the pixels
    // pixel data is stored as 32bit numbers, and SPI only works with
    // 8 bit numbers, so bitshift and mask to get the info
    for (uint16_t p=0; p< numLEDs; p++)
    {
      // method is the same as SPI.transfer() but saves the call
      // send red pixel
      SPDR = pixels[p]>>16 & 0xff;
      while (!(SPSR & (1<<SPIF))) {};
      // send green pixel
      SPDR = pixels[p]>>8 & 0xff;
      while (!(SPSR & (1<<SPIF))) {};
      // send blue pixel
      SPDR = pixels[p] & 0xff;
      while (!(SPSR & (1<<SPIF))) {};
    }

    // for my protocol analyzer - see above
    digitalWrite(SS,HIGH);
}

void WS2801::setPixelColor(uint16_t n, uint8_t r, uint8_t g, uint8_t b)
{
    uint32_t data;

    if (n > numLEDs) return;

    data = r;
    data <<= 8;
    data |= g;
    data <<= 8;
    data |= b;

    pixels[n] = data;
}


void WS2801::setPixelColor(uint16_t n, uint32_t c)
{
    if (n > numLEDs) return;

    // mask off lower 24 bits
    pixels[n] = c & 0xFFFFFF;
}


//---
// Translate x and y to a LED index number in an array.
//---
uint16_t WS2801::Translate(uint16_t x, uint16_t y)
{
    if(x%2)
    {
        return(((x+1) * gridHeight)- 1 - y);
    }
    else
    {
        return((x * gridHeight) + y);
    }
}


//---
// ph1x3r - create a single color value out of RGB values
//---
uint32_t WS2801::Color(uint8_t r, uint8_t g, uint8_t b)
{
    uint32_t data;

    data = r;
    data <<= 8;
    data |= g;
    data <<= 8;
    data |= b;

    return(data);
}


//=====
// set a linear block of pixels to a color byte (ph1x3r)
//=====
void WS2801::setRange(uint16_t startLED, uint16_t endLED, uint32_t color )
{
    uint16_t xx;

    for(xx=startLED; xx<=endLED; xx++)
    {
        setPixelColor(startLED++, color);
    }
}

//-----
// draw a horizontal or vertical line from an XY starting point
// for len length and in dir direction
// 0=right, 1=down, 2=left, 3=up
//-----
void WS2801::HVLine(int x, int y, int len, int dir, uint32_t color)
{
     if(dir==0) { 
        for(int delta=0; delta<len; delta++) {
          setPixelColor(Translate(x+delta, y), color); }
     }
     if(dir==1) { 
        for(int delta=0; delta<len; delta++) {
          setPixelColor(Translate(x, y+delta), color);}
     }
     if(dir==2) { 
        for(int delta=0; delta<len; delta++) {
          setPixelColor(Translate(x-delta, y), color);}
     }
     if(dir==3) { 
        for(int delta=0; delta<len; delta++) {
          setPixelColor(Translate(x, y-delta), color);}
     }
}

//---
// Draw a line in defined color between two points
// Using Bresenham's line algorithm, optimized for no floating point.
//---
void WS2801::line(int x0,  int y0, int x1, int y1, uint32_t color)
{
    boolean steep;
    steep= abs(y1 - y0) > abs(x1 - x0);
    if (steep) {
        swap(x0, y0);
        swap(x1, y1);
    }

    if (x0 > x1) {
        swap(x0, x1);
        swap(y0, y1);
    }

    int deltax = x1 - x0;
    int deltay = abs(y1 - y0);
    int error = 0;
    int ystep;
    int y = y0;
    int x;

    if (y0 < y1)
        ystep = 1;
    else
        ystep = -1;

    for (x=x0; x<=x1; x++)                        // from x0 to x1
    {
        if (steep) {
            pixels[Translate(y,x)]= color;        //Fast set LEDs in grid.
        } else {
            pixels[Translate(x,y)]= color;        //Fast set LEDs in grid.
        }

        error = error + deltay;

        if (2 * error >= deltax)
        {
            y = y + ystep;
            error = error - deltax;
        }
    }
}


//=====
// draw a box using the line drawing function (ph1x3r)
//=====
void WS2801::box(int x0, int y0, int x1, int y1, uint32_t color)
{
    line(x0,y0,x1,y0,color);
    line(x1,y0,x1,y1,color);
    line(x1,y1,x0,y1,color);
    line(x0,y1,x0,y0,color);

}


//=====
// draw dot along strand with fading tail (ph1x3r)
//=====
void WS2801::comet(int wait, int red, int green, int blue, int tail)
{
    int xx, yy, zz;

// run the length of the strip
    for(zz=0; zz<(numLEDs+tail); zz++)
    {
        clear();
        xx=tail;
        for(yy=254; yy>=0; yy=yy-(abs(255/tail)))
        {
            setPixelColor(zz-xx,
                Color(
                constrain(red-yy, 0, 254),
                constrain(green-yy, 0, 254),
                constrain(blue-yy, 0, 254)
                )
                );
            xx--;
        }
        show();
        delay(wait);
    }
}


//=====
//Set all leds to a random color (ph1x3r)
//=====
void WS2801::AllRandom()
{
    int xx;
    for(xx=0;xx<numLEDs;xx++)
    {
        setPixelColor(xx, Color(random(0,254),random(0,254),random(0,254) ) );
    }
}


//=====
//Set all LEDS to a single color.
//=====
void WS2801::All( uint32_t color)
{
    setRange(0,numLEDs-1,color);
}


//=====
// Twinkle all lights - argument determines how many cycles (ph1x3r)
//=====
// twinkle achieved by setting random colors to all lights and then repeating
// xx number of times
//=====
void WS2801::Twinkle(int wait, int repeat)
{
    int x;
    for(x=0;x<repeat;x++)
    {
        AllRandom();
        show();
        delay(wait);
    }
}


//=====
//Wipe horizontal lines down the grid (ph1x3r)
//=====
void WS2801::MoveLinesH(int del, uint32_t col)
{
    int xx;
    for(xx=0;xx<gridHeight;xx++)
    {
        line(0,xx,gridWidth-1,xx,col );
        show();
        delay(del);
    }

    for(xx=0;xx<gridHeight;xx++)
    {
        line(0,xx,gridWidth-1,xx,0 );
        show();
        delay(del);
    }
}


//=====
//Wipe horizontal lines up the grid (ph1x3r)
//=====
void WS2801::MoveLinesRH(int del, uint32_t col)
{
    int xx;
    for(xx=gridHeight;xx>-1;xx--)
    {
        line(0,xx,gridWidth-1,xx,col );
        show();
        delay(del);
    }

    for(xx=gridHeight;xx>-1;xx--)
    {
        line(0,xx,gridWidth-1,xx,0 );
        show();
        delay(del);
    }
}


//=====
//Wipe vertical lines across (left to right) the grid (ph1x3r)
//=====
void WS2801::MoveLinesV(int del, uint32_t col)
{
    int xx;
    for(xx=0;xx<gridWidth;xx++)
    {
        line(xx,0,xx,gridHeight-1,col );
        show();
        delay(del);
    }

    for(xx=0;xx<gridWidth;xx++)
    {
        line(xx,0,xx,gridHeight-1,0 );
        show();
        delay(del);
    }
}


//=====
//Wipe vertical lines across (right to left) the grid (ph1x3r)
//=====
void WS2801::MoveLinesRV(int del, uint32_t col)
{
    int xx;
    for(xx=gridWidth;xx>-1;xx--)
    {
        line(xx,0,xx,gridHeight-1,col );
        show();
        delay(del);
    }

    for(xx=gridWidth;xx>-1;xx--)
    {
        line(xx,0,xx,gridHeight-1,0 );
        show();
        delay(del);
    }
}


//=====
// Spin a line in the panel (clockwise) (ph1x3r)
// this does one full rotation
//=====
void WS2801::SpinLine(int del, uint32_t col)
{
    int xx, yy;

    // draw lines in the horizontal direction
    // line is erased before drawing the next line
    for(xx=0; xx<gridWidth; xx++)
    {
        line(xx,0,gridWidth-1-xx, gridHeight-1, col);
        show();
        delay(del);
        line(xx,0,gridWidth-1-xx, gridHeight-1, 0);
    }
    // draw the lines in the vertical direction
    // line is erased before drawing the next line
    for(yy=1; yy<gridHeight-1; yy++)
    {
        line(0, gridHeight-1-yy, gridWidth-1, yy, col);
        show();
        delay(del);
        line(0, gridHeight-1-yy, gridWidth-1, yy, 0);
    }
}


//=====
// shortcut for erasing all of the pixels at once (ph1x3r)
//=====
void WS2801::clear()
{
    for (uint16_t i=0; i< numLEDs; i++) {
        pixels[i] = 0;
    }
}
