#include <WProgram.h>
#include <SPI.h>

class WS2801
{
    private:
// the arrays of int32s that hold each LED's 24 bit color values
        uint32_t *pixels;
        uint32_t *bgpixels;
        uint16_t numLEDs;

// ph1x3r - additional header defines
        uint16_t gridWidth, gridHeight;

    public:
        WS2801(uint16_t n);
        void begin();
        void show();
        void setPixelColor(uint16_t n, uint8_t r, uint8_t g, uint8_t b);
        void setPixelColor(uint16_t n, uint32_t c);
        uint16_t numPixels(void);
// ph1x3r - additional header defines
        WS2801(uint16_t x, uint16_t y);
        uint16_t Translate(uint16_t x, uint16_t y);
        uint32_t Color(uint8_t r, uint8_t g, uint8_t b);
        void setRange(uint16_t startLED, uint16_t endLED, uint32_t color );

        void line(int x0,  int y0, int x1, int y1, uint32_t color);
        void HVLine(int x, int y, int len, int dir, uint32_t color);

        void box(int x0, int y0, int x1, int y1, uint32_t color);
        void boxFill(int x0, int y0, int x1, int y1, uint32_t color);
        void triangle(int x0, int y0, int x1, int y1, int x2, int y2, uint32_t color);

        void circle(uint16_t x0, uint16_t y0, uint16_t r, uint32_t color) ;
        void circleHelp(uint16_t x0, uint16_t y0, uint16_t r, uint16_t corn, uint32_t color);

        void comet(int wait, int red, int green, int blue, int dir);
        void AllRandom();
        void All( uint32_t color);
        void Twinkle(int wait, int xx);
        void SpinLine(int del, uint32_t col);
        void clear();
        void bgclear();
        void bgAll( uint32_t color);

        void MoveLinesH(int del, uint32_t col);
        void MoveLinesRH(int del, uint32_t col);
        void MoveLinesV(int del, uint32_t col);
        void MoveLinesRV(int del, uint32_t col);

};
