#include "common.h"

#define DISPLAY_BRIGHTNESS 10

#define PIN_DIGIT_1 16
#define PIN_DIGIT_2 23
#define PIN_DIGIT_3 22
#define PIN_DIGIT_4 33
uint8_t pindigit[] = {PIN_DIGIT_1, PIN_DIGIT_2, PIN_DIGIT_3, PIN_DIGIT_4};

#define PIN_A 17
#define PIN_B 32
#define PIN_C 26
#define PIN_D 14
#define PIN_E 12
#define PIN_F 13
#define PIN_G 25
#define PIN_DP 27
uint8_t pinseg[] = {PIN_A, PIN_B, PIN_C, PIN_D, PIN_E, PIN_F, PIN_G, PIN_DP};

void init_3461as() {
  pinMode(PIN_DIGIT_1, OUTPUT);
  pinMode(PIN_DIGIT_2, OUTPUT);
  pinMode(PIN_DIGIT_3, OUTPUT);
  pinMode(PIN_DIGIT_4, OUTPUT);
  pinMode(PIN_A, OUTPUT);
  pinMode(PIN_B, OUTPUT);
  pinMode(PIN_C, OUTPUT);
  pinMode(PIN_D, OUTPUT);
  pinMode(PIN_E, OUTPUT);
  pinMode(PIN_F, OUTPUT);
  pinMode(PIN_G, OUTPUT);
  pinMode(PIN_DP, OUTPUT);
  Serial.println("- [OK] 3461AS driver written by ZJY.");
}

uint8_t b(uint8_t n1, uint8_t n2, uint8_t n3, uint8_t n4, uint8_t n5, uint8_t n6, uint8_t n7, uint8_t n8) {
  return (uint8_t) ((n1 << 7) + (n2 << 6) + (n3 << 5) + (n4 << 4) + (n5 << 3) + (n6 << 2) + (n7 << 1) + n8);
}

void show_on_3461as(uint8_t which, char num, uint8_t dp = 0) {
  uint8_t flag = 0;
  switch (num) {
    case 0: case '0': case 'D': case 'O':
      flag = b(1, 1, 1, 1, 1, 1, 0, 0); break;
    case 1: case '1': case 'I': case 'i': case 'l':
      flag = b(0, 1, 1, 0, 0, 0, 0, 0); break;
    case 2: case '2': case 'Z': case 'z':
      flag = b(1, 1, 0, 1, 1, 0, 1, 0); break;
    case 3: case '3':
      flag = b(1, 1, 1, 1, 0, 0, 1, 0); break;
    case 4: case '4': case 'Y':
      flag = b(0, 1, 1, 0, 0, 1, 1, 0); break;
    case 5: case '5': case 'S': case 's':
      flag = b(1, 0, 1, 1, 0, 1, 1, 0); break;
    case 6: case '6': case 'G':
      flag = b(1, 0, 1, 1, 1, 1, 1, 0); break;
    case 7: case '7':
      flag = b(1, 1, 1, 0, 0, 0, 0, 0); break;
    case 8: case '8': case 'B':
      flag = b(1, 1, 1, 1, 1, 1, 1, 0); break;
    case 9: case '9': case 'g':
      flag = b(1, 1, 1, 1, 0, 1, 1, 0); break;
    case 'A':
      flag = b(1, 1, 1, 0, 1, 1, 1, 0); break;
    case 'b':
      flag = b(0, 0, 1, 1, 1, 1, 1, 0); break;
    case 'C':
      flag = b(1, 0, 0, 1, 1, 1, 0, 0); break;
    case 'c':
      flag = b(0, 0, 0, 1, 1, 0, 1, 0); break;
    case 'd':
      flag = b(0, 1, 1, 1, 1, 0, 1, 0); break;
    case 'E':
      flag = b(1, 0, 0, 1, 1, 1, 1, 0); break;
    case 'e':
      flag = b(1, 1, 0, 1, 1, 1, 1, 0); break;
    case 'F':
      flag = b(1, 0, 0, 0, 1, 1, 1, 0); break;
    case 'H':
      flag = b(0, 1, 1, 0, 1, 1, 1, 0); break;
    case 'h':
      flag = b(0, 0, 1, 0, 1, 1, 1, 0); break;
    case 'J': case 'j':
      flag = b(0, 1, 1, 1, 0, 0, 0, 0); break;
    case 'L':
      flag = b(0, 0, 0, 1, 1, 1, 0, 0); break;
    case 'n':
      flag = b(0, 0, 1, 0, 1, 0, 1, 0); break;
    case 'o':
      flag = b(0, 0, 1, 1, 1, 0, 1, 0); break;
    case 'P': case 'p':
      flag = b(1, 1, 0, 0, 1, 1, 1, 0); break;
    case 'Q':
      flag = b(1, 1, 1, 1, 1, 1, 0, 1); break;
    case 'q':
      flag = b(1, 1, 1, 0, 0, 1, 1, 0); break;
    case 'r':
      flag = b(0, 0, 0, 0, 1, 0, 1, 0); break;
    case 't':
      flag = b(0, 0, 0, 1, 1, 1, 1, 0); break;
    case 'U': case 'V':
      flag = b(0, 1, 1, 1, 1, 1, 0, 0); break;
    case 'u': case 'v':
      flag = b(0, 0, 1, 1, 1, 0, 0, 0); break;
    case 'y':
      flag = b(0, 1, 1, 1, 0, 1, 1, 0); break;
    case '-':
      flag = b(0, 0, 0, 0, 0, 0, 1, 0); break;
    case '.':
      flag = b(0, 0, 0, 0, 0, 0, 0, 1); break;
  }
  if (dp) flag++;
  for (int i = 0; i < 8; i++) {
    uint8_t value = ((uint8_t) (flag << i) >> 7) ? HIGH : LOW;
    digitalWrite(pinseg[i], value);
  }
  uint8_t numpin = 0;
  switch (which) {
    case 1: // 1000
      numpin = 8; break;
    case 2: // 0100
      numpin = 4; break;
    case 3: // 0010
      numpin = 2; break;
    case 4: // 0001
      numpin = 1;
  }
  for (int i = 0; i < 4; i++) {
    uint8_t value = ((uint8_t) (numpin << (i + 4)) >> 7) ? LOW : HIGH;
    digitalWrite(pindigit[i], value);
  }
}


void show_char(char c1, char c2, char c3, char c4, uint8_t dotbit = 0) {
  show_on_3461as(1, c1, dotbit & 8);
  delay(1);
  show_on_3461as(2, c2, dotbit & 4);
  delay(1);
  show_on_3461as(3, c3, dotbit & 2);
  delay(1);
  show_on_3461as(4, c4, dotbit & 1);
  delay(1);
}

void show_char_wait(int delay, char c1, char c2, char c3, char c4) {
  delay /= 4;
  while (delay--) {
    show_char(c1, c2, c3, c4);
  }
}


void show_num(uint16_t num) {
  uint8_t n1 = num / 1000;
  uint8_t n2 = (num - n1 * 1000) / 100;
  uint8_t n3 = (num - n1 * 1000 - n2 * 100) / 10;
  uint8_t n4 = num - n1 * 1000 - n2 * 100 - n3 * 10;
  show_char(n1, n2, n3, n4);
}
