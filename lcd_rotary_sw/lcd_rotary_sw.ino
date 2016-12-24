#include <MenuSystem.h>
#include <Encoder.h>
#include <Wire.h> 
#include <LiquidCrystal_I2C.h>


#define ENCFORW_PIN 3
#define ENCBACKW_PIN 2
#define ENCSW_PIN 4
#define SW0_PIN 5
#define SW1_PIN 6

Encoder Enc(ENCFORW_PIN, ENCBACKW_PIN);
long oldPosition  = 0;

// set the LCD address to 0x27 for a 16 chars and 2 line display
// set the LCD address to 0x3F for a 16 chars and 4 line display
LiquidCrystal_I2C lcd(0x27,20,4);  // set the LCD address to 0x27 for a 16 chars and 2 line display

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


void setup() {
  Serial.begin(115200);
  // initialize the lcd 
  lcd.init();
  lcd.init();
  // Print a message to the LCD.
  lcd.backlight();
  lcd.setCursor(3,0);
  lcd.print("Basic Encoder");

  pinMode(ENCSW_PIN, INPUT);
  pinMode(SW0_PIN, INPUT);
  pinMode(SW1_PIN, INPUT);
  digitalWrite(ENCSW_PIN, HIGH);
  digitalWrite(SW0_PIN, HIGH);
  digitalWrite(SW1_PIN, HIGH);
  
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

void loop() {
  char str[20];
  char sw0,sw1,encsw;
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
  
  if ((!encsw) || (!sw0) || (!sw1)) {
    commandReceived = true;
  }
  
  //SELECTED_DISPLAY_DELAY
  updateDisplay();
  
  if(!menuSelected) {
    sprintf(str, "Val:%d      ",newPosition);
    lcd.setCursor(3,2);
    lcd.print(str);
    sprintf(str, "SW:%d,%d,%d    ",encsw,sw0,sw1);
    lcd.setCursor(3,3);
    lcd.print(str);
  }
  
}
