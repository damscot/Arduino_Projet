#define TEMP_MONITOR
#define DEBUG

#include <MenuSystem.h>
#include <Encoder.h>
#ifdef TEMP_MONITOR
#include <OneWire.h>
#endif
#include <Wire.h> 
#include <LiquidCrystal_I2C.h>

#define ENCFORW_PIN 2
#define ENCBACKW_PIN 3
#define ENCSW_PIN 4
#define SW0_PIN 5
#define SW1_PIN 6
#define SW2_PIN 7
#define ONEWIRE_PIN A3

// DS18S20 Temperature chip i/o
#ifdef TEMP_MONITOR
OneWire ds(ONEWIRE_PIN);  // on pin 8
#endif
LiquidCrystal_I2C lcd(0x3F,20,4);  // set the LCD address to 0x3F for a 16 chars and 2 line display
Encoder Enc(ENCFORW_PIN, ENCBACKW_PIN);

long oldPosition  = 0;

uint8_t ConfValue = 0;

// Menu variables
MenuSystem ms;
Menu mm("Main Menu");
MenuItem mm_mi1("Menu 1");
MenuItem mm_mi2("Menu 2");
Menu mu1("Configuration");
MenuItem mu1_mi1("Show current");
MenuItem mu1_mi2("Set value");
MenuItem mu1_mi3("Go back");

#define SELECTED_DISPLAY_DELAY 500
boolean menuSelected = false; // no menu item has been selected
boolean commandReceived = false;// no command has been received (Seria, button, etc)
enum setMenuSelected_Type { 
  VALUE_SET, OTHER }; // this enum is in case we want to expand this example
setMenuSelected_Type setMenu;
byte cursorPosition;
String setString;

// Menu callback functions
void on_item1_selected(MenuItem* p_menu_item) {
  lcd.setCursor(0,1);
  lcd.print("Analog A0: ");
  lcd.print(analogRead(A0));
  delay(SELECTED_DISPLAY_DELAY);
}

void on_item2_selected(MenuItem* p_menu_item) {
  lcd.setCursor(0,1);
  lcd.print("ON");
  delay(SELECTED_DISPLAY_DELAY);
}

// on_back_selected is usefull if you don't have a button to make the back function
void on_back_selected(MenuItem* p_menu_item) {
  ms.back();
}

void on_value_show_selected(MenuItem* p_menu_item) {
  char str[4];
  lcd.setCursor(0,1);
  snprintf(str,4,"%03d",ConfValue);
  lcd.print(str);
  delay(SELECTED_DISPLAY_DELAY);
}

void on_value_set_selected(MenuItem* p_menu_item)
{
  char str[4];
  lcd.setCursor(0,1);
  snprintf(str,4,"%03d",ConfValue);
  lcd.print(str);
  cursorPosition = 0;
  lcd.blink();
  setMenu = VALUE_SET;
  menuSelected = true;

}

void displayMenu() {
  lcd.clear();
  lcd.setCursor(0,0);

  // Display the menu
  Menu const* cp_menu = ms.get_current_menu();
  lcd.print(cp_menu->get_selected()->get_name());
}

void updateDisplay() {
  if(!menuSelected && commandReceived) {
    displayMenu();
    commandReceived = false;
  } 
  else if (commandReceived) {
    switch(setMenu) {
    case VALUE_SET:

      break;
      // add new cases as you add set menus (don't forget to add the corresponding enum)
    default:
      break;
    }
  }
}
 
#ifdef TEMP_MONITOR
// number of capteur
byte cap=0;

void temp_loop(void) {
 
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
#endif

void menu_loop() {
  char str[20];
  char sw0,sw1,sw2,encsw;
  long newPosition = (Enc.read())/4;
  if (newPosition != oldPosition) {
    if (newPosition > oldPosition) {
      if(!menuSelected) {
        Serial.println("goto next");      
        ms.next();
      }
      else {
        if(setMenu == VALUE_SET) {
          char str[4];
          ConfValue++;
          snprintf(str,4,"%03d",ConfValue);
          lcd.setCursor(cursorPosition,1);
          lcd.print(str);
        }
      }
    }
    else {
      if(!menuSelected) {
        Serial.println("goto prev");
        ms.prev();
      }
      else {
        if(setMenu == VALUE_SET) {
          char str[4];
          ConfValue--;
          snprintf(str,4,"%03d",ConfValue);
          lcd.setCursor(cursorPosition,1);
          lcd.print(str);
        }
      }
    }
    oldPosition = newPosition;
    commandReceived = true;
  }
  
  sw0 = digitalRead(SW0_PIN);
  if (!sw0) {
    if(!menuSelected)
      ms.back();
    delay(200);
  }
  
  sw1 = digitalRead(SW1_PIN);
  if (!sw1) {
    if(menuSelected)
      menuSelected = false;
    delay(200);
  }

  sw2 = digitalRead(SW2_PIN);
  if (!sw2) {
    if(menuSelected)
      menuSelected = false;
    delay(200);
  }
  
  encsw = digitalRead(ENCSW_PIN);
  if (!encsw) {
    if(!menuSelected)
      ms.select();
    else {
      menuSelected = false;
      ms.back();
    }
    delay(200);
  }
  
  if ((!encsw) || (!sw0) || (!sw1) || (!sw2)) {
    commandReceived = true;
  }
  
  //SELECTED_DISPLAY_DELAY
  updateDisplay();
  
  if(!menuSelected) {
    sprintf(str, "Val:%d      ",newPosition);
    lcd.setCursor(3,2);
    lcd.print(str);
    sprintf(str, "SW:%d,%d,%d,%d ",encsw,sw0,sw1,sw2);
    lcd.setCursor(3,3);
    lcd.print(str);
  }
  
}


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

  
  pinMode(ENCSW_PIN, INPUT);
  pinMode(SW0_PIN, INPUT);
  pinMode(SW1_PIN, INPUT);
  pinMode(SW2_PIN, INPUT);
  
  digitalWrite(ENCSW_PIN, HIGH);
  digitalWrite(SW0_PIN, HIGH);
  digitalWrite(SW1_PIN, HIGH);
  digitalWrite(SW2_PIN, HIGH);
  digitalWrite(ONEWIRE_PIN, HIGH);

  menuSelected = false;
  mm.add_item(&mm_mi1, &on_item1_selected);
  mm.add_item(&mm_mi2, &on_item2_selected);
  mm.add_menu(&mu1);
  mu1.add_item(&mu1_mi1, &on_value_show_selected);
  mu1.add_item(&mu1_mi2, &on_value_set_selected);
  mu1.add_item(&mu1_mi3, &on_back_selected);
  ms.set_root_menu(&mm);
  displayMenu();
  
}

void loop(void) {
#ifdef TEMP_MONITOR
  temp_loop();
#endif
  menu_loop();
}

