/* Voiture Télécommandé numéro 0 */
/* Nikko VALOR */
#define VOITURE_ID 0

/*Radiohead libraries parameters */
#define RH_ASK_ARDUINO_USE_TIMER2 // ne fonctionne pas a ce niveau makefile manuelle ou forcage dans la lib (conflit avec lib servo)!!
#define RH24

#ifdef RH33
#include <RH_ASK.h>
#undef RH_ASK_PTT_PIN
#endif
#ifdef RH24
#include <RH_NRF24.h>
#endif
#include <Arduino.h>
#include <OneWire.h>
#include <SPI.h>
#include <Wire.h>

/*Soft servo or PWM driven servo*/
//#define SOFTSERVO
#ifdef SOFTSERVO
#include <Adafruit_SoftServo.h>
Adafruit_SoftServo dir;
#else
#include <Servo.h>
Servo dir;
#endif

#define DS_PIN 8

#define CE_PIN 4
#define SS_PIN 10
#define MOSI_PIN 11
#define MISO_PIN 12
#define SCK_PIN 13

#define FORWARD_PIN       5
#define BACKWARD_PIN      3
#define DIRECTION_PIN     9

#define CENTER_POS       90
#define LEFT_MIN_POS     65
#define LEFT_MAX_POS     40
#define RIGHT_MIN_POS    115
#define RIGHT_MAX_POS    140

#define SPEED_NULL       0
#define SPEED_VERYSLOW   10
#define SPEED_SLOW       120
#define SPEED_FAST       220 //255

#define BACK_SPEED_NULL       0
#define BACK_SPEED_VERYSLOW   10
#define BACK_SPEED_SLOW       85
#define BACK_SPEED_FAST       204 //200

#ifdef RH33
RH_ASK RH_433 = RH_ASK (2000, 2, 3, -1, false);
#endif

#ifdef RH24
RH_NRF24 RH_24 = RH_NRF24 (CE_PIN, SS_PIN);
#endif


OneWire ds(10);  // on pin 10

uint8_t forward = SPEED_NULL;
uint8_t backward = BACK_SPEED_NULL;
uint8_t left_right = CENTER_POS;
unsigned long time, time2;
uint8_t network_address[5] = {VOITURE_ID,VOITURE_ID,VOITURE_ID,VOITURE_ID,VOITURE_ID};

int erreur=0;

#define SERVO_MS_UPDATE    20
#define SLEEP_COUNT        (1000/SERVO_MS_UPDATE)

int sleep_cnt = SLEEP_COUNT;

void setup() {
  
  Serial.begin (115200);
  dir.attach(DIRECTION_PIN);
  dir.write(CENTER_POS);
#ifdef SOFTSERVO
  dir.refresh();
#endif  
  pinMode(FORWARD_PIN, OUTPUT);
  analogWrite(FORWARD_PIN, SPEED_NULL);
  pinMode(BACKWARD_PIN, OUTPUT);
  analogWrite(BACKWARD_PIN, SPEED_NULL);

  Serial.print("Radiohead ");
  Serial.print(RH_VERSION_MAJOR);
  Serial.print(".");
  Serial.print(RH_VERSION_MINOR);
  Serial.println();

#ifdef RH33
  Serial.println ("Radiohead 433 initialization");
  if (!RH_433.init ()) {
    Serial.println ("Radiohead 433 init failed");
    erreur++;
  }
#endif

#ifdef RH24
  Serial.println ("Radiohead nRF24L01 initialization");
  if (!RH_24.init()) {
    Serial.println("Radiohead NRF24 init failed");
    erreur++;
  }
  // Defaults after init are 2.402 GHz (channel 1), 2Mbps, 0dBm
  if (!RH_24.setChannel(2))
    Serial.println("setChannel failed");
  if (!RH_24.setRF(RH_NRF24::DataRate2Mbps, RH_NRF24::TransmitPower0dBm))
    Serial.println("setRF failed");
  if (!RH_24.setNetworkAddress(network_address, sizeof(network_address)))
    Serial.println("setNet failed");
#endif
}

void loop() {
  
  uint8_t buf[8];
  uint8_t buflen = sizeof(buf);
  int serial;
  bool rcv=false;
  String str="";

  while(erreur) {};
  
  if(Serial.available() > 0)
  {
    str = Serial.readStringUntil('\n');
    Serial.println(str);
    str.toCharArray((char *)(buf), (unsigned int)(buflen));
  }

#ifdef RH33
  if (RH_433.recv (buf, &buflen)) // Non-blocking
  {
    rcv++;
    //Serial.print("RH_433 received Buf:");
    //Serial.println((char *)(buf));
  }
#endif
  
#ifdef RH24
  RH_24.waitAvailableTimeout(0);
  if (RH_24.recv(buf, &buflen)) // Non-blocking
  {
    rcv++;
    //Serial.print("RH_24 received Buf:");
    //Serial.println((char *)(buf));
  }
#endif
  
  if ((str.length() > 0) || (rcv >0))
  {
    //Serial.print("Receive Message : ");
    //Serial.println((char *)(buf));
    if ((buf[0]=='V') && (buf[1]==('0' + VOITURE_ID))) {
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
      
      //Serial.println(forward);
      //Serial.println(backward);
      //Serial.println(left_right);
    }
    else {
      if (buf[0] !='V') {
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
    }
    sleep_cnt = SLEEP_COUNT;
    time = millis();
  }
  
  time2 = millis();
  if ((time2 > time) && ((time2 - time) > SERVO_MS_UPDATE)) {
    if (sleep_cnt-- == 0) {
      Serial.print("Nothing received... stop\n");
      left_right = CENTER_POS;
      forward = SPEED_NULL;
      backward = BACK_SPEED_NULL;
      sleep_cnt = SLEEP_COUNT;
    }
    time = millis();
#ifdef SOFTSERVO
    dir.refresh();
#endif
  }
  
  dir.write(left_right);
  analogWrite(BACKWARD_PIN, backward);
  analogWrite(FORWARD_PIN, forward);
  
}
