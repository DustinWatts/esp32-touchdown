/**
  Title: ESP32 TouchDown Keypad
  Author: Dustin Watts
  Date: 01-01-2021

  A simple 4 by 3 matrix of buttons. To show you how to create a custom keypad. Each button can have it's
  own function. These buttons do nothing for now, except print to the serial monitor and make a little sound.

  This example is based on the examples that come with the TFT_eSPI library. Which includes the Adafruit GFX button handling class.

*/

#include <TFT_eSPI.h> // The TFT_eSPI library
#include <FT6236.h>   // FT6236 Touch Controller library

// Keypad start position, key sizes and spacing
#define KEY_W 95 // Key width
#define KEY_H 95 // Key height
#define KEY_SPACING_X 20 // X gap
#define KEY_SPACING_Y 10 // Y gap

#define KEY_X (KEY_W/2) + KEY_SPACING_X // X-axis centre of the first key
#define KEY_Y (KEY_H/2) + KEY_SPACING_Y // Y-axis centre of the first key

#define KEY_TEXTSIZE 3   // Font size multiplier

// Choose the fnt you are using
#define LABEL1_FONT &FreeSansOblique12pt7b // Key label font

// Create the touch object
FT6236 ts = FT6236();

// Create the screen object
TFT_eSPI tft = TFT_eSPI();

// Creating the labels for the buttons
// <name>[<number-of-lables>][<number-of-chars-per-label]
// The number of chars per label should include the termination \0.
char keyLabel[12][3] = {"1", "2", "3", "4", "5", "6", "7", "8", "9", "10", "11", "12"};

// Setting the colour for each button
// You can use the colours defined in TFT_eSPI.h
uint16_t keyColor[12] = {TFT_BLUE, TFT_RED, TFT_GREEN, TFT_NAVY, 
                         TFT_MAROON, TFT_MAGENTA, TFT_ORANGE, TFT_SKYBLUE, 
                         TFT_PURPLE, TFT_CYAN, TFT_PINK, TFT_DARKCYAN
                        };
                        
// Create 12 'keys' to use later
TFT_eSPI_Button key[12];

void setup() {
  
  Serial.begin(9600);
  Serial.println("");

  // Setup PWM channel for Piezo speaker
  ledcSetup(0, 500, 8);

  // Initialise the touchcontroller
  ts.begin(40);

  // Initialise the TFT screen
  tft.init();

  // Set the rotation before we calibrate
  tft.setRotation(1);

  // Clear the screen
  tft.fillScreen(TFT_BLACK);

  // Draw the keys ( 3 times 4 loops to create 12)
  for (uint8_t row = 0; row < 3; row++) {   // 3 rows
    for (uint8_t col = 0; col < 4; col++) { // of 4 buttons
      
      uint8_t b = col + row * 4; // The button number is the column we are on added to the row we are on 
                                 // multiplied by the number of buttons per row. C++ uses the Order of operations
                                 // you are used to so first the row is multiplied by 4 and then the col is added.
                                 // The first button is 0.
                                 // Example. col = 2 (third column), row = 1 (second row), becomes: 1 * 4 = 4 --> 4 + 2 = 6. This is the 7th button.

      key[b].initButton(&tft, KEY_X + col * (KEY_W + KEY_SPACING_X),
                        KEY_Y + row * (KEY_H + KEY_SPACING_Y), // x, y, w, h, outline, fill, text
                        KEY_W, KEY_H, TFT_WHITE, keyColor[b], TFT_WHITE,
                        keyLabel[b], KEY_TEXTSIZE);
      key[b].drawButton();
    }
  }

}

void loop() {

  uint16_t t_x = 0, t_y = 0; // To store the touch coordinates
  boolean pressed = false;   // Make sure we are not using last loop's touch
  
  if (ts.touched())
    {
      
      // Retrieve a point
      TS_Point p = ts.getPoint();

      //Flip things around so it matches our screen rotation     
      p.x = map(p.x, 40, 280, 320, 0); // I'm mapping the touch coordinates to start at 40 and end at 280 because I found this to
                                       // make the touches more accurete.
      t_y = p.x; // because we are using the screen horizontal the x-axis becomes the y-asis
      t_x = p.y; // and the y-axis becones the x-axis

      // Let the rest of the loop now we have a touch
      pressed = true;
    }

  // Check if any key coordinate boxes contain the touch coordinates
  for (uint8_t b = 0; b < 12; b++) {
    if (pressed && key[b].contains(t_x, t_y)) {
      key[b].press(true);  // tell the button it is pressed
    } else {
      key[b].press(false);  // tell the button it is NOT pressed
    }
  }

  // Check if any key has changed state
  for (uint8_t b = 0; b < 12; b++) {

    if (key[b].justReleased()) key[b].drawButton(); // draw normal again

    if (key[b].justPressed()) 
    {
      key[b].drawButton(true);  // draw invert (background becomes text colour and text becomes background colour
      buttonpress(b); // Call the button press function and pass the button number
    }
  }

}

void buttonpress(int button)
{
  // First do a little beep
  beep();
  //Handle a button press. Buttons are 0 indexed, meaning that the first button is button 0.
  switch(button){
    case 0:
     Serial.println("Button 1 pressed");
     // Doing nothing for now.
     break;
    case 1:
     Serial.println("Button 2 pressed");
     // Doing nothing for now
     break;
    case 2:
     Serial.println("Button 3 pressed");
     // Doing nothing for now
     break;
    case 3:
     Serial.println("Button 4 pressed");
     // Doing nothing for now
     break;
    case 4:
     Serial.println("Button 5 pressed");
     // Doing nothing for now
     break;
    case 5:
     Serial.println("Button 6 pressed");
     // Doing nothing for now
     break;
    case 6:
     Serial.println("Button 7 pressed");
     // Doing nothing for now
     break;
    case 7:
     Serial.println("Button 8 pressed");
     // Doing nothing for now
     break;
    case 8:
     Serial.println("Button 9 pressed");
     // Doing nothing for now
     break;
    case 9:
     Serial.println("Button 10 pressed");
     // Doing nothing for now
     break;
    case 10:
     Serial.println("Button 11 pressed");
     // Doing nothing for now
     break;
    case 11:
     Serial.println("Button 12 pressed");
     // Doing nothing for now
     break;
  }
}

void beep(){

      // Beep 
      ledcAttachPin(26, 0); // The piezo speaker is attached to GPIO26
      ledcWriteTone(0, 1000);
      delay(150);
      ledcDetachPin(26);
      ledcWrite(0, 0);

}
  
