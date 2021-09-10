/**
 * ESP32 TouchDown Example using ArduinoOSC
 * date: 10-9-2021
 * 
 * This example shows how to use ArduinoOSC to display cues. The cue format is:
 * 
 * -----------------------------
 * /PATH "String of text" color 
 * -----------------------------
 * 
 * Color options are red, green, blue, orange, yellow, purple. If no color is specified, it uses magenta.
 * 
 * Settings are loaded from an SD card. An example settings.json is provided in the SD folder. After succesfully loading 
 * the settings from SD, the settings are stored to SPIFFS so the SD card is not needed anymore. To update the settings
 * simply plug in an SD card with different settings in the settings.json.
 * 
 * You can touch the screen and stop displaying a cue.
 * 
 * 
 * These libraries need to be installed:
 * 
 * - FT6236 (https://github.com/DustinWatts/FT6236)
 * - TFT_eSPI (available through the Library Manager)
 * - ArduinoOSC (available through the Library Manager)
 * - ArduinoJSON (available through the Library Manager)
 * 
 * ! Don't forget to configure User_Setup.h in the TFT_eSPI library. You can use User_setup.h in "/TFT_eSPI Setup Example".
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


//#define ARDUINOOSC_DEBUGLOG_ENABLE
//#define USE_LittleFS

// Includes
#include <ArduinoOSCWiFi.h>
#include <TFT_eSPI.h>
#include <SD.h>
#include <FS.h>
#include <ArduinoJson.h>
#include <FT6236.h>
#include <FS.h>
#ifdef USE_LittleFS
  #define SPIFFS LITTLEFS
  #include <LITTLEFS.h> 
#else
  #include <SPIFFS.h>
#endif 

// Create tft and touchscreen objects
TFT_eSPI tft = TFT_eSPI();
FT6236 ts = FT6236();

// If opening settings from SD or SPIFFS this is the fallback WiFi stuff.
const char* fallbackssid = "fallbackssid"; // Change this to match your WiFi
const char* fallbackpass = "fallbackpass"; // Change this to match your WiFi
const char* fallbackpath = "fallbackpath"; // Change this to match your WiFi


// for ArduinoOSC
const int recv_port = 54445;
const char* path;
const char* ssid;
const char* pwd;

// Send / receive varibales
int i;
float f;
String s;

// Color string to color
int getColor(String colorstring){

  if(colorstring != NULL){

    if(colorstring.equalsIgnoreCase("red")){

      return TFT_RED;
      
    }

    else if(colorstring.equalsIgnoreCase("green")){

      // For this background we use black text, as it is better readable.
      tft.setTextColor(TFT_BLACK);
      return TFT_GREEN;
      
    }

    else if(colorstring.equalsIgnoreCase("blue")){

      return TFT_BLUE;
      
    }

    else if(colorstring.equalsIgnoreCase("orange")){

      return TFT_ORANGE;
      
    }

    else if(colorstring.equalsIgnoreCase("purple")){

      return TFT_PURPLE;
      
    }

    else if(colorstring.equalsIgnoreCase("yellow")){

      // For this background we use black text, as it is better readable.
      tft.setTextColor(TFT_BLACK);
      return TFT_YELLOW;
      
    }


   return TFT_MAGENTA;
    
  } 

  return TFT_MAGENTA;
}

// A little blinking ! in the top right corner
void notify(int color){

  tft.setTextSize(4);

  for(int i=0; i<5; i++){

    tft.setCursor(450,10);
    tft.print("!");
    delay(300);
    tft.fillRect(420,0,480,50,color);
    delay(300);
    
  }

  tft.setTextSize(3);
}

// On reveice callback function
void onOscReceived(const OscMessage& m) {

      int color = getColor(m.arg<String>(1));

      tft.fillScreen(color);

      tft.setCursor(35,130);
      tft.println(m.arg<String>(0));

      notify(color);

      // Reset the text color to white
      tft.setTextColor(TFT_WHITE);

}


void setup() {
    Serial.begin(115200);
    delay(2000);

    // Init the screen
    tft.begin();
    tft.setRotation(1);

    // Display connecting to the screen
    tft.fillScreen(TFT_BLUE);
    tft.setTextColor(TFT_WHITE);
    tft.setTextSize(2);

    SPIFFS.begin();

    // Start the touchscreen
    if (!ts.begin(40))
    {
        Serial.println("Unable to start the capacitive touchscreen.");
    }

    // Begin SD
    SD.begin(25);
      

    // Open settings json file
    File configfile = SD.open("/settings.json");

    if(configfile){
      // Process settings using ArduinoJSON

    Serial.println("Loading settings from SD");
    tft.println("Loading settings from SD");

    StaticJsonDocument<200> doc;
    DeserializationError error = deserializeJson(doc, configfile);
    
    if (error) {
      Serial.print(F("deserializeJson() failed: "));
      Serial.println(error.f_str());
    }


    ssid = doc["wifissid"];
    pwd = doc["password"];
    path = doc["path"];

    // we were able to open te config file, save settings to Flash File System
    
    File spiffsfile = SPIFFS.open("/settings.json", FILE_WRITE);
    if(!spiffsfile){
        Serial.println("- failed to open file for writing");
    }
    else
    {

      Serial.println("Copying settings to SPIFFS");
      tft.println("Copying settings to SPIFFS");

      Serial.print("{\n");
      Serial.print("\"wifissid\":\"");
      Serial.print(ssid);
      Serial.print("\",\n \"password\":\"");
      Serial.print(pwd);
      Serial.print("\",\n \"path\":\"");
      Serial.print(path);
      Serial.print("\"\n}");   

      spiffsfile.print("{\n");
      spiffsfile.print("\"wifissid\":\"");
      spiffsfile.print(ssid);
      spiffsfile.print("\",\n \"password\":\"");
      spiffsfile.print(pwd);
      spiffsfile.print("\",\n \"path\":\"");
      spiffsfile.print(path);
      spiffsfile.print("\"\n}");     
    }

       spiffsfile.close(); 

       configfile.close();

  }
  else
  {

      Serial.println("Loading settings from SPIFFS");
      tft.println("Loading settings from SPIFFS");

    File spiffsfile = SPIFFS.open("/settings.json", FILE_READ);

    StaticJsonDocument<200> doc;
    DeserializationError error = deserializeJson(doc, spiffsfile);
    
    if (error) {
      Serial.print(F("deserializeJson() failed: "));
      Serial.println(error.f_str());

      Serial.println("SPIFFS load failed, using fallback");
      tft.println("SPIFFS load failed, using fallback");
    }

    ssid = doc["wifissid"] | fallbackssid;
    pwd = doc["password"] | fallbackpass;
    path = doc["path"] | fallbackpath;

    spiffsfile.close();
    
  }

    tft.print("Connecting to: ");
    tft.println(ssid);
    

    // WiFi stuff (no timeout setting for WiFi)    
    WiFi.disconnect(true, true);  // disable wifi, erase ap info
    delay(1000);
    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid, pwd);
    while (WiFi.status() != WL_CONNECTED) {
        Serial.print(".");
        delay(500);
    }    

    // Subscribe to OSC messages
    OscWiFi.subscribe(recv_port, path, onOscReceived);

    // Everthing OK, display info to screen
    tft.fillScreen(TFT_BLUE);
    tft.setCursor(0,0);
    tft.println("Connected!");
    tft.print("IP = ");
    tft.println(WiFi.localIP()); 
    tft.println("Waiting for OSC messages");
    tft.print("Port: ");
    tft.println(recv_port);
    tft.print("Path: ");
    tft.println(path);

    // Set the textsize for displaying messages
    tft.setTextSize(3);

    
}

void loop() {

  // Check for touch
  if (ts.touched())
    {
        // Retrieve a point
        TS_Point p = ts.getPoint();

        // For now we are doing nothing with this point
        // expect filling the screen with black

     tft.fillScreen(TFT_BLACK);

    }
   
    // update() is called to receive + send osc
    OscWiFi.update();
}
