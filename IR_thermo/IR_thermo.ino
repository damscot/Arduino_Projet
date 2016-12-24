#include <Wire.h>
#include <Adafruit_MLX90614.h>
#include <LiquidCrystal_I2C.h>
#include "U8glib.h"

#define LASER_PIN     6
#define BUTTON_PIN    12  

uint8_t count;
uint8_t laser_strength[4] = {0,1,32,255};
char str[20]="";
char str2[20]="";
char str3[20]="";

unsigned long time2 = 0;
uint8_t skip_next = 0;

int16_t amb;
int16_t obj;
int16_t amb_sav=0;
int16_t obj_sav=0;

Adafruit_MLX90614 mlx = Adafruit_MLX90614();
LiquidCrystal_I2C lcd(0x3F,20,4);  // set the LCD address to 0x27 for a 16 chars and 2 line display
U8GLIB_SSD1306_128X64 u8g(U8G_I2C_OPT_NONE|U8G_I2C_OPT_DEV_0);	// I2C / TWI

void setup() {
  Serial.begin(115200);
  pinMode(BUTTON_PIN, INPUT);
  digitalWrite(BUTTON_PIN, HIGH);
  
  // initialize the lcd 
  lcd.init();
  lcd.init();
  // Print a message to the LCD.
  lcd.backlight();
  lcd.setCursor(0,0);
  lcd.print("==   IR  THERMO   ==");
  count = 0; 

  mlx.begin();  
}
 
void draw(char * str, char * str2, char * str3) {
  // graphic commands to redraw the complete screen should be placed here  
  u8g.setFont(u8g_font_unifont);
  u8g.setPrintPos(0, 12); 
  u8g.print("== IR  THERMO ==");
  u8g.setPrintPos(0, 30); 
  u8g.print(str);
  u8g.setPrintPos(0, 45); 
  u8g.print(str2);
  u8g.setPrintPos(0, 60); 
  u8g.print(str3);
}

void loop() {
  unsigned long time = millis();
  
  if ((digitalRead(BUTTON_PIN) == 0) && (skip_next == 0)) {
    delay(200);
    if (digitalRead(BUTTON_PIN) == 0) {
      delay(1000);
      Serial.print("SAVING");
      amb_sav = amb;
      obj_sav = obj;
      skip_next = 1;
    }
    else {
      count++;
      if (count >= 4) {
        count = 0;
      }
      Serial.print("LASER MODE = ");
      Serial.print(count);
      Serial.println();
    }
  }
  else {
    if ((digitalRead(BUTTON_PIN) == 1) && (skip_next == 1)) {
      delay(100);
      skip_next = 0;
    }
  }
  
  if (((time - time2) > 500) || (time2 > time)) {
    time2 = time;
    amb = (mlx.readAmbientTempC())*10;
    obj = (mlx.readObjectTempC())*10;
    
    snprintf(str,20," %03d.%d  |  %03d.%d ",amb/10, amb%10, obj/10, obj%10);
    snprintf(str2,20," %03d.%d  |  %03d.%d ",amb_sav/10, amb_sav%10, obj_sav/10, obj_sav%10);
    snprintf(str3,20,"%c%03d.%d  | %c%03d.%d",((amb-amb_sav<0) ? '-':'+'), abs((amb-amb_sav)/10), abs((amb-amb_sav)%10), ((obj-obj_sav<0) ? '-':'+'), abs((obj-obj_sav)/10), abs((obj-obj_sav)%10));
    Serial.print(str);
    Serial.println();
    Serial.print(str2);
    Serial.println();
    Serial.print(str3);
    Serial.println();
    lcd.setCursor(0,1);
    lcd.print(str);
    lcd.setCursor(0,2);
    lcd.print(str2);
    lcd.setCursor(0,3);
    lcd.print(str3);
    analogWrite(LASER_PIN, laser_strength[count]);
    Serial.println();
  }
  
  u8g.firstPage();  
  do {
    draw(str,str2,str3);
  } while( u8g.nextPage() );
  
}
