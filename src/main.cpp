#include "common.h"
#include "ble.h"
#include "my_wifi.h"
#include "3461as.h"

#define random(x) (esp_random() % x)
#define BLUE_LED_PIN 2
#define BUTTON_PIN 15
#define EEPROM_SIZE 64
#define BLINK_TIME 6

WiFiClass WiFi;
Preferences *pref;
int steps = 12345, last = -1;
int targets = 10000;
char connectedDevices = 0;
char addPerTime = 0;
char led = 0, bled = 0;
char isPressing = false;
uint16_t count = 0, generating = 0, buttonPress = 0;
char g1 = 0, g2 = 0, g3 = 0, g4 = 0;

uint8_t split_digit(int in, int digit, char fix = 1) {
  while (--digit) {
    in /= 10;
  }
  if (fix) {
    // dirty fix: if return zero and make it divided, kernel will panic
    return (in % 10 == 0) ? 9 : in % 10;
  }
  return in % 10;
}

void setup() {
  Serial.begin(115200);


  Serial.println("- Reading preferences from EEPROM...");
  pref = new Preferences();
  pref->begin("sport", false);
  // validate targets
  targets = pref->getInt("targets", 0);
  Serial.print("- targets from eeprom is "); Serial.println(targets);
  if (targets <= 10000 || targets > 100000) {
    Serial.println("- [!] Data is broken. Reset to 19999.");
    targets = 19999;
    pref->putInt("targets", targets);
  }
  // validate addPerTime
  addPerTime = pref->getChar("addPerTime", 0);
  Serial.print("- addPerTime from eeprom is "); Serial.println(addPerTime, DEC);
  if (addPerTime <= 0 || addPerTime >= 10) {
    Serial.println("- [!] Data is broken. Reset to 1.");
    addPerTime = 1;
    pref->putChar("addPerTime", addPerTime);
  }
  // validate last steps
  last = pref->getInt("last", -1);
  Serial.print("- last steps from eeprom is "); Serial.println(last, DEC);

  init_3461as();
  show_char_wait(500, 'Z', 'J', 'y', '.');
  show_char_wait(500, 'S', 't', 'E', 'P');
  show_char_wait(500, '6', '6', '6', '6');
  show_char_wait(500, ' ', ' ', ' ', ' ');

  if (last < 0) generating = 1; else steps = last;

	init_ble();
	WiFi = init_wifi();
	pinMode(BLUE_LED_PIN, OUTPUT);
  pinMode(BUTTON_PIN, INPUT);
	Serial.println("- [OK] ESP32 has been successfully started.");
}

void loop() {
  // delay 4ms each `show_char`, 1 * count == 4ms
  // count up to 65536 -> 0
  count++;
  // 10 * 4ms = 40ms
  if (count % 10 == 0) {
    // generating animation stop
    if (generating == 25) {
      generating = 0;
      steps = g1 * 10000 + g2 * 1000 + g3 * 100 + g4 * 10 + random(10);
    }
    // generating animating
    if (generating > 0) {
      if (generating < 4 * BLINK_TIME) g4 = random(10);
      else if (generating == 4 * BLINK_TIME) g4 = random(split_digit(targets, 2) + 1);
      if (generating < 3 * BLINK_TIME) g3 = random(10);
      else if (generating == 3 * BLINK_TIME) g3 = random(split_digit(targets, 3) + 1);
      if (generating < 2 * BLINK_TIME) g2 = random(10);
      else if (generating == 2 * BLINK_TIME) g2 = random(split_digit(targets, 4) + 1);
      if (generating < 1 * BLINK_TIME) g1 = random(10);
      else if (generating == 1 * BLINK_TIME) {
        // first digit is fixed to 1, or random from 1 to the 5th digit of target
        uint8_t t = split_digit(targets, 5);
        if (t <= 1) g1 = 1;
        // no 0
        else g1 = random(t) + 1;
      }
      generating++;
    }
    // blue led blink
    if (connectedDevices) {
      bled = !bled;
      digitalWrite(BLUE_LED_PIN, bled);
    } else {
      digitalWrite(BLUE_LED_PIN, LOW);
    }
  }

  // 4ms * 5 = 20ms
  if (count % 5 == 0) {
    // check button status
    isPressing = digitalRead(BUTTON_PIN);
    buttonPress += isPressing;
    if (!isPressing) {
      // 50 * 20ms = 1s
      if (buttonPress > 0 && buttonPress < 50) {
        // 0s < press time < 1s, re-generate
        Serial.println("- Regenerating...");
        show_char_wait(500, 'r', 'E', 'F', '-');
        pref->putInt("last", -1);
        generating = 1;
      } else if (buttonPress >= 50) {
        // press time >= 1s, save data
        Serial.println("- Saving data...");
        pref->putInt("last", steps);
        show_char_wait(500, 'S', 'A', 'V', 'E');
      }
      buttonPress = 0;
    }
  }

  // 250 * 4ms = 1s
  if (count % 250 == 0) {
    steps += addPerTime * led;
    Serial.print("Set steps to ");
    Serial.println(steps);
    led = !led;
  }

  // 1250 * 4ms = 5s
  if (count % 1250 == 0) {
  	if (!generating) {
      if (connectedDevices > 0) {
        uint8_t pedo[4] = {0x01, steps << 16 >> 16, steps << 8 >> 16, steps >> 16};
        pChar_pedo->setValue(pedo, 4);
    	  pChar_pedo->notify();
        Serial.print("Notifying steps: ");
    	  Serial.println(steps);
      } else
        Serial.println("No connected devices, do not notify.");
    }
  }

  if (generating) show_char(g1, g2, g3, g4, led);
  else show_char(split_digit(steps, 5, 0), split_digit(steps, 4, 0), split_digit(steps, 3, 0), split_digit(steps, 2, 0), led);
}
