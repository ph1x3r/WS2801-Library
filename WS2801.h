#include <WProgram.h>

//=====
// ph1x3r - bitbang speed improvements in the show() function
// defines used for bit masks as they are faster than variables
// WARNING - hard coded for PORTD using pins 0 to 7
// default clock on pin 3 and data on pin 4
//=====
#define cPIN 3
#define dPIN 4
#define cpLOW B11110111
#define dpLOW B11101111
#define cpHIGH B1000
#define dpHIGH B10000

class WS2801
{
    private:
// the arrays of int32s that hold each LED's 24 bit color values
        uint32_t *pixels;
        uint16_t numLEDs;
        uint8_t dataPin, clockPin;

// ph1x3r - additional header defines
        uint16_t gridWidth, gridHeight;
        uint8_t cpinLOW, cpinHIGH, dpinLOW, dpinHIGH;

    public:
        WS2801(uint16_t n, uint8_t dpin, uint8_t cpin);
        void begin();
        void show();
        void setPixelColor(uint16_t n, uint8_t r, uint8_t g, uint8_t b);
        void setPixelColor(uint16_t n, uint32_t c);
        uint16_t numPixels(void);

// ph1x3r - additional header defines
        WS2801(uint16_t x, uint16_t y, uint8_t dpin, uint8_t cpin);
        uint16_t Translate(uint16_t x, uint16_t y);
        uint32_t Color(uint8_t r, uint8_t g, uint8_t b);
        void setRange(uint16_t startLED, uint16_t endLED, uint32_t color );

        void swap(int * a, int * b);
        void line(int x0,  int y0, int x1, int y1, uint32_t color);

        void box(int x0, int y0, int x1, int y1, uint32_t color);
        void comet(int wait, int red, int green, int blue, int dir);
        void AllRandom();
        void All( uint32_t color);
        void Twinkle(int wait, int xx);
        void SpinLine(int del, uint32_t col);
        void clear();

        void MoveLinesH(int del, uint32_t col);
        void MoveLinesRH(int del, uint32_t col);
        void MoveLinesV(int del, uint32_t col);
        void MoveLinesRV(int del, uint32_t col);

};
