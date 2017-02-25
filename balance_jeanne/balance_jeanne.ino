#include <MenuSystem.h>
#include <Encoder.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <Keypad_I2Ca.h>
#include <Keypad.h>


long newPosition;

#define ENCFORW_PIN 3
#define ENCBACKW_PIN 2
#define ENCSW_PIN 4

#define BALANCE_SCK_PIN A2
#define BALANCE_SI_PIN A3

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

/****************************************/
/* DECODE WEIGHT AND PRICE FROM BALANCE */
/****************************************/

#define BAL_LEN 40 /*270 ???*/

void get_serial_balance (char poids[10], char pu[10], char prix[10])
{
  byte myDataIn[BAL_LEN] = { 0 };
  unsigned long temp, temp2;
  int i, j, pinState;
  int poids_kg;
  int poids_gr;
  int pu_euro;
  int pu_cent;
  int prix_euro;
  int prix_cent;
  
  while(1)
  {

  temp = pulseIn(BALANCE_SCK_PIN, HIGH, 200000);
  if ((temp == 0) || (temp < 40000)) {
    continue;
  }
  else
    break;
  }
    
  for (j=0; j<BAL_LEN; j++)
  {
  for (i=7; i>=0; i--)
  {
    delayMicroseconds(20);
    temp = digitalRead(BALANCE_SI_PIN);
    if (temp) {
        pinState = 1;
        //set the bit to 0 no matter what
        myDataIn[j] = myDataIn[j] | (1 << i);
    }
    else {
        //turn it off -- only necessary for debuging
        //print statement since myDataIn starts as 0
        pinState = 0;
    }
    
    temp = pulseIn(BALANCE_SCK_PIN, HIGH, 2000);
    if ((temp == 0) || (temp > 500)) {
      continue;
    }
  }
  }
  
  /*Serial.println("****************");
  for (j=2; j<40; j++) {
    if (myDataIn[j] != 0)
      Serial.print("data (");
      Serial.print(j,DEC);
      Serial.print("):");
      Serial.println(myDataIn[j],HEX);
  }
  Serial.println("****************");*/

  poids_kg = ((myDataIn[2] & 0xF0)>>4);
  poids_gr = ((myDataIn[3] & 0xF) * 100) +
            (((myDataIn[5] & 0xF0)>>4) * 10) +
            ((myDataIn[6] & 0xF) * 1);
            
  sprintf(poids, "%d.%03d",poids_kg, poids_gr);
  
  pu_euro = ((myDataIn[15] & 0xF) * 100) +
            (((myDataIn[17] & 0xF0)>>4) * 10) +
            (myDataIn[18] & 0xF);
  pu_cent = (((myDataIn[20] & 0xF0)>>4) * 10) +
            (myDataIn[21] & 0xF);
  
  sprintf(pu, "%d.%02d",pu_euro, pu_cent);
  
  prix_euro = (((myDataIn[23] & 0xF0)>>4) * 1000) +
              ((myDataIn[24] & 0xF) * 100) +
              (((myDataIn[26] & 0xF0)>>4) *10) +
              (myDataIn[27] & 0xF);
  prix_cent = (((myDataIn[29] & 0xF0)>>4) * 10) +
              (myDataIn[30] & 0xF);
              
  sprintf(prix, "%d.%02d",prix_euro, prix_cent);
  
  Serial.print("poids :");
  Serial.println(poids);
  
  Serial.print("pu :");
  Serial.println(pu);
  
  Serial.print("prix :");   
  Serial.println(prix);
  
}

/******************************************/
/* KEYBOARD MGMT FROM AND BACK TO BALANCE */
/******************************************/

//long offsetdelay[3] = {0};
//int offsetdelay_pos=0;

const byte ROWS = 7; //rows
const byte COLS = 3; //columns
//define the cymbols on the buttons of the keypads
char keys[ROWS][COLS] = {
  {'*','P','F'},
  {'0','2','5'},
  {'C','1','4'},
  {'T','8','9'},
  {'Z','3','6'},
  {'H','E','D'},
  {'I','7','N'}
};

byte rowPins[ROWS] = { 0, 1, 2, 3, 4, 5, 9}; //connect to the row pinouts of the keypad
byte colPins[COLS] = {6, 7, 8}; //connect to the column pinouts of the keypad

//initialize an instance of class NewKeypad
Keypad_I2Ca customKeypad = Keypad_I2Ca( makeKeymap(keys), rowPins, colPins, ROWS, COLS, 0x20, PCA9555 );

/* transparent key map */
char keys_out[ROWS][COLS] = {
  {'D','*','N'},
  {'E','H','I'},
  {'T','F','P'},
  {'C','Z','0'},
  {'9','6','3'},
  {'8','5','2'},
  {'7','4','1'}
};

/* partially transparent key map */
char keys_out_remap[ROWS][COLS] = {
  {'d','=','b'},
  {'c','b','a'},
  {'T','+','-'},
  {'C','Z','0'},
  {'9','6','3'},
  {'8','5','2'},
  {'7','4','1'}
};

int keys_out_delay[7][3] = {
  {160,1480,1140},
  {335,1480,1075},
  {535,1470,1072},  
  {710,1470,1083},
  {920,1500,1030},
  {1200,1380,1100},
  {1415,1490,1090}
};

int keys_out_length[7][3] = {
  {200,200,200},
  {200,200,200},
  {200,200,200},
  {200,200,200},
  {200,200,200},
  {200,200,200},
  {150,150,150}
};

byte rowPins_out[ROWS] = {A0, 7, 10, 9, 11, 8, 12}; //connect to the row pinouts of the keypad
byte colPins_out[COLS] = {6, 5, A1}; //connect to the column pinouts of the keypad

int search_keys_out(char in, char* out, int *row, int *col)
{
  int i,j;
  Serial.print("Search : ");
  Serial.println(in);
  for(i = 0; i < ROWS; i++)
  {
    for(j = 0; j < COLS; j++)
    {
      if (in == keys_out[i][j]) {
        Serial.print("Found: ");
        Serial.print(i);
        Serial.print(" , ");
        Serial.println(j);
        *row = i;
        *col =j;
        *out = keys_out_remap[i][j];
        if (keys_out_remap[i][j] == keys_out[i][j])
          return 1;
        else
          return 0;
      }
    }
  }
  return 0;
}

char send_keys_out(char in)
{
      int row,col,col_pin;
      unsigned long temp;
      char str[20];
      char out = 0;
      
      if (search_keys_out(in, &out, &row, &col)) {
        /* get column pin */
        col = colPins_out[col];
        while (1) {
        /* pin 8 is Row 3 (2,5,8 character) is is a synchro signal */
        temp = pulseIn(8, HIGH, 110000);
        if (temp > 50000) {
          delayMicroseconds(keys_out_delay[row][0]/*+offsetdelay[0]*/);
          digitalWrite(col,HIGH);
          delayMicroseconds(keys_out_length[row][0]);
          digitalWrite(col,LOW);
          delayMicroseconds(keys_out_delay[row][1]/*+offsetdelay[1]*/);
          digitalWrite(col,HIGH);
          delayMicroseconds(keys_out_length[row][1]);
          digitalWrite(col,LOW);
          delayMicroseconds(keys_out_delay[row][2]/*+offsetdelay[2]*/);
          digitalWrite(col,HIGH);
          delayMicroseconds(keys_out_length[row][2]);
          digitalWrite(col,LOW);
          Serial.print("Send Key: ");
          Serial.println(in);
          /*Serial.print("Value: ");
          Serial.print(row);
          Serial.print(": ");
          Serial.print(keys_out_delay[row][0]+offsetdelay[0]);
          Serial.print(" ");
          Serial.print(keys_out_delay[row][1]+offsetdelay[1]);
          Serial.print(" ");
          Serial.println(keys_out_delay[row][2]+offsetdelay[2]);
          sprintf(str, "%d:%ld,%ld,%ld   ",offsetdelay_pos, offsetdelay[0], offsetdelay[1], offsetdelay[2]);
          lcd.setCursor(0,3);
          lcd.print(str);*/
          break;
        }
        }
      }
      
      return out;
}

/***********************************/
/*               SETUP             */
/***********************************/

void setup() {
  int i;
  
  Serial.begin(115200);
  // initialize the lcd 
  lcd.init();
  lcd.init();
  // Print a message to the LCD.
  lcd.backlight();
  lcd.setCursor(3,0);
  lcd.print("Basic Encoder");

  pinMode(ENCSW_PIN, INPUT);
  digitalWrite(ENCSW_PIN, HIGH);
  
  pinMode(BALANCE_SCK_PIN, INPUT);
  pinMode(BALANCE_SI_PIN, INPUT);
  digitalWrite(BALANCE_SCK_PIN, HIGH);
  digitalWrite(BALANCE_SI_PIN, HIGH);
  
  for (i=0; i< ROWS; i++)
  {
    pinMode(rowPins_out[i],INPUT);
  }
  for (i=0; i< COLS; i++)
  {
    pinMode(colPins_out[i],OUTPUT);
    digitalWrite(6,LOW);
  }

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

/***********************************/
/*             MAIN LOOP           */
/***********************************/

void loop() {
  char str[20];
  char inputKey = customKeypad.getKey();
  char outputKey;
  char poids[10], pu[10], prix[10];
    
  char sw0,sw1,encsw;
  newPosition = (Enc.read())/4;
  //offsetdelay[offsetdelay_pos] = newPosition;
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
  
  encsw = digitalRead(ENCSW_PIN);
  if (!encsw) {
    /*offsetdelay_pos +=1;
    if (offsetdelay_pos >= 3)
      offsetdelay_pos = 0;*/
    if(!menuSelected)
      ms.select();
    else {
      menuSelected = false;
      ms.back();
    }
    delay(200);
  }
  
  if (!encsw) {
    commandReceived = true;
  }
  
  //SELECTED_DISPLAY_DELAY
  updateDisplay();
  
  if(!menuSelected) {
    
    if (inputKey){
      outputKey = send_keys_out(inputKey);
      sprintf(str, "Key:%c   ",outputKey);
      lcd.setCursor(3,1);
      lcd.print(str);
      if (outputKey == '=')
        get_serial_balance(poids,pu,prix);
    }
    sprintf(str, "Val:%d   ",newPosition);
    lcd.setCursor(3,2);
    lcd.print(str);
    sprintf(str, "SW:%d  ",encsw);
    lcd.setCursor(0,3);
    lcd.print(str);
  }
  
  
}
