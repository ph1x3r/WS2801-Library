#include <WProgram.h>

class WS2801
{
    private:
// the arrays of int32s that hold each LED's 24 bit color values
        uint32_t *pixels;
        uint16_t numLEDs;
        uint8_t dataPin, clockPin;

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
