// ---------------------------------------------------------------------------
// Example NewPing library sketch that does a ping about 20 times per second.
// ---------------------------------------------------------------------------

#include <NewPing.h>
#include <Wire.h> 
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x3F,20,4);  // set the LCD address to 0x27 for a 16 chars and 2 line display

#define TRIGGER_PIN  3  // Arduino pin tied to trigger pin on the ultrasonic sensor.
#define ECHO_PIN     2  // Arduino pin tied to echo pin on the ultrasonic sensor.
#define MAX_DISTANCE 300 // Maximum distance we want to ping for (in centimeters). Maximum sensor distance is rated at 400-500cm.

NewPing sonar(TRIGGER_PIN, ECHO_PIN, MAX_DISTANCE); // NewPing setup of pins and maximum distance.

void setup() {
  //Serial.begin(115200); // Open serial monitor at 115200 baud to see ping results.
  // initialize the lcd 
  lcd.init();
  lcd.init();
  // Print a message to the LCD.
  lcd.backlight();
  lcd.setCursor(3,0);
  lcd.print("Mesure Distance");
}

void loop() {
  char str[20];
  int i;
  int dist = 0;
  for (i = 0; i < 20; i++) {
    delay(50);
    dist += sonar.ping_cm();
  }
  
  dist = dist / 20;
  if (dist == 0) {

    lcd.setCursor(7,1);
    sprintf(str, "stuck...",dist);
    lcd.print(str);
    delay(100);
    pinMode(ECHO_PIN, OUTPUT);
    digitalWrite(ECHO_PIN, LOW);
    delay(100);
    pinMode(ECHO_PIN, INPUT);
  }
  else {
    lcd.setCursor(7,1);
    sprintf(str, "%d cm    ",dist);
    lcd.print(str);
  }
}
