
#include <Arduino.h>

#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
#define OLED_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin)

#define NUMFLAKES     10 // Number of snowflakes in the animation example

#define LOGO_HEIGHT   16
#define LOGO_WIDTH    16
static const unsigned char PROGMEM logo_bmp[] =
{ B00000000, B11000000,
  B00000001, B11000000,
  B00000001, B11000000,
  B00000011, B11100000,
  B11110011, B11100000,
  B11111110, B11111000,
  B01111110, B11111111,
  B00110011, B10011111,
  B00011111, B11111100,
  B00001101, B01110000,
  B00011011, B10100000,
  B00111111, B11100000,
  B00111111, B11110000,
  B01111100, B11110000,
  B01110000, B01110000,
  B00000000, B00110000 };

#define XPOS   0 // Indexes into the 'icons' array in function below
#define YPOS   1
#define DELTAY 2



void testDisplay(Adafruit_SSD1306 display);

void testdrawline(Adafruit_SSD1306 display);
void testdrawrect(Adafruit_SSD1306 display);
void testfillrect(Adafruit_SSD1306 display);
void testdrawcircle(Adafruit_SSD1306 display);
void testfillcircle(Adafruit_SSD1306 display);
void testdrawroundrect(Adafruit_SSD1306 display);
void testfillroundrect(Adafruit_SSD1306 display);
void testdrawtriangle(Adafruit_SSD1306 display);
void testfilltriangle(Adafruit_SSD1306 display);
void testdrawchar(Adafruit_SSD1306 display);
void testdrawstyles(Adafruit_SSD1306 display);
void testscrolltext(Adafruit_SSD1306 display);
void testdrawbitmap(Adafruit_SSD1306 display);
void testanimate(Adafruit_SSD1306 display,const uint8_t *bitmap, uint8_t w, uint8_t h);