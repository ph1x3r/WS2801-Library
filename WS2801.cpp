#include "WS2801.h"

#include <pins_arduino.h>
#include <SPI.h>

/* Example to control WS2801-based RGB LED Modules in a strand or strip
 * Written by Adafruit - MIT license
 *****************************************************************************
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

/* SPI ports are part of the hardware design. Use defaults.
 * pin names from the pins_arduino.h file
 * only DATAOUT and SPICLOCK are used, the rest are ignored
 * =====+=====+========
 * pin  | Uno | Teensy2
 * =====+=====+========
 * MOSI | 11  | 2
 * MISO | 12  | 3
 * SCLK | 13  | 1
 * SS   | 10  | 0
 *
 * Data for setting the SPI control register - informational only
 * SPCR
 * | 7    | 6    | 5    | 4    | 3    | 2    | 1    | 0    |
 * | SPIE | SPE  | DORD | MSTR | CPOL | CPHA | SPR1 | SPR0 |
 * 
 * SPIE - Enables the SPI interrupt when 1
 * SPE  - Enables the SPI when 1
 * DORD - Sends data least Significant Bit First when 1, most Significant Bit first
 * MSTR - Sets the Arduino in master mode when 1, slave mode when 0
 * CPOL - Sets the data clock to be idle when high if set to 1, idle when low if se
 * CPHA - Samples data on the falling edge of the data clock when 1, rising edge wh
 * SPR1 and SPR0 - Sets the SPI speed, 00 is fastest (4MHz) 11 is slowest (250KHz) */

//-----
// initialize with the length of the string
//-----
WS2801::WS2801(uint16_t n)
{
    // set height and width to a resonable number just in case one of the
    // grid functions is called
    gridWidth=n;
    gridHeight=1;
    numLEDs = n;

    // allocate memory for the array and make pixels point to the start of it
    pixels = (uint32_t *) calloc(numLEDs, sizeof(uint32_t));
    clear();

    bgpixels = (uint32_t *) calloc(numLEDs, sizeof(uint32_t));
    bgclear();
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

    // allocate memory for the array and make pixels point to the start of it
    pixels = (uint32_t *) calloc(numLEDs, sizeof(uint32_t));
    clear();

    bgpixels = (uint32_t *) calloc(numLEDs, sizeof(uint32_t));
    bgclear();
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
      // squirt the data out to the pixel string
      // method is the same as SPI.transfer() but saves the call
      //
      // if the foreground is empty (0) then display the background
      if(*(pixels+p) == 0)
      {
        //  *(pixels+p) = *(bgpixels+p);
        SPDR = *(bgpixels+p)>>16 & 0xff;
        while (!(SPSR & (1<<SPIF))) {};
        // send green pixel
        SPDR = *(bgpixels+p)>>8 & 0xff;
        while (!(SPSR & (1<<SPIF))) {};
        // send blue pixel
        SPDR = *(bgpixels+p) & 0xff;
        while (!(SPSR & (1<<SPIF))) {};
      } else {
        // send red pixel
        SPDR = *(pixels+p)>>16 & 0xff;
        while (!(SPSR & (1<<SPIF))) {};
        // send green pixel
        SPDR = *(pixels+p)>>8 & 0xff;
        while (!(SPSR & (1<<SPIF))) {};
        // send blue pixel
        SPDR = *(pixels+p) & 0xff;
        while (!(SPSR & (1<<SPIF))) {};
      }
    }

    // for my protocol analyzer - see above
    digitalWrite(SS,HIGH);
}

void WS2801::setPixelColor(uint16_t n, uint8_t r, uint8_t g, uint8_t b)
{
    uint32_t data;

    if (n > numLEDs) return;
    if (n < 0) return;

    data = r;
    data <<= 8;
    data |= g;
    data <<= 8;
    data |= b;

    *(pixels+n) = data & 0xFFFFFF;
}


void WS2801::setPixelColor(uint16_t n, uint32_t c)
{
    if (n > numLEDs) return;
    if (n < 0) return;

    // mask off lower 24 bits
    *(pixels+n) = c & 0xFFFFFF;
}

//---
// Translate x and y to a LED index number in an array.
//---
uint16_t WS2801::Translate(uint16_t x, uint16_t y)
{
    // stop user from sending values too small or large
    if(x<0 || x>gridWidth-1) { x=0; }
    if(y<0 || y>gridHeight-1) { y=0; }

    if(x%2)
    { return(((x+1) * gridHeight)- 1 - y); }
    else
    { return((x * gridHeight) + y); }
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
    for(int xx=startLED; xx<=endLED; xx++)
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
            setPixelColor(Translate(y,x), color);        //Fast set LEDs in grid.
        } else {
            setPixelColor(Translate(x,y), color);        //Fast set LEDs in grid.
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
// draw a box using the HVline drawing function (ph1x3r)
//=====
void WS2801::box(int x0, int y0, int x1, int y1, uint32_t color)
{
    HVLine(x0, y0, x1-x0, 0,color); // draw from start in right direction
    HVLine(x1, y0, y1-y0, 1,color); // now draw downwards
    HVLine(x1, y1, x1-x0, 2,color); // then back across to the left
    HVLine(x0, y1, y1-y0, 3,color); // back up to the start again

}

//=====
// draw a filled box using the HVline drawing function (ph1x3r)
//=====
void WS2801::boxFill(int x0, int y0, int x1, int y1, uint32_t color)
{
    // draw vertical lines the size of the box for the width
    // of the box
    for(int xx=x0; xx<=x1; xx++)
    {
      HVLine(xx, y0, y1-y0, 1, color); 
    }
}


//=====
// draw a triangle
//=====
void WS2801::triangle(int x0, int y0, int x1, int y1, int x2, int y2, uint32_t color)
{
  line(x0, y0, x1, y1, color);
  line(x1, y1, x2, y2, color);
  line(x2, y2, x0, y0, color);
}

//=====
// draw a circle
//=====
void WS2801::circle(uint16_t x0, uint16_t y0, uint16_t r, uint32_t color) {
  setPixelColor(Translate(x0, y0+r), color);
  setPixelColor(Translate(x0, y0-r), color);
  setPixelColor(Translate(x0+r, y0), color);
  setPixelColor(Translate(x0-r, y0), color);

  circleHelp(x0, y0, r, 0xF, color);
}

void WS2801::circleHelp(uint16_t x0, uint16_t y0, uint16_t r, uint16_t corner, uint32_t color) {
  int f = 1 - r;
  int ddF_x = 1;
  int ddF_y = -2 * r;
  int x = 0;
  int y = r;


  while (x<y) {
    if (f >= 0) {
      y--;
      ddF_y += 2;
      f += ddF_y;
    }
    x++;
    ddF_x += 2;
    f += ddF_x;
    if (corner & 0x4) {
      setPixelColor(Translate(x0 + x, y0 + y), color);
      setPixelColor(Translate(x0 + y, y0 + x), color);
    }
    if (corner & 0x2) {
      setPixelColor(Translate(x0 + x, y0 - y), color);
      setPixelColor(Translate(x0 + y, y0 - x), color);
    }
    if (corner & 0x8) {
      setPixelColor(Translate(x0 - y, y0 + x), color);
      setPixelColor(Translate(x0 - x, y0 + y), color);
    }
    if (corner & 0x1) {
      setPixelColor(Translate(x0 - y, y0 - x), color);
      setPixelColor(Translate(x0 - x, y0 - y), color);
    }
  }
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
            setPixelColor(constrain(zz-xx, 0, numLEDs-1),
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
    for(int xx=0; xx<numLEDs; xx++)
    {
      setPixelColor(xx, random(0, 0xffffff) );
    }
}


//=====
//Set all LEDS to a single color. (ph1x3r)
//=====
void WS2801::All( uint32_t color)
{
    setRange(0, numLEDs-1, color);
}


//=====
// Twinkle all lights - argument determines how many cycles (ph1x3r)
//=====
// twinkle achieved by setting random colors to all lights and then repeating
// xx number of times
//=====
void WS2801::Twinkle(int wait, int repeat)
{
    for(int xx=0; xx<repeat; xx++)
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
    for(xx=0; xx<gridHeight; xx++)
    {
        line(0, xx, gridWidth-1, xx, col );
        show();
        delay(del);
    }

    for(xx=0; xx<gridHeight; xx++)
    {
        line(0, xx, gridWidth-1, xx, 0 );
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
    for(xx=gridHeight-1; xx>=0; xx--)
    {
        line(0, xx, gridWidth-1, xx, col );
        show();
        delay(del);
    }

    for(xx=gridHeight-1; xx>=0; xx--)
    {
        line(0, xx, gridWidth-1, xx, 0 );
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
    for(xx=0; xx<gridWidth; xx++)
    {
        line(xx, 0, xx, gridHeight-1, col );
        show();
        delay(del);
    }

    for(xx=0; xx<gridWidth; xx++)
    {
        line(xx, 0, xx, gridHeight-1, 0 );
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
    for(xx=gridWidth-1; xx>=0; xx--)
    {
        line(xx, 0, xx, gridHeight-1, col );
        show();
        delay(del);
    }

    for(xx=gridWidth-1; xx>=0; xx--)
    {
        line(xx, 0, xx, gridHeight-1, 0 );
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
    for(int i=0; i< numLEDs; i++)
    {
        *(pixels+i) = 0;
    }
}

//=====
// functions for working with the background (ph1x3r)
//=====
void WS2801::bgclear()
{
    for(int i=0; i< numLEDs; i++)
    {
        *(bgpixels+i) = 0;
    }
}

void WS2801::bgAll(uint32_t col)
{
    for(int xx=0;xx<numLEDs;xx++)
    {
      *(bgpixels+xx) = col & 0xffffff;
    }
}
