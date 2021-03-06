#include <MenuSystem.h>
#include <avr/pgmspace.h>
#include <Encoder.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <Keypad_I2Ca.h>
#include <Keypad.h>

// Rotary Encoder Global Position
long newPosition;
uint8_t idx_art;

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

/***********************************/
/*             ARTICLES            */
/***********************************/

int current_art_pu_cent_kg;

typedef enum famille_art {
  MANUEL=0,
  LEGUMES,
  FRUITS,
  FROMAGES,
  CHARCUTERIES,
} famille_art_t;

typedef struct articles
{
  uint8_t code;
  char name[20];
  int pu_cent_kg;
} articles_t;

static const articles_t PROGMEM manuel[] = {
  {-1,NULL,0},
};

static const articles_t PROGMEM legumes[] = {
  {0,"legumes",-1},
  {1,"Tomate Bio",211},
  {2,"Poireau",245},
  {3,"Carotte",235},
  {4,"Pomme de Terre Bio",235},
  {5,"Tomate Bio",211},
  {6,"Poireau",245},
  {7,"Carotte",235},
  {8,"Pomme de Terre Bio",235},
  {9,"Tomate Bio",211},
  {10,"Poireau",245},
  {11,"Carotte",235},
  {12,"Poireau",245},
  {13,"Carotte",235},
  {14,"Pomme de Terre Bio",235},
  {15,"Tomate Bio",211},
  {16,"Poireau",245},
  {17,"Carotte",235},
  {18,"Pomme de Terre Bio",235},
  {19,"Tomate Bio",211},
  {20,"Poireau",245},
  {-1,NULL,0},
};

static const articles_t PROGMEM fruits[] = {
  {0,"fruits",-1},
  {1,"Poire",311},
  {2,"Ananas",275},
  {3,"Pomme Bio",135},
  {4,"Poire",311},
  {5,"Ananas",275},
  {6,"Pomme Bio",135},
  {7,"Poire",311},
  {8,"Ananas",275},
  {9,"Pomme Bio",135},
  {10,"Poire",311},
  {11,"Ananas",275},
  {12,"Pomme Bio",135},
  {13,"Poire",311},
  {14,"Ananas",275},
  {15,"Pomme Bio",135},
  {16,"Poire",311},
  {17,"Ananas",275},
  {18,"Pomme Bio",135},
  {19,"Poire",311},
  {20,"Ananas",275},
  {-1,NULL,0},
};

static const articles_t PROGMEM fromages[] = {
  {0,"fromages",-1},
  {1,"Gruyere",211},
  {2,"Emmental",245},
  {3,"Gouda",235},
  {4,"Bleu Auvergne",235},
  {5,"Chevre Schudeler",235},
  {6,"Tomme de Savoie",235},
  {7,"Gruyere",211},
  {8,"Emmental",245},
  {9,"Gouda",235},
  {10,"Bleu Auvergne",235},
  {11,"Chevre Schudeler",235},
  {12,"Tomme de Savoie",235},
  {13,"Gruyere",211},
  {14,"Emmental",245},
  {15,"Gouda",235},
  {16,"Bleu Auvergne",235},
  {17,"Chevre Schudeler",235},
  {18,"Tomme de Savoie",235},
  {19,"Gruyere",211},
  {20,"Emmental",245},
  {-1,NULL,0},
};

static const articles_t PROGMEM charcuteries[] = {
  {0,"charcuteries",-1},
  {1,"Saucisse Ardeche",311},
  {2,"Jambon",275},
  {3,"Salami",135},
  {4,"Saucisse Ardeche",311},
  {5,"Jambon",275},
  {6,"Salami",135},
  {7,"Saucisse Ardeche",311},
  {8,"Jambon",275},
  {9,"Salami",135},
  {10,"Saucisse Ardeche",311},
  {11,"Jambon",275},
  {12,"Salami",135},
  {13,"Saucisse Ardeche",311},
  {14,"Jambon",275},
  {15,"Salami",135},
  {16,"Saucisse Ardeche",311},
  {17,"Jambon",275},
  {18,"Salami",135},
  {19,"Saucisse Ardeche",311},
  {20,"Jambon",275},
  {-1,NULL,0},
};

typedef struct famille
{
  uint8_t size;
  const articles_t * table;
} famille_t;

static const famille_t art_table [] = {
  [MANUEL] =  {sizeof(manuel)/sizeof(articles_t),manuel},
  [LEGUMES] =  {sizeof(legumes)/sizeof(articles_t),legumes},
  [FRUITS] = {sizeof(fruits)/sizeof(articles_t),fruits},
  [FROMAGES] = {sizeof(fromages)/sizeof(articles_t),fromages},
  [CHARCUTERIES] = {sizeof(charcuteries)/sizeof(articles_t),charcuteries},
};

typedef enum article_status {
  ART_NONE=0,
  ART_UNDEF,
  ART_PRICE
} article_status_t;

int Display_Art(uint8_t fam, uint8_t idx) {
  if ((fam > MANUEL) && (fam <= CHARCUTERIES) && (idx < art_table[fam].size)) {
    char str[20];
    articles_t art;
    char * src = (char *)(&(art_table[fam].table[idx]));
    char * dest= (char *)(&art);
    
    for (int i = 0; i < sizeof(art); i++)
    {
      *dest++ = pgm_read_byte_near(src++);
    }
    
    sprintf(str, "%d,%d,%d   ",fam, idx, art_table[fam].size);
    lcd.setCursor(0,1);
    lcd.print(str);   
    sprintf(str, "%s",art.name);
    lcd.setCursor(0,2);
    lcd.print(str);
    
    current_art_pu_cent_kg = art.pu_cent_kg;
    sprintf(str, "Prix: %d.%02d  Euro   ",art.pu_cent_kg/100 ,art.pu_cent_kg%100);
    lcd.setCursor(0,3);
    lcd.print(str);
    if (current_art_pu_cent_kg == -1)
      return ART_UNDEF;
    else
      return ART_PRICE;
  }
  else {
    idx_art = 0;
    return ART_NONE;
  }
}

/***********************************/
/*             MENU UI             */
/***********************************/

// Menu variables
MenuSystem ms;
Menu mm("Main Menu");
MenuItem mm_mi1("* MANUEL");
Menu mu1("=> LEGUMES");
MenuItem mu1_mi1("* LEGUMES");
Menu mu2("=> FRUITS");
MenuItem mu2_mi1("* FRUITS");
Menu mu3("=> FROMAGES");
MenuItem mu3_mi1("* FROMAGES");
Menu mu4("=> CHARCUTERIE");
MenuItem mu4_mi1("* CHARCUTERIE");
Menu muc("Configuration");
MenuItem muc_mi1("Show current");
MenuItem muc_mi2("Set value");
MenuItem muc_mi3("Go back");

#define SELECTED_DISPLAY_DELAY 500

boolean menuSelected = false; // no menu item has been selected
boolean commandReceived = false;// no command has been received (Seria, button, etc)
enum setMenuSelected_Type { 
  VALUE_SET, OTHER }; // this enum is in case we want to expand this example
setMenuSelected_Type setMenu;
byte cursorPosition;
String setString;

// Initialize Menu
void setup_menu(void) {
  menuSelected = false;
  mm.add_item(&mm_mi1, &on_item1_selected);
  mm.add_menu(&mu1);
  mu1.add_item(&mu1_mi1, &on_back_selected);
  mm.add_menu(&mu2);
  mu2.add_item(&mu2_mi1, &on_back_selected);
  mm.add_menu(&mu3);
  mu3.add_item(&mu3_mi1, &on_back_selected);
  mm.add_menu(&mu4);
  mu4.add_item(&mu4_mi1, &on_back_selected);
   
  mm.add_menu(&muc);
  muc.add_item(&muc_mi1, &on_value_show_selected);
  muc.add_item(&muc_mi2, &on_value_set_selected);
  muc.add_item(&muc_mi3, &on_back_selected);
  ms.set_root_menu(&mm);
  displayMenu();
}


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
  //Serial.print("Search : ");
  //Serial.println(in);
  for(i = 0; i < ROWS; i++)
  {
    for(j = 0; j < COLS; j++)
    {
      if (in == keys_out[i][j]) {
        //Serial.print("Found: ");
        //Serial.print(i);
        //Serial.print(" , ");
        //Serial.println(j);
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

char send_keys_out(char in, int to_balance)
{
      int row,col,col_pin;
      unsigned long temp;
      char str[20];
      char out = 0;
      
      if (search_keys_out(in, &out, &row, &col)) {
        col = colPins_out[col];
        /* get column pin */
        while (to_balance) {
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
          //Serial.print("Send Key: ");
          //Serial.println(in);
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

void send_number_out(int val)
{
  int i = 0;
  char number[6];

  while ((i < 6) && val) {
    //send_keys_out('0'+(val%10),1);
    number[i++] = '0' + val%10;
    val = ((val - val%10)/10);
  }
  
  send_keys_out('C',1);
  delay(50);
  while (i > 0) {
    send_keys_out(number[--i],1);
    delay(50);
  }
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
  
  setup_menu();

}

/***********************************/
/*             MAIN LOOP           */
/***********************************/
article_status_t art_stat = ART_NONE;

void loop() {
  char str[20];
  char inputKey = customKeypad.getKey();
  char outputKey;
  char poids[10], pu[10], prix[10];
  const Menu * curent_menu;
    
  char sw0,sw1,encsw;
  newPosition = (Enc.read())/4;
  //offsetdelay[offsetdelay_pos] = newPosition;
  if (newPosition != oldPosition) {
    if (newPosition > oldPosition) {
      if(!menuSelected) {
        idx_art ++;      
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
        ms.prev();
        idx_art --;
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
  curent_menu = ms.get_current_menu();
  
  if(!menuSelected) {
    
    if (inputKey){
      outputKey = send_keys_out(inputKey, (art_stat != ART_PRICE));
      sprintf(str, "Key:%c   ",outputKey);
      lcd.setCursor(3,1);
      lcd.print(str);
      if (outputKey == '=') {
        send_number_out(current_art_pu_cent_kg);
        delay(100);
        get_serial_balance(poids,pu,prix);
      }
    }
    
    
    if ((curent_menu != &mm) && (curent_menu != &muc))
      art_stat = (article_status_t)(Display_Art(mm.get_cur_menu_component_num(),idx_art));
    else {
    sprintf(str, "Val:%d   ",newPosition);
    lcd.setCursor(3,2);
    lcd.print(str);
    sprintf(str, "SW:%d  ", ms.get_current_menu());
    lcd.setCursor(0,3);
    lcd.print(str);
    }
  } 
}
