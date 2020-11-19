/*  ESP32 TouchDown Battery Monitoring Demo 
 *  by: Dustin Watts
 *  date: 29-10-2020
 *  
 *  This demo uses the BATT_DIV pin (GPIO35) which uses the ADC that is connected to 
 *  a 100K/100K voltage divider. It takes a number of readings that are all added to
 *  totalReadings, with a delay in between.
 *  
 *  After that the total of the ADC readings is divided by the number of readings to get
 *  an average. This is then multiplied by a multiplicationFactor to convert the analog 
 *  readings to the voltage it represents. Because this gives the voltage in mV, the result 
 *  is divided by 1000 to get the voltage in V. This voltage is then printed to serial.
 *  
 *  It then waits a second and repeats.
 *  
 */

uint8_t BATT_DIV = 35;
uint16_t totalReadings = 0;
uint8_t numReadings = 8;
float multiplicationFactor = 1.782;
uint8_t sampleTimeDelay = 250;

void setup() {
  
  Serial.begin(9600);

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
  Serial.print("Battery Voltage = ");
  Serial.print(voltage / 1000);
  Serial.println("V");
  delay(1000);

  // Empty the totalReadings for the next time it is used.
  totalReadings = 0;
  
}
