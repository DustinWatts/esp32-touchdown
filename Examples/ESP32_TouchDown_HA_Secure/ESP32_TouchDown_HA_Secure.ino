/**
  Title: ESP32 TouchDown HA Secure
  Author: Dustin Watts
  Date: 01-01-2021

  A simple 4 by 3 matrix of buttons (called A through L) to show how you could control your Home Assistant by using their API.
  For this example you will need to have a Long Lives Access Token from HA. You will also need the IP address and port
  of your server. 

  This example uses a secure connection to an HA server. You will need the root certificate of the server. You'll need to convert this 
  to a multiline string. Basically, take the raw text of the certificate and add `"` at the beginning of each line and add `\n"`
  at the end of each line.

  HA expects a JSON object as payload. You can use ArduinoJson to create those objects, but in this exmaple I'm just creating the raw
  data myself. Make sure when you create the string to escape all double quotes! E.g. "{\"entity_id\":\"switch.simple_led_1\"}"

  This example is based on the examples that come with the TFT_eSPI library. Which includes the Adafruit GFX button handeling class.

*/

#include <TFT_eSPI.h> // The TFT_eSPI library
#include <WiFi.h> // Wifi support
#include <WiFiClientSecure.h>
#include <HTTPClient.h> 
#include <ESPmDNS.h> // mDNS functionality
#include <Wire.h>
#include <FT6236.h>

const char *ssid = " "; // <- SSID here
const char *password = " "; // <- Password here
const char *server = " "; // the IP address of your HA server here
uint16_t port = 443; // the port of your HA server here

// Place your Long Lived Access Token from HA here:
const char *authtoken = " ";

// Root Certificate of the server

const char* rootCACertificate = \
"-----BEGIN CERTIFICATE-----\n"
"MIIFFjCCAv6gAwIBAgIRAJErCErPDBinU/bWLiWnX1owDQYJKoZIhvcNAQELBQAw\n"
"TzELMAkGA1UEBhMCVVMxKTAnBgNVBAoTIEludGVybmV0IFNlY3VyaXR5IFJlc2Vh\n"
"cmNoIEdyb3VwMRUwEwYDVQQDEwxJU1JHIFJvb3QgWDEwHhcNMjAwOTA0MDAwMDAw\n"
"WhcNMjUwOTE1MTYwMDAwWjAyMQswCQYDVQQGEwJVUzEWMBQGA1UEChMNTGV0J3Mg\n"
"RW5jcnlwdDELMAkGA1UEAxMCUjMwggEiMA0GCSqGSIb3DQEBAQUAA4IBDwAwggEK\n"
"AoIBAQC7AhUozPaglNMPEuyNVZLD+ILxmaZ6QoinXSaqtSu5xUyxr45r+XXIo9cP\n"
"R5QUVTVXjJ6oojkZ9YI8QqlObvU7wy7bjcCwXPNZOOftz2nwWgsbvsCUJCWH+jdx\n"
"sxPnHKzhm+/b5DtFUkWWqcFTzjTIUu61ru2P3mBw4qVUq7ZtDpelQDRrK9O8Zutm\n"
"NHz6a4uPVymZ+DAXXbpyb/uBxa3Shlg9F8fnCbvxK/eG3MHacV3URuPMrSXBiLxg\n"
"Z3Vms/EY96Jc5lP/Ooi2R6X/ExjqmAl3P51T+c8B5fWmcBcUr2Ok/5mzk53cU6cG\n"
"/kiFHaFpriV1uxPMUgP17VGhi9sVAgMBAAGjggEIMIIBBDAOBgNVHQ8BAf8EBAMC\n"
"AYYwHQYDVR0lBBYwFAYIKwYBBQUHAwIGCCsGAQUFBwMBMBIGA1UdEwEB/wQIMAYB\n"
"Af8CAQAwHQYDVR0OBBYEFBQusxe3WFbLrlAJQOYfr52LFMLGMB8GA1UdIwQYMBaA\n"
"FHm0WeZ7tuXkAXOACIjIGlj26ZtuMDIGCCsGAQUFBwEBBCYwJDAiBggrBgEFBQcw\n"
"AoYWaHR0cDovL3gxLmkubGVuY3Iub3JnLzAnBgNVHR8EIDAeMBygGqAYhhZodHRw\n"
"Oi8veDEuYy5sZW5jci5vcmcvMCIGA1UdIAQbMBkwCAYGZ4EMAQIBMA0GCysGAQQB\n"
"gt8TAQEBMA0GCSqGSIb3DQEBCwUAA4ICAQCFyk5HPqP3hUSFvNVneLKYY611TR6W\n"
"PTNlclQtgaDqw+34IL9fzLdwALduO/ZelN7kIJ+m74uyA+eitRY8kc607TkC53wl\n"
"ikfmZW4/RvTZ8M6UK+5UzhK8jCdLuMGYL6KvzXGRSgi3yLgjewQtCPkIVz6D2QQz\n"
"CkcheAmCJ8MqyJu5zlzyZMjAvnnAT45tRAxekrsu94sQ4egdRCnbWSDtY7kh+BIm\n"
"lJNXoB1lBMEKIq4QDUOXoRgffuDghje1WrG9ML+Hbisq/yFOGwXD9RiX8F6sw6W4\n"
"avAuvDszue5L3sz85K+EC4Y/wFVDNvZo4TYXao6Z0f+lQKc0t8DQYzk1OXVu8rp2\n"
"yJMC6alLbBfODALZvYH7n7do1AZls4I9d1P4jnkDrQoxB3UqQ9hVl3LEKQ73xF1O\n"
"yK5GhDDX8oVfGKF5u+decIsH4YaTw7mP3GFxJSqv3+0lUFJoi5Lc5da149p90Ids\n"
"hCExroL1+7mryIkXPeFM5TgO9r0rvZaBFOvV2z0gp35Z0+L4WPlbuEjN/lxPFin+\n"
"HlUjr8gRsI3qfJOQFy/9rKIJR0Y/8Omwt/8oTWgy1mdeHmmjk7j1nYsvC9JSQ6Zv\n"
"MldlTTKB3zhThV1+XWYp6rjd5JW1zbVWEkLNxE7GJThEUG3szgBVGP7pSWTUTsqX\n"
"nLRbwHOoq7hHwg==\n"
"-----END CERTIFICATE-----";

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
     sendSecureRestPost("/api/services/switch/toggle", "{\"entity_id\":\"switch.simple_led_1\"}");
     break;
    case 1:
     Serial.println("Button 2 pressed");
     // An example of switching a switch
     sendSecureRestPost("/api/services/switch/toggle", "{\"entity_id\":\"switch.simple_led_2\"}");
     break;
    case 2:
     Serial.println("Button 3 pressed");
     // An example of turning on a scene
     sendSecureRestPost("/api/services/scene/turn_on", "{\"entity_id\":\"scene.turn_off_both_leds\"}");
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

void sendSecureRestPost(String endpoint, String payload)
{

 WiFiClientSecure *client = new WiFiClientSecure;
  if(client) {
    client->setCACert(rootCACertificate);
   {
      HTTPClient https;

      Serial.print("Posting to ");
      Serial.print(server);
      Serial.print(" on port ");
      Serial.print(port);
      Serial.print(" to endpoint ");
      Serial.println(endpoint);

      if (https.begin(*client, server, port, endpoint)) {
        https.addHeader("Content-Type", "application/json");
        String auth = "Bearer ";
        auth += authtoken;
        https.addHeader("Authorization", auth);
        https.addHeader("cache-control","no-cache");

        Serial.print("Posting: ");
        Serial.println(payload);
          
        int httpCode = https.POST(payload);
  
        if (httpCode > 0) {

          String response = https.getString();

          Serial.print("Posted! Response: ");
          Serial.println(response);
  
        } else {
          Serial.printf("POST... failed, error: %s\n", https.errorToString(httpCode).c_str());
          String payload = https.getString();
          Serial.println(String("Error:") + payload);
        }
  
        https.end();
      } else {
        Serial.println("Unable to connect");
      }
    }
  
    delete client;
  } else {
    Serial.println("Unable to create client");
  }
  
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
