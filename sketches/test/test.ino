/* How to use the DHT-22 sensor with Arduino uno
   Temperature and humidity sensor
   More info: http://www.ardumotive.com/how-to-use-dht-22-sensor-en.html
   Dev: Michalis Vasilakis // Date: 1/7/2015 // www.ardumotive.com */

//Libraries
#include "utils.h"

/*
 Dust Sensor project for arduino
 sensor GP2Y1010AU0F
 www.keyestudio.com
*/

#define ANALPIN 0
#define LEDPIN 3
#define SAMPLING 280
#define DELTATIME 40
#define SLEEPTIME 9680


float voMeasured = 0;
float calcVoltage = 0;
float dustDensity = 0;

void setup()
{
    SerialDebug.begin(SERIAL_DEFAULT_BAUDRATE);
//     utils::board::debug_init();
    
    pinMode(LEDPIN, OUTPUT);
    SerialDebug.println("Dust sensor START: ");

}
void loop(){

    digitalWrite(LEDPIN, LOW); // power on the LED

    delayMicroseconds(SAMPLING);
    voMeasured = analogRead(ANALPIN); // read the dust value
    delayMicroseconds(DELTATIME);
    
    digitalWrite(LEDPIN,HIGH); // turn the LED off
//     delayMicroseconds(SLEEPTIME);
    
  // 0 - 5V mapped to 0 - 1023 integer values
  // recover voltage
    calcVoltage = voMeasured * (5.0 / 1024.0);
    dustDensity = 170 * calcVoltage - 0.1;
    SerialDebug.print("Analog value = ");
    SerialDebug.println(voMeasured);
    SerialDebug.print("Mapped value = ");
    SerialDebug.println(calcVoltage);
    SerialDebug.print("The dust concentration is: ");
    SerialDebug.print(dustDensity);
    SerialDebug.print(" ug/m3\n");  

    delay(1000); //Delay 2 sec.
}
