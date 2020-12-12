/* ESP32 TouchDown Welcome
 * by: Dustin Watts
 * date: 12-12-2020
 * 
 * This sketch comes pre-loaded with every new ESP32 TouchDown. In the SPIFFS there is one file: congrats.bmp
 * 
 */

#include <FT6236.h>       // Capacitive Touch Controller Library
#include <pgmspace.h>     // PROGMEM support header
#include "FS.h"           // File System header
#include <TFT_eSPI.h>     // The TFT_eSPI library

TFT_eSPI tft = TFT_eSPI();

byte red = 31;
byte green = 0;
byte blue = 0;
byte state = 0;
unsigned int colour = red << 11; // Colour order is RGB 5+6+5 bits each

FT6236 ts = FT6236();

void setup() {
  
  // Use serial port
  Serial.begin(9600);
  Serial.println("");

  if (!SPIFFS.begin())
    {
      Serial.println("SPIFFS initialisation failed!");
      tft.println("SPIFFS initialisation failed!");
      while (1)
        yield(); // We stop here
    }
    Serial.println("SPIFFS initialised.");

  // Initialise TFT
  tft.init();
  tft.setTextColor(TFT_WHITE,TFT_BLACK);
  tft.setTextSize(2);

  // Set the rotation before we calibrate
  tft.setRotation(1);

  // Clear the screen
  tft.fillScreen(TFT_BLACK);

    if (!ts.begin(40)) { 
      Serial.println("Unable to start the capacitive touchscreen!");
    } 
    else { 
      Serial.println("Capacitive touch started!");
      Serial.println("Touch the screen to see some pretty colours");
    }

  drawBmpSPIFFS("/congrats.bmp", 0, 0);
    
  // Setup PWM channel for Piezo speaker
  ledcSetup(0, 500, 8);
  
}

void loop() {

  if (ts.touched()){
      
      // Beep Beep  
      ledcAttachPin(26, 0);
      ledcWriteTone(0, 600);
      delay(150);
      ledcDetachPin(26);
      ledcWrite(0, 0);
    
      ledcAttachPin(26, 0);
      ledcWriteTone(0, 800);
      delay(150);
      ledcDetachPin(26);
      ledcWrite(0, 0);
          
      tft.fillScreen(TFT_BLACK);
      
      for(int i=0; i < 80; i++)
      {
        rainbow_fill();
      }

      tft.fillScreen(TFT_BLACK);
      tft.setCursor (0, 0);
      tft.println("ESP32 TouchDown ready for use!");
      tft.println("Or touch again for more pretty colours.");       
  }
  
}

// Fill screen with a rainbow pattern
void rainbow_fill()
{
  // The colours and state are not initialised so the start colour changes each time the funtion is called
  
  for (int i = 479; i > 0; i--) {
    // Draw a vertical line 1 pixel wide in the selected colour
    tft.drawFastHLine(0, i, tft.width(), colour); // in this example tft.width() returns the pixel width of the display
    // This is a "state machine" that ramps up/down the colour brightnesses in sequence
    switch (state) {
      case 0:
        green ++;
        if (green == 64) {
          green = 63;
          state = 1;
        }
        break;
      case 1:
        red--;
        if (red == 255) {
          red = 0;
          state = 2;
        }
        break;
      case 2:
        blue ++;
        if (blue == 32) {
          blue = 31;
          state = 3;
        }
        break;
      case 3:
        green --;
        if (green == 255) {
          green = 0;
          state = 4;
        }
        break;
      case 4:
        red ++;
        if (red == 32) {
          red = 31;
          state = 5;
        }
        break;
      case 5:
        blue --;
        if (blue == 255) {
          blue = 0;
          state = 0;
        }
        break;
    }
    colour = red << 11 | green << 5 | blue;
  }
}

void drawBmpSPIFFS(const char *filename, int16_t x, int16_t y)
{

  if ((x >= tft.width()) || (y >= tft.height()))
    return;

  fs::File bmpFS;

  bmpFS = SPIFFS.open(filename, "r");

  if (!bmpFS)
  {
    Serial.print("File not found");
    return;
  }

  uint32_t seekOffset;
  uint16_t w, h, row, col;
  uint8_t r, g, b;

  uint32_t startTime = millis();

  if (read16(bmpFS) == 0x4D42)
  {
    read32(bmpFS);
    read32(bmpFS);
    seekOffset = read32(bmpFS);
    read32(bmpFS);
    w = read32(bmpFS);
    h = read32(bmpFS);

    if ((read16(bmpFS) == 1) && (read16(bmpFS) == 24) && (read32(bmpFS) == 0))
    {
      y += h - 1;

      bool oldSwapBytes = tft.getSwapBytes();
      tft.setSwapBytes(true);
      bmpFS.seek(seekOffset);

      uint16_t padding = (4 - ((w * 3) & 3)) & 3;
      uint8_t lineBuffer[w * 3 + padding];

      for (row = 0; row < h; row++)
      {

        bmpFS.read(lineBuffer, sizeof(lineBuffer));
        uint8_t *bptr = lineBuffer;
        uint16_t *tptr = (uint16_t *)lineBuffer;
        // Convert 24 to 16 bit colours
        for (uint16_t col = 0; col < w; col++)
        {
          b = *bptr++;
          g = *bptr++;
          r = *bptr++;
          *tptr++ = ((r & 0xF8) << 8) | ((g & 0xFC) << 3) | (b >> 3);
        }

        // Push the pixel row to screen, pushImage will crop the line if needed
        // y is decremented as the BMP image is drawn bottom up
        tft.pushImage(x, y--, w, 1, (uint16_t *)lineBuffer);
      }
      tft.setSwapBytes(oldSwapBytes);
    }
    else
      Serial.println("[WARNING]: BMP format not recognized.");
  }
  bmpFS.close();
}

uint16_t read16(fs::File &f)
{
  uint16_t result;
  ((uint8_t *)&result)[0] = f.read(); // LSB
  ((uint8_t *)&result)[1] = f.read(); // MSB
  return result;
}

uint32_t read32(fs::File &f)
{
  uint32_t result;
  ((uint8_t *)&result)[0] = f.read(); // LSB
  ((uint8_t *)&result)[1] = f.read();
  ((uint8_t *)&result)[2] = f.read();
  ((uint8_t *)&result)[3] = f.read(); // MSB
  return result;
}
