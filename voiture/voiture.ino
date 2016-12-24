#include <Arduino.h>
#undef RH_ASK_PTT_PIN
#include <RH_ASK.h>
#include <SPI.h>
//#include <Servo.h>
#include <Adafruit_SoftServo.h>

//Servo dir;
Adafruit_SoftServo dir;

#define FORWARD_PIN       5
#define BACKWARD_PIN      6
#define DIRECTION_PIN     9

#define CENTER_POS       80
#define LEFT_MIN_POS     55
#define LEFT_MAX_POS     20
#define RIGHT_MIN_POS    105
#define RIGHT_MAX_POS    140

#define SPEED_NULL       0
#define SPEED_VERYSLOW   10
#define SPEED_SLOW       160
#define SPEED_FAST       255

#define BACK_SPEED_NULL       0
#define BACK_SPEED_VERYSLOW   75
#define BACK_SPEED_SLOW       85
#define BACK_SPEED_FAST       95

RH_ASK RH_433 = RH_ASK (2000, 2, 3, -1, false);

uint8_t forward = SPEED_NULL;
uint8_t backward = BACK_SPEED_NULL;
uint8_t left_right = CENTER_POS;
unsigned long time, time2;

#define SERVO_MS_UPDATE    20
#define SLEEP_COUNT        (1000/SERVO_MS_UPDATE)

int sleep_cnt = SLEEP_COUNT;

void setup() {
  
  Serial.begin (115200);
  dir.attach(DIRECTION_PIN);
  dir.write(CENTER_POS);
  dir.refresh();
  
  pinMode(FORWARD_PIN, OUTPUT);
  analogWrite(FORWARD_PIN, SPEED_NULL);
  pinMode(BACKWARD_PIN, OUTPUT);
  analogWrite(BACKWARD_PIN, SPEED_NULL);
  
  Serial.println ("Radiohead 433 initialization");
  if (!RH_433.init ())
    Serial.println ("Radiohead 433 init failed");
}

void loop() {
  
  uint8_t buf[8];
  uint8_t buflen = sizeof(buf);
  int serial;
  String str="";
  
  if(Serial.available() > 0)
  {
    str = Serial.readStringUntil('\n');
    Serial.println(str);
    str.toCharArray((char *)(buf), (unsigned int)(buflen));
  }
  
  if ((str.length() > 0) || (RH_433.recv (buf, &buflen)))
  {
    Serial.print("Receive Message : ");
    Serial.println((char *)(buf));
    if ((buf[0]=='V') && (buf[1]=='M')) {
      int x = (buf[2]-'0')*10 + (buf[3]-'0');
      int y = (buf[4]-'0')*10 + (buf[5]-'0');
      
      if (x < 45) {
        forward = map(x, 0, 45, SPEED_FAST, SPEED_VERYSLOW);
        backward = BACK_SPEED_NULL;
      } else if (x > 55) {
        backward = map(x, 55, 99, BACK_SPEED_VERYSLOW, BACK_SPEED_FAST);
        forward = SPEED_NULL;
      } else {
        forward = SPEED_NULL;
        backward = BACK_SPEED_NULL;
      }

      left_right = map(y, 0, 99, LEFT_MAX_POS, RIGHT_MAX_POS);
      
      Serial.println(forward);
      Serial.println(backward);
      //Serial.println(left_right);
    }
    else {
      Serial.print("Unknown Message : ");
      for (int j = 0 ; j < sizeof(buf) ; j++) {
        char x = buf[j];
        Serial.print(x);
      }
      Serial.print("\n");
      left_right = CENTER_POS;
      forward = SPEED_NULL;
      backward = BACK_SPEED_NULL;
    }
    sleep_cnt = SLEEP_COUNT;
    time = millis();
  }
  
  time2 = millis();
  if ((time2 > time) && ((time2 - time) > SERVO_MS_UPDATE)) {
    if (sleep_cnt-- == 0) {
      //Serial.print("Nothing received... stop\n");
      left_right = CENTER_POS;
      forward = SPEED_NULL;
      backward = BACK_SPEED_NULL;
      sleep_cnt = SLEEP_COUNT;
    }
    time = millis();
    dir.refresh();
  }
  
  dir.write(left_right);
  analogWrite(BACKWARD_PIN, backward);
  analogWrite(FORWARD_PIN, forward);
  
}
