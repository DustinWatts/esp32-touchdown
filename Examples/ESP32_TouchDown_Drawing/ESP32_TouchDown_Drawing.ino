/**
 * ESP32 TouchDown Example
 * date: 31-12-2020
 * 
 * This example show how to read touch data and draw something to the screen acoording to the coordinates.
 * 
 * Used libraries:
 * 
 * TFT_eSPI (by Bodmer)
 * FT6232 (by Dustin Watts)
 * 
 * It is important that in the TFT_eSPI library the right configuration is used. You can do this by modifying the User_Setup.h file
 * in the library folder.
 * 
 * User_Setup.h configuration:
 * 
 * #define ILI9488_DRIVER 
 * 
 * #define TFT_MISO 19
 * #define TFT_MOSI 23
 * #define TFT_SCLK 18
 * #define TFT_CS   15  // Chip select control pin
 * #define TFT_DC    2  // Data Command control pin
 * #define TFT_RST   4 
 * 
 */

#include <FT6236.h>        // Capacitive Touch Controller Library
#include <TFT_eSPI.h>      // The TFT_eSPI library

TFT_eSPI tft = TFT_eSPI();
FT6236 ts = FT6236();

uint8_t dotsize = 8;
bool displaystartscreen = true;

void setup() {

  Serial.begin(9600);

  // Initialise Touch
  ts.begin(40);
   
  
  // Initialise TFT
  tft.init();
  tft.setTextColor(TFT_WHITE,TFT_BLACK);
  tft.setTextSize(2);
  tft.setRotation(1);

  // Fill the screen with black (clearing the screen)
  tft.fillScreen(TFT_BLACK);

  // Draw an intial message
  tft.println("Start touching to start drawing.");

}

void loop() {

    if (ts.touched())
    {
      if(displaystartscreen)
      {
        tft.fillScreen(TFT_BLACK);
        displaystartscreen = false;
      }
      // Retrieve a point
      TS_Point p = ts.getPoint();

      // Print the coordinates to the serial monitor
      Serial.print("X: ");
      Serial.print(p.x);
      Serial.print(" Y: ");
      Serial.println(p.y);

      // You can use 'map' to flip things around so it matches the screen rotation. in this orientation, 
      // the X coordinate becomes the Y coordinate and vice versa.
      // You can also use `map` to make sure the the coordinates where you touch match the coordinates
      // that are being read from the touch controller. So that when you draw something to the screen it 
      // is more accurate.
      
      // This is for the horizontal orientation. I found that that the bottom left wasn't X = 0, but more like
      // x = 40. The top left wasn't x = 320 but more like X = 280.

      int y = map(p.x, 40, 280, 320, 0);
      int x = p.y;

      if(x < 480 && y < 320) // Make sure we are not trying to draw outside the screen.
      {
        // Clear the screen (or don't to keep drawing like a pen)
        //tft.fillScreen(TFT_BLACK);

        // Draw the circle
        tft.fillCircle(x, y, dotsize, TFT_BLUE);
      }
      
        
     }

}
