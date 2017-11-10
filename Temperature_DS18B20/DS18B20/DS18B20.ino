#include <OneWire.h>
#include <Wire.h> 
#include <LiquidCrystal_I2C.h>

/* Use serialplot 0.8.1 for display 
https://bitbucket.org/hyOzd/serialplot/src
*/
 
// DS18S20 Temperature chip i/o
OneWire ds(10);  // on pin 10

// set the LCD address to 0x27 for a 16 chars and 2 line display
// set the LCD address to 0x3F for a 16 chars and 4 line display
LiquidCrystal_I2C lcd(0x27,20,4);  // set the LCD address to 0x27 for a 16 chars and 2 line display

// number of capteur
byte cap=0;
 
void setup(void) {
  // initialize inputs/outputs
  // start serial port
  Serial.begin(115200);

  // initialize the lcd 
  lcd.init();
  lcd.init();
  // Print a message to the LCD.
  lcd.backlight();
  lcd.setCursor(3,0);
  lcd.print("Multi Termometre");
}

//#define DEBUG

void loop(void) {
 
  //For conversion of raw data to C
  int HighByte, LowByte, TReading, SignBit, Tc_100, Whole, Fract;
 
  byte i;
  byte present = 0;
  byte data[12];
  byte addr[8];
 
  if ( !ds.search(addr)) {
#ifdef DEBUG
      Serial.print("No more addresses.\n");
#else
      Serial.print("\n");
#endif
      ds.reset_search();
      cap=0;
      return;
  }

#ifdef DEBUG
  Serial.print("R=");
  for( i = 0; i < 8; i++) {
    Serial.print(addr[i], HEX);
    Serial.print(" ");
  }
#endif
 
  if ( OneWire::crc8( addr, 7) != addr[7]) {
      Serial.print("CRC is not valid!\n");
      return;
  }
 
  if ( addr[0] == 0x10) {
#ifdef DEBUG
      Serial.print("Device is a DS18S20 family device.\n");
#endif
  }
  else if ( addr[0] == 0x28) {
#ifdef DEBUG
      Serial.print("Device is a DS18B20 family device.\n");
#endif
  }
  else {
      Serial.print("Device family is not recognized: 0x");
      Serial.println(addr[0],HEX);
      return;
  }
 
  ds.reset();
  ds.select(addr);
  ds.write(0x44,1);         // start conversion, with parasite power on at the end
 
  delay(1000);     // maybe 750ms is enough, maybe not
  // we might do a ds.depower() here, but the reset will take care of it.
 
  present = ds.reset();
  ds.select(addr);    
  ds.write(0xBE);         // Read Scratchpad

#ifdef DEBUG
  Serial.print("P=");
  Serial.print(present,HEX);
  Serial.print(" ");
#endif
  for ( i = 0; i < 9; i++) {           // we need 9 bytes
    data[i] = ds.read();
#ifdef DEBUG
    Serial.print(data[i], HEX);
    Serial.print(" ");
#endif
  }
#ifdef DEBUG
  Serial.print(" CRC=");
  Serial.print( OneWire::crc8( data, 8), HEX);
  Serial.println();
#endif
 
  //Conversion of raw data to C
  LowByte = data[0];
  HighByte = data[1];
  TReading = (HighByte << 8) + LowByte;
  SignBit = TReading & 0x8000;  // test most sig bit
  if (SignBit) // negative
  {
    TReading = (TReading ^ 0xffff) + 1; // 2's comp
  }
  Tc_100 = (6 * TReading) + TReading / 4;    // multiply by (100 * 0.0625) or 6.25
 
  Whole = Tc_100 / 100;  // separate off the whole and fractional portions
  Fract = Tc_100 % 100;
 
  lcd.setCursor(((cap % 2) * 11), ((cap / 2) + 1));
  //Serial.print("T");
  lcd.print("T");
  lcd.print(cap);
  //Serial.print(cap);
  //Serial.print(";");
  
  if (SignBit) // If its negative
  {
     Serial.print("-");
     lcd.print(" -");
  }
  else {
     lcd.print("  "); 
  }
  Serial.print(Whole);
  lcd.print(Whole);
  Serial.print(".");
  lcd.print(".");
  if (Fract < 10)
  {
     Serial.print("0");
     lcd.print("0");
  }
  Serial.print(Fract);
  lcd.print(Fract/10);
  if (!(cap & 0x1)) {
    //Separateur
    lcd.print(" |");
  }
  
#ifdef DEBUG
  Serial.print("\n");
#else
  Serial.print(",");
#endif 
  cap +=1;
  
  //End conversion to C
}
