/*
 * Example program to count the number of times a door is 
 * opened and closed using an ultrasonic sensor on the door.
 * Also includes a 7-segment display counter and status LEDs
 * to show the status of the door.
 * 
 */
#include <NewPing.h>
#include <CompileTime.h>
using namespace CompileTime;

#define   TRIGGER_PIN   11
#define   ECHO_PIN      10
#define   RED_LED_PIN   A0
#define   GRN_LED_PIN   A1
#define   MAX_DISTANCE  50
#define   TRG_DISTANCE  20

// Global variables
NewPing sonar(TRIGGER_PIN, ECHO_PIN, MAX_DISTANCE);

// segment pin numbers
int const sevseg_pins[7] = {
//a, b, c, d, e, f, g
  2, 3, 4, 5, 6, 7, 8
};

// 7 segment bit patterns for digits 0-9
uint8_t const digits[10] = {
//   abcdefg
  0b01111110, // '0'
  0b00110000, // '1'
  0b01101101, // '2'
  0b01111001, // '3'
  0b00110011, // '4'
  0b01011011, // '5'
  0b01011111, // '6'
  0b01110000, // '7'
  0b01111111, // '8'
  0b01111011  // '9'
};

int lstState = 0;
int lstCount = 0;
int count = 0;
int ppl = 0;

// update the 7-segment display with the given number
void update_display(uint8_t const value) {
    // get the but pattern for this number from the digits[] array
    uint8_t bits = digits[value];

    // for each segment, set it to the next bit in the pattern
    for (int segment=0; segment < 7; segment++) {
      // 0x40 = 01000000
        digitalWrite(sevseg_pins[segment], bits & 0x40);

        // shift the pattern one bit to the left to get the next
        // segment in position 0b01000000 (0x40 in hex)
        bits <<= 1;
    }
}

void setup() {
  // sync our time with the PC
  setCompileTime(9);

  // make the segment pins all outputs
  for (int pin : sevseg_pins) {
    pinMode(pin, OUTPUT);
  }

  // make the LED pins outputs
  pinMode(RED_LED_PIN, OUTPUT);
  pinMode(GRN_LED_PIN, OUTPUT);

  // start the serial output
  Serial.begin(115200);

  //1st time displays 0
  update_display(ppl);
}

void loop() {
  updateTime(micros());

  int dist = sonar.ping_cm();
  delay(100);

  // decide if state is true or false
  int crtState = (dist < TRG_DISTANCE && dist != 0);

  // update LED status's
  digitalWrite(RED_LED_PIN,  crtState);
  digitalWrite(GRN_LED_PIN, !crtState);

  // check for trigger state change
  if (crtState != lstState) {
    lstState = crtState;

    // if set, increase the trigger count
    if (crtState) {
      count++;

      // set 1 trigger to 1 people, 2 to 1, 3 to 2, 4 to 2 and so on
      ppl = (count+1)/2;
    }
  }

  // see if trigger count has changed
  if (lstCount != count) {
    // keep the last value in sync
    lstCount = count;
 
  // format a string with the people count and the current time:
    char buff[64] = "";
    sprintf(buff, "%d people, door triggered at %s%2d %d - %2d:%02d:%02d",
            ppl, month, day, year, hour, minute, second);

    // output the string to the monitor window:
    Serial.println(buff);

    // update the 7-segment display with the people count:
    update_display(ppl%10);
  }
}