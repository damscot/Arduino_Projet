#define PUMP_PIN 5
#define TANK_FULL_PIN 2

#define PUMP_MAX 255
#define PUMP_NORMAL 255 // can be use to reduce power/noise
#define PUMP_STOP 0
#define FORCE_PUMP_WAKEUP 60*60*6 // every 6 hours
#define TANK_FULL_STATE false // Pin is 0 when full
unsigned long delaycount = FORCE_PUMP_WAKEUP;

typedef enum {
  STATE_NORMAL = 0,
  STATE_TANK_FULL,
  STATE_TANK_READY,
} statem_t;

statem_t sm;
statem_t next_sm;

void setup() {
  Serial.begin (115200);
  pinMode(TANK_FULL_PIN, INPUT);
  digitalWrite(TANK_FULL_PIN, HIGH);
  pinMode(PUMP_PIN, OUTPUT);
  digitalWrite(PUMP_PIN, LOW);
  analogWrite(PUMP_PIN, PUMP_STOP);
  Serial.println("Tank pump");
  Serial.println("*********");
  next_sm = sm = STATE_NORMAL;
  attachInterrupt(digitalPinToInterrupt(TANK_FULL_PIN), tank_full_isr, CHANGE);
}

void tank_full_isr()
{ 
  bool first, second, third;

  first = digitalRead(TANK_FULL_PIN);
  second  = digitalRead(TANK_FULL_PIN);
  third  = digitalRead(TANK_FULL_PIN);

  if ((first == second) and (first == third) and (first == false)) {
      Serial.println("Tank Full ISR Detected");
      next_sm = STATE_TANK_FULL;
  } else if ((first == second) and (first == third) and (first == true)) {
      Serial.println("Tank Empty ISR Detected");
      next_sm = STATE_TANK_READY;
  } else {
    Serial.println("Tank ISR Suprious");
    next_sm = sm; // Keep last state
  }
  
}

void loop() {

  // evaluate state machine once per second.
  delay(1000);
  
  if (next_sm != sm) {
    // Update state machine
    sm = next_sm;
  } else {
    if (sm == STATE_NORMAL) {
      if (--delaycount == 0) {
        Serial.println("** STATE = Normal (Regular Process) **");
        delaycount = FORCE_PUMP_WAKEUP;
        analogWrite(PUMP_PIN, PUMP_MAX);
        Serial.println("Wakeup pump for 2s");
        delay(2000);
        while (digitalRead(TANK_FULL_PIN) == TANK_FULL_STATE) {
          Serial.println("Warning Tank Still Full");
          analogWrite(PUMP_PIN, PUMP_MAX);
          delay(10000); // Force 10s pump before reevaluating
        }
        analogWrite(PUMP_PIN, PUMP_STOP);
      } else {
        Serial.print("** STATE = Normal (Count Down : ");
        Serial.print(delaycount);
        Serial.println(") **");
      }
    } else if (sm == STATE_TANK_FULL) {
      Serial.println("** STATE = Tank Full **");
      analogWrite(PUMP_PIN, PUMP_NORMAL);
      delay(30000); // 30s pumping water out
      if (digitalRead(TANK_FULL_PIN) == TANK_FULL_STATE) {
        Serial.println("Warning Tank Still Full");
        next_sm = STATE_TANK_FULL;
      }
      else
        next_sm = STATE_TANK_READY;
    } else if (sm == STATE_TANK_READY) {
      Serial.println("** STATE = Tank Ready **");
      analogWrite(PUMP_PIN, PUMP_STOP);
      if (digitalRead(TANK_FULL_PIN) == TANK_FULL_STATE) {
        Serial.println("Warning Tank Again Full");
        next_sm = STATE_TANK_FULL;
      }
      else
        next_sm = STATE_NORMAL;
    } else {
      Serial.print("Unknown State");
      next_sm = STATE_NORMAL;
    }
  }

}
