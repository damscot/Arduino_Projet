#include "HX711.h"

#define HX711_DOUT 2
#define HX711_SCK 3

#define CD4052_A 4
#define CD4052_B 5

HX711 scale(HX711_DOUT, HX711_SCK);		// parameter "gain" is ommited; the default value 128 is used by the library

void setup() {
  Serial.begin(115200);
  Serial.println("HX711 Demo");
  pinMode (CD4052_A, OUTPUT);
  digitalWrite (CD4052_A, LOW);
  pinMode (CD4052_B, OUTPUT);
  digitalWrite (CD4052_B, LOW);

  Serial.println("Before setting up the scale:");
  Serial.print("read: \t\t");
  Serial.println(scale.read());			// print a raw reading from the ADC

  Serial.print("read average: \t\t");
  Serial.println(scale.read_average(20));  	// print the average of 20 readings from the ADC

  Serial.print("get value: \t\t");
  Serial.println(scale.get_value(5));		// print the average of 5 readings from the ADC minus the tare weight (not set yet)

  Serial.print("get units: \t\t");
  Serial.println(scale.get_units(5), 1);	// print the average of 5 readings from the ADC minus tare weight (not set) divided 
						// by the SCALE parameter (not set yet)  

  scale.set_scale(2280.f);                      // this value is obtained by calibrating the scale with known weights; see the README for details
  scale.tare();				        // reset the scale to 0
  
  Serial.println("After setting up the scale:");

  Serial.print("read: \t\t");
  Serial.println(scale.read());                 // print a raw reading from the ADC

  Serial.print("read average: \t\t");
  Serial.println(scale.read_average(20));       // print the average of 20 readings from the ADC

  Serial.print("get value: \t\t");
  Serial.println(scale.get_value(5));		// print the average of 5 readings from the ADC minus the tare weight, set with tare()

  Serial.print("get units: \t\t");
  Serial.println(scale.get_units(5), 1);        // print the average of 5 readings from the ADC minus tare weight, divided 
						// by the SCALE parameter set with set_scale

  Serial.println("Readings:");
}

void loop() {
  for (uint8_t i=0; i<4; i++) {
    digitalWrite (CD4052_A, (i&0x1));
    digitalWrite (CD4052_B, ((i>>1)&0x1));
    scale.power_up();
    delay(10);
    Serial.print("Sensor ");
    Serial.print(i);
    Serial.print(" one reading:\t");
    Serial.print(scale.get_units(), 1);
    Serial.print("\t| average:\t");
    Serial.println(scale.get_units(10), 1);
    scale.power_down();
    delay(100);
  }
}
