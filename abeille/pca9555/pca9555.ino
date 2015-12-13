#include <Wire.h>
#include <pca9555.h>   // import library
//will work even for tca9555


pca9555 mcp(0x20);//instance

void setup(){
  Serial.begin(115200);
  delay(100);
  mcp.begin();//x.begin(true) will override automatic SPI initialization
  mcp.gpioPinMode(OUTPUT);

}

void loop(){
  unsigned long bef,aft;
  //Serial.print("\nHIGH ");
  for (int i = 0; i < 16; i++) {
    bef = micros();
    mcp.gpioDigitalWrite(i, HIGH);
    aft = micros();
    /*Serial.print(i);
    Serial.print(" ");*/
    delay(5);
  }

  //Serial.print("\nLOW ");
  for (int i = 0; i < 16; i++) {
    mcp.gpioDigitalWrite(i, LOW);
    /*Serial.print(i);
    Serial.print(" ");*/
    delay(5);
  } 
  
  /*Serial.print("\nTIME ");
  Serial.print(aft-bef);*/
  
}
