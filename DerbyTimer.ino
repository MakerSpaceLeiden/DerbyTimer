#include <MD_MAX72xx.h>
#include <SPI.h>
#include <Servo.h>

// 4 times 8x8 MAX 7216 chaisy chained diplays
#define HARDWARE_TYPE MD_MAX72XX::FC16_HW
#define MAX_DEVICES 4
#define CLK_PIN   13  // or SCK (13)
#define DIN_PIN  11  // or MOSI (11)
#define CS_PIN    10  // or SS (10)

#define SERVO_OUT_PIN     (5) /* 1/Groen op stekker --  PWM to start servo */
#define BUTTON_IN_PIN     (4) /* 4/Wit op de stekker -- req. pull up */
#define BUZZER_OUT_PIN    (3)
#define DETECTOR_IN_PIN   (2) /* From photo cell */

#define HOLD_POS           90
#define START_POS           0
#define END_POS           120
#define START_HOLD_DELAY 2000   /* 2 second */

MD_MAX72XX mx = MD_MAX72XX(HARDWARE_TYPE, CS_PIN, MAX_DEVICES);
Servo startServo;

enum { WAIT = 0, WAITING, READY, SET, GO, RACE, HOLD, RESET } state = WAIT, ostate = HOLD;

void setup()
{

  Serial.begin(115200);
  Serial.print("\n\n\nStarting: "__FILE__ "\n" __DATE__ "\n" __TIME__"\n");

  mx.begin();
  startServo.attach(SERVO_OUT_PIN);
  startServo.write(HOLD_POS);

  pinMode(BUTTON_IN_PIN, INPUT_PULLUP);
  pinMode(DETECTOR_IN_PIN, INPUT);
  pinMode(BUZZER_OUT_PIN, OUTPUT);
  digitalWrite(BUZZER_OUT_PIN, 1);

  Serial.println("Starting loop & normal ops\n");
}


unsigned int last_change, last_move, last_start = 0;
bool moving;
const char buff[32];

void loop()
{
  // We're using the wrong type of servo - the continues running
  // type. So we need to switch it 'off' fairly accurately after
  // the right number of milli seconds. So we do this in the main
  // loop - and try to be careful to kick it off late in the state
  // changes.
  //
  if (moving && millis() - last_move > 200) {
    startServo.write(HOLD_POS);
    moving = false;
  }
  if (state != ostate) {
    last_change = millis();
    ostate = state;
  };
  switch (state) {
    case WAIT: printText(0, MAX_DEVICES - 1, "waiting");
      Serial.println("Waiting for car...");
      state = WAITING;
      last_change = millis();
      break;
    case WAITING:
      if (digitalRead(BUTTON_IN_PIN) == LOW) {
        state = READY;
        printText(0, MAX_DEVICES - 1, "READY");
        Serial.println("Count down...");
      };
      break;
    case READY:
      if (millis() - last_change > START_HOLD_DELAY) {
        state = SET;
        printText(0, MAX_DEVICES - 1, "   SET");
        Serial.println("SET !");
      };
      break;
    case SET:
      if ((millis() - last_change) % 600  < 200)
        digitalWrite(BUZZER_OUT_PIN, 0);
      else
        digitalWrite(BUZZER_OUT_PIN, 1);

      if (millis() - last_change > START_HOLD_DELAY) {
        state = RACE;
        printText(0, MAX_DEVICES - 1, "    GO!");
        last_start = millis();
        Serial.println("Racing !");
        startServo.write(START_POS); moving = true; last_move = millis();
        digitalWrite(BUZZER_OUT_PIN, 0);
      };
      break;
    case RACE:
      if (digitalRead(DETECTOR_IN_PIN) == HIGH &&  millis() - last_start > 500) {
        state = HOLD;
        startServo.write(END_POS); moving = true; last_move = millis();
      }

      if ((millis() - last_change > 100 && millis() - last_start > 500) || (state == HOLD)) {
        digitalWrite(BUZZER_OUT_PIN, 1);
        unsigned long l = millis() - last_start;
        int s = l / 1000;
        int m = s / 60;
        sprintf(buff, "%02d.%03d", s % 60, l % 1000);
        printText(0, MAX_DEVICES - 1, buff);
        if (state == HOLD) {
          Serial.print("Race finished - time: ");
          Serial.println(buff);
        };
      }

      if (digitalRead(BUTTON_IN_PIN) == LOW &&  millis() - last_start > 1000) {
        printText(0, MAX_DEVICES - 1, "Abort!");
        delay(1000);
        state = RESET;
      };
      break;
    case HOLD:
      if (digitalRead(BUTTON_IN_PIN) == LOW) {
        Serial.println("\nResetting for next race.");
        state = RESET;
      }
      break;
    case RESET:
      if (digitalRead(BUTTON_IN_PIN) == HIGH) {
        state = WAIT;
      }
      break;
  };
};
