#include <Encoder.h>

#define ENCFORW_PIN 3
#define ENCBACKW_PIN 2
#define ENCSW_PIN 4
#define SW0_PIN 5
#define SW1_PIN 6

Encoder Enc(ENCFORW_PIN, ENCBACKW_PIN);
long oldPosition  = 0;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  Serial.print("Basic Encoder");
  digitalWrite(ENCSW_PIN, HIGH);
  digitalWrite(SW0_PIN, HIGH);
  digitalWrite(SW1_PIN, HIGH);
}

void loop() {
  // put your main code here, to run repeatedly:
  long newPosition = (Enc.read())/4;
  if (newPosition != oldPosition) {
    Serial.println(newPosition);
    oldPosition = newPosition;
  }
}
