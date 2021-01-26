/**
  Title: ESP32 TouchDown HA
  Author: Dustin Watts
  Date: 01-01-2021

  A simple 4 by 3 matrix of buttons (called A through L) to show how you could control your Home Assistant by using their API.
  For this example you will need to have a Long Lives Access Token from HA. You will also need the IP address and port
  of your server. 

  This example uses HTTP. If your HA uses HTTPS check out the example called "ESP32 TouchDown HA Secure".

  HA expects a JSON object as payload. You can use ArduinoJson to create those objects, but in this exmaple I'm just creating the raw
  data myself. Make sure when you create the string to escape all double quotes! E.g. "{\"entity_id\":\"switch.simple_led_1\"}"

  These libraries need to be installed:

  - FT6236 (https://github.com/DustinWatts/FT6236)
  - TFT_eSPI (available through the Library Manager)

  This example is based on the examples that come with the TFT_eSPI library. Which includes the Adafruit GFX button handling class.

  ! Don't forget to configure User_Setup.h in the TFT_eSPI library. You can use User_setup.h in "/TFT_eSPI Setup Example".

*/

#include <TFT_eSPI.h> // The TFT_eSPI library
#include <WiFi.h> // Wifi support
#include <HTTPClient.h> 
#include <ESPmDNS.h> // mDNS functionality
#include <Wire.h>
#include <FT6236.h>

const char *ssid = " "; // <- SSID here
const char *password = " "; // <- Password here
const char *server = " "; // the IP address of your HA server here
uint16_t port = 8123; // the port of your HA server here

// Place your Long Lived Access Token from HA here:
const char *authtoken = " ";

// Keypad start position, key sizes and spacing
#define KEY_W 95 // Width and height
#define KEY_H 95
#define KEY_SPACING_X 20 // X and Y gap
#define KEY_SPACING_Y 10

#define KEY_X (KEY_W/2) + KEY_SPACING_X // Centre of key
#define KEY_Y (KEY_H/2) + KEY_SPACING_Y 

#define KEY_TEXTSIZE 3   // Font size multiplier

#define LABEL1_FONT &FreeSansOblique12pt7b // Key label font

FT6236 ts = FT6236();
TFT_eSPI tft = TFT_eSPI();

// Creating the labels for the buttons
char keyLabel[12][2] = {"A", "B", "C", "D", "E", "F", "G", "H", "I", "J", "K", "L"};

// Setting the colour for each button
uint16_t keyColor[12] = {TFT_BLUE, TFT_RED, TFT_GREEN, TFT_NAVY, 
                         TFT_MAROON, TFT_MAGENTA, TFT_ORANGE, TFT_SKYBLUE, 
                         TFT_PURPLE, TFT_CYAN, TFT_PINK, TFT_DARKCYAN
                        };

TFT_eSPI_Button key[12];
                        
void setup() {
  
  Serial.begin(9600);
  Serial.println("");

  Serial.printf("Connecting to %s", ssid);
    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED)
    {
      delay(500);
      Serial.print(".");
    }
    Serial.println("");
    Serial.print("Connected! IP address: ");
    Serial.println(WiFi.localIP());

  // Initialise the touchcontroller
  ts.begin(40);

  // Initialise the TFT screen
  tft.init();

  // Set the rotation before we calibrate
  tft.setRotation(1);

  // Clear the screen
  tft.fillScreen(TFT_BLACK);

  // Draw keypad
  drawKeypad();

}

void loop() {

  uint16_t t_x = 0, t_y = 0; // To store the touch coordinates
  boolean pressed = false;
  
  if (ts.touched())
    {
      
      // Retrieve a point
      TS_Point p = ts.getPoint();

      //Flip things around so it matches our screen rotation
      p.x = map(p.x, 40, 280, 320, 0);
      t_y = p.x;
      t_x = p.y;

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

    if (key[b].justReleased()) key[b].drawButton();     // draw normal

    if (key[b].justPressed()) 
    {
      key[b].drawButton(true);  // draw invert
      buttonpress(b);
    }
  }

}

void buttonpress(int button)
{
  //Handle a button press. Buttons are 0 indexed, meaning that the first button is button 0.
  switch(button){
    case 0:
     Serial.println("Button 1 pressed");
     // An example of switching a switch
     sendRestPost("/api/services/switch/toggle", "{\"entity_id\":\"switch.simple_led_1\"}");
     break;
    case 1:
     Serial.println("Button 2 pressed");
     // An example of switching a switch
     sendRestPost("/api/services/switch/toggle", "{\"entity_id\":\"switch.simple_led_2\"}");
     break;
    case 2:
     Serial.println("Button 3 pressed");
     // An example of turning on a scene
     sendRestPost("/api/services/scene/turn_on", "{\"entity_id\":\"scene.turn_off_both_leds\"}");
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

void sendRestPost(String endpoint, String payload)
{
      
      HTTPClient rest;

      // Just some information to the serial monitor
      Serial.print("Posting to ");
      Serial.print(server);
      Serial.print(" on port ");
      Serial.print(port);
      Serial.print(" to endpoint ");
      Serial.println(endpoint);

      rest.begin(server, port, endpoint);
      rest.addHeader("Content-Type", "application/json");
      String auth = "Bearer ";
      auth += authtoken;
      rest.addHeader("Authorization", auth);
      rest.addHeader("cache-control","no-cache");

      // Just some information to the serial monitor
      Serial.print("Posting: ");
      Serial.println(payload);
        
      int httpCode = rest.POST(payload);
  
      if (httpCode > 0) 
      {
        // Print the response to the serial monitor
        String response = rest.getString();
        Serial.print("Sent! Response: ");
        Serial.println(response);
       } 
       else 
       { 
          // Print the error to the serial monitor
          Serial.printf("Sent but POST failed, error: %s\n", rest.errorToString(httpCode).c_str());
          String response = rest.getString();
          Serial.println(String("Error:") + response);
       }
       
       rest.end();
}

void drawKeypad()
{
  // Draw the keys
  for (uint8_t row = 0; row < 3; row++) {
    for (uint8_t col = 0; col < 4; col++) {
      uint8_t b = col + row * 4;


      key[b].initButton(&tft, KEY_X + col * (KEY_W + KEY_SPACING_X),
                        KEY_Y + row * (KEY_H + KEY_SPACING_Y), // x, y, w, h, outline, fill, text
                        KEY_W, KEY_H, TFT_WHITE, keyColor[b], TFT_WHITE,
                        keyLabel[b], KEY_TEXTSIZE);
      key[b].drawButton();
    }
  }
}
