/*  ESP32 TouchDown Battery Monitoring Demo 
 *  by: Dustin Watts
 *  date: 29-10-2020
 *  
 *  !WARNING Be careful when it comes to LiPo batteries. Always use a protected cell!
 *  
 *  This demo uses the BATT_DIV pin (GPIO35) which uses the ADC that is connected to 
 *  a 100K/100K voltage divider. It takes a number of readings that are all added to
 *  totalReadings, with a delay in between.
 *  
 *  After that the total of the ADC readings is divided by the number of readings to get
 *  an average. This is then multiplied by a multiplicationFactor to convert the analog 
 *  readings to the voltage it represents. This voltage is then printed to serial and
 *  the TFT.
 *  
 *  It then waits a second and repeats.
 *  
 *  This library needs to be installed:
 * 
 *  - TFT_eSPI (available through the Library Manager)
 *  
 *  ! Don't forget to configure User_Setup.h in the TFT_eSPI library. You can use User_setup.h in "/TFT_eSPI Setup Example".
 *
 */

#include <TFT_eSPI.h>

TFT_eSPI tft = TFT_eSPI();

uint8_t BATT_DIV = 35;
uint16_t totalReadings = 0;
uint8_t numReadings = 8;
float multiplicationFactor = 0.0017782; // Adjust this factor to report correct voltages.

/* Explanation of the multiplicationFactor:

The ADC resolution is 12-bit. Meaning that the maximum reading of the ADC is 4095. GPIO35 (BATT_DIV) is conneted to
a voltage divider of two 100K resistors. Meaning that the voltage on that pin is half of what it is on the battery.

The ADC is referenced to an internal 1.1V. The ESP32 VCC voltage is 3.3V.

To convert an ADC reading to a voltage we need to divide that reading by the maximum reading of 4095. Next we need 
to double that because of the 100K/100K voltage divider. Next we multiple that by the ESP32's VCC and then multiply 
that by the reference voltage.

The multiplicationFactor is the result of (3.3*1.1*2)/4095. This gives us 0.0017782... If we now multiply the ADC 
reading by this value, we get a value that is in Volt.

Because the resistors are not exactly 100K, the value we read from the ADC may not be exactly half of the battery voltage.
So it is always an approximation of the battery voltage. To get a bit more accurate results we could verify the voltage on
the BATT_DIV pin against the battery voltage and adjust the multiplicationFactor accordingly. Adjust up if it reads low,
down if it reads higher.

Note: When charging (so when the board is plugged in via USB) the voltage reads higher on the BATT_DIV pin then when 
the battery is supplying charge. When calibrating the multiplicationFactor, use the ADC readings when not plugged in.

You could also take an average of the ADC readings and work you way back to a multiplicationFactor. In that case, divide 
the measured battery voltage by the average ADC readings. Note that you will loose some resolution!

Also note that the ADC of the ESP32 is not linear. So although the voltage may be correct at close to 4.2V it may read wrong at lower
voltages or vice versa. But we can get pretty close!

*/

uint8_t sampleTimeDelay = 250;

void setup() {
  
  Serial.begin(9600);

  // Initialise TFT
  tft.init();
  tft.setTextColor(TFT_WHITE,TFT_BLACK);
  tft.setTextSize(2);
  tft.setRotation(1);

}

void loop() {

  
  // Measure battery voltage
  for(int i=0; i < numReadings; i++)
  {
    uint16_t adcReading = analogRead(BATT_DIV);
    totalReadings = totalReadings + adcReading;
    delay(sampleTimeDelay);
  }

  // Average and convert to Volts
  float voltage = (totalReadings/numReadings) * multiplicationFactor;
  Serial.print("Average ADC reading = ");
  Serial.println(totalReadings/numReadings);
  Serial.print("Battery Voltage = ");
  Serial.print(voltage);
  Serial.println("V");

  // Also Display on the screen
  tft.fillScreen(TFT_BLACK);
  tft.setCursor(0, 0);
  tft.print("Average ADC reading = ");
  tft.println(totalReadings/numReadings);
  tft.print("Battery Voltage = ");
  tft.print(voltage);
  tft.println("V");
  
  delay(1000);

  // Empty the totalReadings for the next time it is used.
  totalReadings = 0;
  
}
