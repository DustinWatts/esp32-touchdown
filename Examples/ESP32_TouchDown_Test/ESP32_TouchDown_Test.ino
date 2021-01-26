/* ESP32 TouchDown Test
 * by: Dustin Watts
 * date: 30-10-2020
 * 
 * This is the test used to check all new boards. It tests the TFT screen (SPI),
 * the FT6236 touch controller (I2C), WiFi, SPIFFS, BLE, UART, the battery voltage, 
 * the speaker, the SD card, and tests all the broken out pins.
 * 
 * These libraries need to be installed:
 * 
 * - ESP32-BLE-Keyboard forked from T-vk (https://github.com/DustinWatts/ESP32-BLE-Keyboard)
 * - FT6236 (https://github.com/DustinWatts/FT6236)
 * - TFT_eSPI (available through the Library Manager)
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

#include <FT6236.h>        // Capacitive Touch Controller Library
#include <WiFi.h>          // Wifi support
#include <WiFiClient.h>    // Wifi support
#include <BleKeyboard.h>   // BleKeyboard is used to communicate over BLE

#include "BLEDevice.h"     // Additional BLE functionaity
#include "BLEUtils.h"      // Additional BLE functionaity
#include "BLEBeacon.h"     // Additional BLE functionaity

#include "esp_bt_main.h"   // Additional BLE functionaity
#include "esp_bt_device.h" // Additional BLE functionaity

#include <pgmspace.h> // PROGMEM support header
#include "FS.h"       // File System header
#include "SD.h"       // SD Card header 

#include <TFT_eSPI.h> // The TFT_eSPI library

TFT_eSPI tft = TFT_eSPI();

char ssid[64] = " ";           // <-- Your WIFI SSID
char password[64] = " ";       // <-- Your WIFI PASSWORD

uint8_t sdSelectPin = 25;

byte red = 31;
byte green = 0;
byte blue = 0;
byte state = 0;
unsigned int colour = red << 11; // Colour order is RGB 5+6+5 bits each
 
bool initBluetooth()
{
  if (!btStart()) {
    Serial.println("Failed to initialize controller");
    return false;
  }
 
  if (esp_bluedroid_init() != ESP_OK) {
    Serial.println("Failed to initialize bluedroid");
    return false;
  }
 
  if (esp_bluedroid_enable() != ESP_OK) {
    Serial.println("Failed to enable bluedroid");
    return false;
  }

  return true;
 
}
 
void printDeviceAddress() {
 
  const uint8_t* point = esp_bt_dev_get_address();
 
  for (int i = 0; i < 6; i++) {
 
    char str[3];
 
    sprintf(str, "%02X", (int)point[i]);
    Serial.print(str);
    tft.print(str);
 
    if (i < 5){
      Serial.print(":");
      tft.print(":");
    }
 
  }
}

void readFile(fs::FS &fs, const char * path){
    Serial.printf("Reading file: %s\n", path);

    File file = fs.open(path);
    if(!file){
        Serial.println("Failed to open file for reading");
        return;
    }

    Serial.print("Read from file: ");
    while(file.available()){
        Serial.write(file.read());
    }
    file.close();
}

void writeFile(fs::FS &fs, const char * path, const char * message){
    Serial.printf("Writing file: %s\n", path);

    File file = fs.open(path, FILE_WRITE);
    if(!file){
        Serial.println("Failed to open file for writing");
        return;
    }
    if(file.print(message)){
        Serial.println("File written");
    } else {
        Serial.println("Write failed");
    }
    file.close();
}

void appendFile(fs::FS &fs, const char * path, const char * message){
    Serial.printf("Appending to file: %s\n", path);

    File file = fs.open(path, FILE_APPEND);
    if(!file){
        Serial.println("Failed to open file for appending");
        return;
    }
    if(file.print(message)){
        Serial.println("Message appended");
    } else {
        Serial.println("Append failed");
    }
    file.close();
}

FT6236 ts = FT6236();

BleKeyboard bleKeyboard("ESP32 TouchDown Test", "by Dustin Watts");

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
    tft.println("SPIFFS initialised.");

  if (!SD.begin(sdSelectPin))
    {
      Serial.println("SD Card initialisation failed!");
      tft.println("SD Card initialisation failed!");
      while (1)
        yield(); // We stop here
    }
  
  Serial.println("SD Card initialised.");
  tft.println("SD Card initialised.");

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
      tft.println("Unable to start the capacitive touchscreen!");
    } 
    else { 
      Serial.println("Capacitive touch started!");
      tft.println("Capacitive touch started!");
      Serial.println("Touch the screen to start test.");
      tft.println("Touch the screen to start test.");
    }
    
  // Pin modes
  pinMode(33, OUTPUT);
  pinMode(12, OUTPUT);
  pinMode(13, OUTPUT);
  pinMode(14, OUTPUT);
  pinMode(16, OUTPUT);
  pinMode(17, OUTPUT);

  // Setup PWM channel for Piezo speaker
  ledcSetup(0, 500, 8);
  
}

void loop() {

  if (ts.touched()){

      tft.fillScreen(TFT_BLACK);
      Serial.println("Touch received, starting test...");
      tft.println("Touch received, starting test...");
      
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
    
      // Blink breakout header pins

      Serial.println("IOs on...");
      tft.println("IOs on...");
      
      digitalWrite(33, HIGH);
      delay(100);
      digitalWrite(12, HIGH);
      delay(100);
      digitalWrite(13, HIGH);
      delay(100);
      digitalWrite(14, HIGH);
      delay(100);
      digitalWrite(16, HIGH);
      delay(100);
      digitalWrite(17, HIGH);
      delay(100);

      delay(5000);

      Serial.println("IOs off...");
      tft.println("IOs off...");

      digitalWrite(33, LOW);
      delay(100);
      digitalWrite(12, LOW);
      delay(100);
      digitalWrite(13, LOW);
      delay(100);
      digitalWrite(14, LOW);
      delay(100);
      digitalWrite(16, LOW);
      delay(100);
      digitalWrite(17, LOW);
      delay(100);

      // TEST SD Card
      uint8_t cardType = SD.cardType();

      Serial.print("SD Card Type: ");
      tft.print("SD Card Type: ");
        if(cardType == CARD_MMC){
            Serial.println("MMC");
            tft.println("MMC");
        } else if(cardType == CARD_SD){
            Serial.println("SDSC");
            tft.println("SDSC");
        } else if(cardType == CARD_SDHC){
            Serial.println("SDHC");
            tft.println("SDHC");
        } else {
            Serial.println("UNKNOWN");
            tft.println("UNKNOWN");
        }
    
      uint64_t cardSize = SD.cardSize() / (1024 * 1024);
      Serial.printf("SD Card Size: %lluMB\n", cardSize);
      tft.printf("SD Card Size: %lluMB\n", cardSize);

      // Measure battery voltage
      uint16_t adc = analogRead(35);
      Serial.print("ADC VALUE = ");
      tft.print("ADC VALUE = ");
      Serial.println(adc);
      tft.println(adc);
      delay(1000);
      float voltage = adc * 1.782;
      Serial.print("Battery Voltage = ");
      tft.print("Battery Voltage = ");
      Serial.print(voltage / 1000);
      tft.print(voltage / 1000);
      Serial.println("V");
      tft.println("V");
      delay(1000);

      //Starting BLE
      Serial.println("Starting BLE...");
      tft.println("Starting BLE...");
      initBluetooth();
      delay(1000);
      bleKeyboard.begin();
      Serial.print("BLE Device Address: ");
      tft.print("BLE Device Address: ");
      printDeviceAddress();
      Serial.println("");
      tft.println("");
      delay(1000);
      
      //Starting WiFi
      Serial.println("Starting WiFi...");
      tft.println("Starting WiFi...");
      delay(1000);
      Serial.printf("Connecting to %s", ssid);
      tft.print("Connecting to ");
      tft.print(ssid);
      if (String(WiFi.SSID()) != String(ssid))
      {
        WiFi.mode(WIFI_STA);
        WiFi.begin(ssid, password);
      }
    
      while (WiFi.status() != WL_CONNECTED)
      {
        delay(500);
        Serial.print(".");
        tft.print(".");
      }
      
      Serial.println("");
      tft.println("");
      Serial.print("Connected! IP address: ");
      tft.print("Connected! IP address: ");
      Serial.println(WiFi.localIP());
      tft.println(WiFi.localIP());
      delay(1000);

      Serial.println("Starting to draw pretty colours");
      tft.print("Starting to draw pretty colours");

      for(int i=0; i < 5; i++)
      {
        Serial.print(".");
        tft.print(".");  
        delay(500);
      }

      Serial.println(".");
      
      tft.fillScreen(TFT_BLACK);
      
      for(int i=0; i < 50; i++)
      {
        rainbow_fill();
      }

      tft.fillScreen(TFT_BLACK);
      tft.setCursor (0, 0);
      
      Serial.println("Drawing from SPIFFS");
      drawBmpSPIFFS("/thumbsup.bmp", 0, 0);

      delay(5000);

      Serial.println("Drawing from SD");
      drawBmpSD("/thumbsup.bmp", 0, 0);

      delay(5000);

      tft.fillScreen(TFT_BLACK);
      tft.setCursor (0, 0);
      
      Serial.println("Test successfully finished!");
      tft.println("Test successfully finished!");

      WiFi.mode(WIFI_OFF);
      WiFi.disconnect(true);
      
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

void drawBmpSD(const char *filename, int16_t x, int16_t y)
{

  if ((x >= tft.width()) || (y >= tft.height()))
    return;

  fs::File bmpFS;

  bmpFS = SD.open(filename, "r");

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
