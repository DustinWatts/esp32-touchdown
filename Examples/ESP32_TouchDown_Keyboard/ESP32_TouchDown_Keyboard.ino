/**
  Title: ESP32 TouchDown Keyboard
  Author: Dustin Watts
  Date: 15-01-2021

  A phone style keyboard that sends whatever is typed to the serial monitor after pressing "send". 

  This example is based on the examples that come with the TFT_eSPI library. Which includes the Adafruit GFX button handling class.

  ! Don't forget to configure User_Setup.h in the TFT_eSPI library. You can use User_setup.h in "/TFT_eSPI Setup Example".

*/

#include <TFT_eSPI.h> // The TFT_eSPI library
#include <FT6236.h>   // FT6236 Touch Controller library

// Keypad start position, key sizes and spacing
#define KEY_W 35 // Key width
#define KEY_H 35 // Key height
#define KEY_SPACING_X 10 // X gap
#define KEY_SPACING_Y 10 // Y gap

#define KEY_X 10 + (KEY_W/2) + KEY_SPACING_X // X-axis centre of the first key
#define KEY_Y 120 + (KEY_H/2) + KEY_SPACING_Y // Y-axis centre of the first key

#define KEY_TEXTSIZE 2   // Font size multiplier

// Choose the fnt you are using
#define LABEL1_FONT &FreeSansOblique12pt7b // Key label font

// Create the touch object
FT6236 ts = FT6236();

// Create the screen object
TFT_eSPI tft = TFT_eSPI();

// Creating the labels for the buttons
// <name>[<number-of-lables>][<number-of-chars-per-label]
// The number of chars per label should include the termination \0.
char keyLabel[26][2] = {"q", "w", "e", "r", "t", "y", "u", "i", "o", "p", "a", "s", "d", "f", "g", "h", "j", "k", "l", "z", "x", "c", "v", "b", "n", "m"};
char upperKeyLabel[26][2] = {"Q", "W", "E","R","T","Y","U","I","O","P","A","S","D","F","G","H","J","K","L","Z","X","C","V","B","N","M"};
char numLabel[26][2] = {"1", "2", "3","4","5","6","7","8","9","0","-","/",":",";","(",")","$","&","@","\"",".",",","?","!","'","~"};

boolean upper = true; // we start with using upper case
boolean numeric = false;
boolean alwaysupper = false;

unsigned long previousMillis = 0;

uint16_t keycolor = TFT_DARKGREY;
uint16_t specialKeyColor = tft.color565(90, 90, 90);
uint16_t keyboardBackground = tft.color565(20, 20, 20);

String textbuffer = "";
                        
// Create 'keys' to use later
TFT_eSPI_Button key[32];

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
  
  drawUpperKeyboard();
    
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
  for (uint8_t b = 0; b < 32; b++) {
    if (pressed && key[b].contains(t_x, t_y)) {
      key[b].press(true);  // tell the button it is pressed
    } else {
      key[b].press(false);  // tell the button it is NOT pressed
    }
  }

  // Check if any key has changed state
  for (uint8_t b = 0; b < 32; b++) {

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
  tft.setTextColor(TFT_WHITE, TFT_BLACK);
  tft.setCursor(5, 5);
  // Uncomment next line if you want a beep everytime you press a key
  // beep();
  if(button < 26){
      if(!upper){
        if(!numeric){
          textbuffer += keyLabel[button];
          tft.print(textbuffer);
        }else{
          textbuffer += numLabel[button];
          tft.print(textbuffer);
          if(button == 20){
            drawUpperKeyboard();
            upper = true;           
          }
        }     
      }else{
        if(!numeric){
          textbuffer += upperKeyLabel[button];
          tft.print(textbuffer);
          if(!alwaysupper){
           drawKeyboard();
           upper = false; 
          }
        }else{
          textbuffer += numLabel[button];
          tft.print(textbuffer);
          if(button == 20){
            drawUpperKeyboard();
            upper = true;           
          }         
        }        
    }    
  }

  if(button == 30){

      // If we encounter two space in a row. Make it a . and a space.
      String lastcharacter = String(textbuffer.charAt(textbuffer.length()-1));
      if(lastcharacter == " "){

        //clear the line first
      tft.setCursor(5, 5);
      for(int i=0; i < textbuffer.length(); i++)
      {
       tft.print(" ");
      }
      tft.setCursor(5, 5);
      textbuffer.remove(textbuffer.length() - 1,1);
      
      textbuffer += ". ";

      tft.print(textbuffer);
      
      // After that we draw an upper case keyboard
      drawUpperKeyboard();
        upper = true;
        numeric = false;
        
      }else{
        textbuffer += " ";
        tft.print(textbuffer);
      } 
  }

  if(button == 27){

  // If there are two pressed on the shift key in rapid succesion, we make it CAPS
  if(millis() < previousMillis + 400){

    if(upper){

    // Draw a CAPS button
    key[27].initButton(&tft, KEY_X + (5 + KEY_SPACING_X),
                        KEY_Y + 2 * (KEY_H + KEY_SPACING_Y), // x, y, w, h, outline, fill, text
                        52, KEY_H, TFT_WHITE, TFT_WHITE, keycolor,
                        "^=", KEY_TEXTSIZE);
    key[27].drawButton();
      
    previousMillis = 0;
    alwaysupper = true;
    }
    
  } else {
    
    if(!upper){
      // draw upper case keyboard
      drawUpperKeyboard();
      upper = true;
      numeric = false;
    }else{
      // draw lower case keyboard
      drawKeyboard();
      upper = false;
      numeric = false;
    }

  }

  previousMillis = millis();
  
  }

  if(button == 28){

      //clear the line first
      tft.setCursor(5, 5);
      for(int i=0; i < textbuffer.length(); i++)
      {
       tft.print(" ");
      }
      tft.setCursor(5, 5);
      textbuffer.remove(textbuffer.length() - 1,1);
      tft.print(textbuffer);
    
  }

  if(button == 29){

      if(!numeric){
        // draw numeric keyboard
        drawNumKeyboard();
        upper = false;
        alwaysupper = false;
        numeric = true;
      }else{
        // draw lower case keyboard
        drawKeyboard();
        alwaysupper = false;
        numeric = false;
        upper = false;
      }
    
  }

  if(button == 31){

      //send buffer to serial monitor and clear buffer
      tft.setCursor(5, 5);
      for(int i=0; i < textbuffer.length(); i++)
      {
       tft.print(" ");
      }
      tft.setCursor(5, 5);
      Serial.println(textbuffer);
      textbuffer = "";
    
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

void drawKeyboard(){

  // Keyboard background
  tft.fillRect(0, 117, 480, 203, keyboardBackground);

  int b = 0;

  // Draw all the letters
  
  for (uint8_t row = 0; row < 3; row++) {   // 3 rows
    if(row == 0){

      for (uint8_t col = 0; col < 10; col++) { // of 4 buttons
      
      key[b].initButton(&tft, KEY_X + col * (KEY_W + KEY_SPACING_X),
                        KEY_Y + row * (KEY_H + KEY_SPACING_Y), // x, y, w, h, outline, fill, text
                        KEY_W, KEY_H, TFT_WHITE, keycolor, TFT_WHITE,
                        keyLabel[b], KEY_TEXTSIZE);
      key[b].drawButton();
      b++;
      }    
    }

    if(row == 1){

      for (uint8_t col = 0; col < 9; col++) { // of 4 buttons
      
      key[b].initButton(&tft, 22 + KEY_X + col * (KEY_W + KEY_SPACING_X),
                        KEY_Y + row * (KEY_H + KEY_SPACING_Y), // x, y, w, h, outline, fill, text
                        KEY_W, KEY_H, TFT_WHITE, keycolor, TFT_WHITE,
                        keyLabel[b], KEY_TEXTSIZE);
      key[b].drawButton();
      b++;
      }    
    }

    if(row == 2){

      for (uint8_t col = 0; col < 7; col++) { // of 4 buttons
      
      key[b].initButton(&tft, 67 + KEY_X + col * (KEY_W + KEY_SPACING_X),
                        KEY_Y + row * (KEY_H + KEY_SPACING_Y), // x, y, w, h, outline, fill, text
                        KEY_W, KEY_H, TFT_WHITE, keycolor, TFT_WHITE,
                        keyLabel[b], KEY_TEXTSIZE);
      key[b].drawButton();
      b++;
      }    
    }
    
  }

  // Draw shift + backspace
      
      key[27].initButton(&tft, KEY_X + (5 + KEY_SPACING_X),
                        KEY_Y + 2 * (KEY_H + KEY_SPACING_Y), // x, y, w, h, outline, fill, text
                        52, KEY_H, TFT_WHITE, specialKeyColor, TFT_WHITE,
                        "^", KEY_TEXTSIZE);
      key[27].drawButton();

      key[28].initButton(&tft, 375 + KEY_X + (5 + KEY_SPACING_X),
                        KEY_Y + 2 * (KEY_H + KEY_SPACING_Y), // x, y, w, h, outline, fill, text
                        52, KEY_H, TFT_WHITE, specialKeyColor, TFT_WHITE,
                        "<-", KEY_TEXTSIZE);
      key[28].drawButton();

  // Draw num + space + return
      
      key[29].initButton(&tft, 15 + KEY_X + (5 + KEY_SPACING_X),
                        KEY_Y + 3 * (KEY_H + KEY_SPACING_Y), // x, y, w, h, outline, fill, text
                        80, KEY_H, TFT_WHITE, specialKeyColor, TFT_WHITE,
                        "123", KEY_TEXTSIZE);
      key[29].drawButton();

      key[30].initButton(&tft, 187 + KEY_X + (5 + KEY_SPACING_X),
                        KEY_Y + 3 * (KEY_H + KEY_SPACING_Y), // x, y, w, h, outline, fill, text
                        245, KEY_H, TFT_WHITE, keycolor, TFT_WHITE,
                        "space", KEY_TEXTSIZE);
      key[30].drawButton();

      key[31].initButton(&tft, 360 + KEY_X + (5 + KEY_SPACING_X),
                        KEY_Y + 3 * (KEY_H + KEY_SPACING_Y), // x, y, w, h, outline, fill, text
                        80, KEY_H, TFT_WHITE, specialKeyColor, TFT_WHITE,
                        "Send", KEY_TEXTSIZE);
      key[31].drawButton();

    // Line above the keyboard
    tft.drawLine(0, 116, 480, 116, TFT_WHITE);
}

void drawUpperKeyboard(){

  // Keyboard background
  tft.fillRect(0, 117, 480, 203, keyboardBackground);

  int b = 0;

  // Draw all the upper-case letters
  
  for (uint8_t row = 0; row < 3; row++) {   // 3 rows
    if(row == 0){

      for (uint8_t col = 0; col < 10; col++) { // of 4 buttons
      
      key[b].initButton(&tft, KEY_X + col * (KEY_W + KEY_SPACING_X),
                        KEY_Y + row * (KEY_H + KEY_SPACING_Y), // x, y, w, h, outline, fill, text
                        KEY_W, KEY_H, TFT_WHITE, keycolor, TFT_WHITE,
                        upperKeyLabel[b], KEY_TEXTSIZE);
      key[b].drawButton();
      b++;
      }    
    }

    if(row == 1){

      for (uint8_t col = 0; col < 9; col++) { // of 4 buttons
      
      key[b].initButton(&tft, 22 + KEY_X + col * (KEY_W + KEY_SPACING_X),
                        KEY_Y + row * (KEY_H + KEY_SPACING_Y), // x, y, w, h, outline, fill, text
                        KEY_W, KEY_H, TFT_WHITE, keycolor, TFT_WHITE,
                        upperKeyLabel[b], KEY_TEXTSIZE);
      key[b].drawButton();
      b++;
      }    
    }

    if(row == 2){

      for (uint8_t col = 0; col < 7; col++) { // of 4 buttons
      
      key[b].initButton(&tft, 67 + KEY_X + col * (KEY_W + KEY_SPACING_X),
                        KEY_Y + row * (KEY_H + KEY_SPACING_Y), // x, y, w, h, outline, fill, text
                        KEY_W, KEY_H, TFT_WHITE, keycolor, TFT_WHITE,
                        upperKeyLabel[b], KEY_TEXTSIZE);
      key[b].drawButton();
      b++;
      }    
    }
    
  }

  // Draw shift + backspace
      
      key[27].initButton(&tft, KEY_X + (5 + KEY_SPACING_X),
                        KEY_Y + 2 * (KEY_H + KEY_SPACING_Y), // x, y, w, h, outline, fill, text
                        52, KEY_H, TFT_WHITE, TFT_WHITE, keycolor, // reverse colors
                        "^", KEY_TEXTSIZE);
      key[27].drawButton();

      key[28].initButton(&tft, 375 + KEY_X + (5 + KEY_SPACING_X),
                        KEY_Y + 2 * (KEY_H + KEY_SPACING_Y), // x, y, w, h, outline, fill, text
                        52, KEY_H, TFT_WHITE, specialKeyColor, TFT_WHITE,
                        "<-", KEY_TEXTSIZE);
      key[28].drawButton();

  // Draw num + space + return
      
      key[29].initButton(&tft, 15 + KEY_X + (5 + KEY_SPACING_X),
                        KEY_Y + 3 * (KEY_H + KEY_SPACING_Y), // x, y, w, h, outline, fill, text
                        80, KEY_H, TFT_WHITE, specialKeyColor, TFT_WHITE,
                        "123", KEY_TEXTSIZE);
      key[29].drawButton();

      key[30].initButton(&tft, 187 + KEY_X + (5 + KEY_SPACING_X),
                        KEY_Y + 3 * (KEY_H + KEY_SPACING_Y), // x, y, w, h, outline, fill, text
                        245, KEY_H, TFT_WHITE, keycolor, TFT_WHITE,
                        "space", KEY_TEXTSIZE);
      key[30].drawButton();

      key[31].initButton(&tft, 360 + KEY_X + (5 + KEY_SPACING_X),
                        KEY_Y + 3 * (KEY_H + KEY_SPACING_Y), // x, y, w, h, outline, fill, text
                        80, KEY_H, TFT_WHITE, specialKeyColor, TFT_WHITE,
                        "Send", KEY_TEXTSIZE);
      key[31].drawButton();

    // Line above the keyboard
    tft.drawLine(0, 116, 480, 116, TFT_WHITE);
}

void drawNumKeyboard(){

  // Keyboard background
  tft.fillRect(0, 117, 480, 203, keyboardBackground);

  int b = 0;

  // Draw all the numbers
  
  for (uint8_t row = 0; row < 3; row++) {   // 3 rows
    if(row == 0){

      for (uint8_t col = 0; col < 10; col++) { // of 4 buttons
      
      key[b].initButton(&tft, KEY_X + col * (KEY_W + KEY_SPACING_X),
                        KEY_Y + row * (KEY_H + KEY_SPACING_Y), // x, y, w, h, outline, fill, text
                        KEY_W, KEY_H, TFT_WHITE, keycolor, TFT_WHITE,
                        numLabel[b], KEY_TEXTSIZE);
      key[b].drawButton();
      b++;
      }    
    }

    if(row == 1){

      for (uint8_t col = 0; col < 9; col++) { // of 4 buttons
      
      key[b].initButton(&tft, 22 + KEY_X + col * (KEY_W + KEY_SPACING_X),
                        KEY_Y + row * (KEY_H + KEY_SPACING_Y), // x, y, w, h, outline, fill, text
                        KEY_W, KEY_H, TFT_WHITE, keycolor, TFT_WHITE,
                        numLabel[b], KEY_TEXTSIZE);
      key[b].drawButton();
      b++;
      }    
    }

    if(row == 2){

      for (uint8_t col = 0; col < 7; col++) { // of 4 buttons
      
      key[b].initButton(&tft, 67 + KEY_X + col * (KEY_W + KEY_SPACING_X),
                        KEY_Y + row * (KEY_H + KEY_SPACING_Y), // x, y, w, h, outline, fill, text
                        KEY_W, KEY_H, TFT_WHITE, keycolor, TFT_WHITE,
                        numLabel[b], KEY_TEXTSIZE);
      key[b].drawButton();
      b++;
      }    
    }
    
  }

  // Draw shift + backspace
      
      key[27].initButton(&tft, KEY_X + (5 + KEY_SPACING_X),
                        KEY_Y + 2 * (KEY_H + KEY_SPACING_Y), // x, y, w, h, outline, fill, text
                        52, KEY_H, TFT_WHITE, specialKeyColor, TFT_WHITE, // reverse colors
                        "^", KEY_TEXTSIZE);
      key[27].drawButton();

      key[28].initButton(&tft, 375 + KEY_X + (5 + KEY_SPACING_X),
                        KEY_Y + 2 * (KEY_H + KEY_SPACING_Y), // x, y, w, h, outline, fill, text
                        52, KEY_H, TFT_WHITE, specialKeyColor, TFT_WHITE,
                        "<-", KEY_TEXTSIZE);
      key[28].drawButton();

  // Draw num + space + return
      
      key[29].initButton(&tft, 15 + KEY_X + (5 + KEY_SPACING_X),
                        KEY_Y + 3 * (KEY_H + KEY_SPACING_Y), // x, y, w, h, outline, fill, text
                        80, KEY_H, TFT_WHITE, TFT_WHITE, specialKeyColor,
                        "ABC", KEY_TEXTSIZE);
      key[29].drawButton();

      key[30].initButton(&tft, 187 + KEY_X + (5 + KEY_SPACING_X),
                        KEY_Y + 3 * (KEY_H + KEY_SPACING_Y), // x, y, w, h, outline, fill, text
                        245, KEY_H, TFT_WHITE, keycolor, TFT_WHITE,
                        "space", KEY_TEXTSIZE);
      key[30].drawButton();

      key[31].initButton(&tft, 360 + KEY_X + (5 + KEY_SPACING_X),
                        KEY_Y + 3 * (KEY_H + KEY_SPACING_Y), // x, y, w, h, outline, fill, text
                        80, KEY_H, TFT_WHITE, specialKeyColor, TFT_WHITE,
                        "Send", KEY_TEXTSIZE);
      key[31].drawButton();

    // Line above the keyboard
    tft.drawLine(0, 116, 480, 116, TFT_WHITE);
}
