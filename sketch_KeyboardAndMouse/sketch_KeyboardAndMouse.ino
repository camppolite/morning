/*
arduino
*/
#include <math.h>
#include "Keyboard.h"
#include "AbsMouse.h"
// #include "Mouse.h"
#include <avr/wdt.h> /* Header for watchdog timers in AVR */
#include <DFPlayerMini_Fast.h>


// #if !defined(UBRR1H)
#include <SoftwareSerial.h>
SoftwareSerial mySerial(10, 11); // RX, TX
// #endif

DFPlayerMini_Fast myMP3;

char inData[60];  // Allocate some space for the string
int number_of_bytes_received;
bool readComplete = false;  // whether read is complete
bool work = true;
unsigned long timer = millis();

void delay_reset(int ms) {
  int t = 1500;
  if (ms >= t) {
    delay(t);
    wdt_reset();
    delay(ms - t);
  }
  else {
    delay(ms);
  }
}

int GetRand(int min, int max)
{
    static int Init = 0;
    int rc;

    if (Init == 0)
    {
      srand((unsigned)millis());
      Init = 1;
    }
    rc = (rand() % (max - min + 1) + min);

    return (rc);
}

double my_random(double x)
{
  srand((unsigned)millis());
  if (x < 1)
    return 0;
  return (double)((rand() % (int)x));
}

void keyboard_string(char *str) {
  for (int i = 0; i < strlen(str); i++) {
    Keyboard.write(str[i]);
    delay(25);
  }
}

void keyboard_input(char *str) {
  char* d = strtok(str, ":");
  d = strtok(NULL, "");
  keyboard_string(d);
}

void keyboard_alt_xxx(const uint8_t c) {
  Keyboard.press(KEY_LEFT_ALT);
  delay(10);
  Keyboard.write(c);
  // Serial.println(c);
  Keyboard.release(KEY_LEFT_ALT);
}

void hotkey_alt_xxx(const uint8_t c) {
  delay(GetRand(5,15));
  Keyboard.press(KEY_LEFT_ALT);
  delay(GetRand(15,30));
  Keyboard.write(c);
  delay(GetRand(15,30));
  Keyboard.release(KEY_LEFT_ALT);
  Serial.println("ok");
}

void keyboard_alt_fff(char *str) {
  Keyboard.press(KEY_LEFT_ALT);
  delay(10);
  if (strcmp("F4", str) == 0){
    Keyboard.press(KEY_F4);
  }
  Keyboard.releaseAll();
}

void hotkey_alt_fff(char *str) {
  delay(GetRand(5,15));
  Keyboard.press(KEY_LEFT_ALT);
  delay(GetRand(15,30));
  if (strcmp("F4", str) == 0){
    Keyboard.press(KEY_F4);
  }
  delay(GetRand(15,30));
  Keyboard.release(KEY_F4);
  delay(GetRand(5,15));
  Keyboard.release(KEY_LEFT_ALT);
  Serial.println("ok");
}

void alt_fff(char *str) {
  Keyboard.press(KEY_LEFT_ALT);
  uint8_t k = 0;
  if (strcmp("F3", str) == 0){
    k = KEY_F3;
  }
  else if (strcmp("F4", str) == 0){
    k = KEY_F4;
  }
  if (k != 0) {
    delay(GetRand(15,30));
    Keyboard.press(k);
    delay(GetRand(15,30));
    Keyboard.release(k);
    Keyboard.release(KEY_LEFT_ALT);
  }
  Serial.println("ok");
}

void win_xxx(char k) {
  Keyboard.press(KEY_LEFT_GUI);
  delay(GetRand(15,30));
  Keyboard.press((uint8_t)k);
  delay(GetRand(15,30));
  Keyboard.release((uint8_t)k);
  Keyboard.release(KEY_LEFT_GUI);
  Serial.println("ok");
}

void keyboard_ctrl_xxx(const uint8_t c) {
  Keyboard.press(KEY_LEFT_CTRL);
  delay(10);
  Keyboard.write(c);
  Keyboard.release(KEY_LEFT_CTRL);
}

void hotkey_ctrl_xxx(const uint8_t c) {
  Keyboard.press(KEY_LEFT_CTRL);
  delay(10);
  Keyboard.write(c);
  Keyboard.release(KEY_LEFT_CTRL);
  Serial.println("ok");
}

void hotkey_ctrl_fff(char *str) {
  delay(GetRand(5,15));
  Keyboard.press(KEY_LEFT_CTRL);
  delay(GetRand(15,30));
  if (strcmp("TAB", str) == 0){
    Keyboard.press(KEY_TAB);
    delay(GetRand(15,30));
    Keyboard.release(KEY_TAB);
  }
  delay(GetRand(5,15));
  Keyboard.release(KEY_LEFT_CTRL);
  Serial.println("ok");
}

void keyboard_press(const uint8_t k) {
  Keyboard.press(k);
  delay(5);
  Keyboard.release(k);
}

void keyboard_fight(const uint8_t k) {
  Keyboard.press(k);
  delay(GetRand(30,45));
  // delay(100);
  Keyboard.release(k);
  // delay(10);
}

void hotkey_press(const uint8_t k) {
  Keyboard.press(k);
  delay(GetRand(5,15));
  Keyboard.release(k);
}

void abs_mouse_click(char *str, uint8_t mode = MOUSE_LEFT) {
  char* d = strtok(str, ":");
  d = strtok(NULL, ",");
  const uint16_t x = (uint16_t)atoi(d);
  d = strtok(NULL, "");
  const uint16_t y = (uint16_t)atoi(d);
  AbsMouse.move(x, y);
  delay(GetRand(5,10));
  AbsMouse.press(mode);
  delay(GetRand(5,10));
  AbsMouse.release(mode);
}

void abs_mouse_move_liner(char *str) {
  char* d = strtok(str, ":");
  d = strtok(NULL, ",");
  const int cx = atoi(d);
  d = strtok(NULL, ",");
  const int cy = atoi(d);
  d = strtok(NULL, ",");
  const int x = atoi(d);
  d = strtok(NULL, "");
  const int y = atoi(d);
  short vector_x = x > cx ? 1 : -1;
  short vector_y = y > cy ? 1 : -1;
  uint32_t a = abs(x - cx);
  uint32_t b = abs(y - cy);
  double c = sqrt((double)(a * a + b * b));
  short step = 1;
  double step_a = (double)(a / c * step * vector_x);
  double step_b = (double)(b / c * step * vector_y);
  int n = 0;
  double x0 = cx;
  double y0 = cy;

  while (true) {
    n++;
    if (abs(x0 - x) <= step and abs(y0 - y) <= step or n >= 2000) break;
    x0 = x0 + step_a;
    y0 = y0 + step_b;
    AbsMouse.move((uint16_t)x0, (uint16_t)y0);
    // delay(9);
  }
  Serial.println(n < 2000);
}

void abs_mouse_click_liner(char *str) {
  char* d = strtok(str, ":");
  d = strtok(NULL, ",");
  const int x = atoi(d);
  d = strtok(NULL, ",");
  const int y = atoi(d);
  d = strtok(NULL, ",");
  const int near_x = atoi(d);
  d = strtok(NULL, ",");
  const int near_y = atoi(d);
  d = strtok(NULL, ",");
  const int fake_x = atoi(d);
  d = strtok(NULL, ",");
  const int fake_y = atoi(d);
  d = strtok(NULL, "");
  const uint8_t mode = (uint8_t)atoi(d);
  const int offset_x = fake_x - near_x;  // 漂移量
  const int offset_y = fake_y - near_y;  // 漂移量
  const int real_x = x - offset_x;
  const int real_y = y - offset_y;
  short vector_x = real_x > near_x ? 1 : -1;
  short vector_y = real_y > near_y ? 1 : -1;
  uint32_t a = abs(real_x - near_x);
  uint32_t b = abs(real_y - near_y);
  double c = sqrt((double)(a * a + b * b));
  short step = 1;
  double step_a = (double)(a / c * step * vector_x);
  double step_b = (double)(b / c * step * vector_y);
  int n = 0;
  double x0 = near_x;
  double y0 = near_y;
  while (true) {
    AbsMouse.move((uint16_t)x0, (uint16_t)y0);
    n++;
    if (abs(y0 - real_y) <= step and abs(x0 - real_x) <= step or n >= 500) break;
    x0 = x0 + step_a;
    y0 = y0 + step_b;
    // delay(9);
  }
  if (mode == MOUSE_LEFT || mode == MOUSE_RIGHT) {
    delay(3);
    AbsMouse.press(mode);
    delay(7);
    AbsMouse.release(mode);
    delay(5);
  }
  Serial.println(n < 500);
}

void abs_mouse_move(char *str) {
  char* d = strtok(str, ":");
  d = strtok(NULL, ",");
  const uint16_t x = (uint16_t)atoi(d);
  d = strtok(NULL, ",");
  const uint16_t y = (uint16_t)atoi(d);
  d = strtok(NULL, "");
  AbsMouse.move(x, y);
  const uint8_t mode = (uint8_t)atoi(d);

  if (mode == MOUSE_LEFT || mode == MOUSE_RIGHT) {
      AbsMouse.press(mode);
      delay(50);
      AbsMouse.release(mode);
  }
  else if (mode == 3) {
    for (int i = 0; i < 2; i++) {
      AbsMouse.press(MOUSE_LEFT);
      delay(50);
      AbsMouse.release(MOUSE_LEFT);
    }
  }
  Serial.println("ok");
}

void arrow_action(uint8_t arrow, uint8_t mode = 0) {
  switch(mode) {
    case 0:
    {
      keyboard_fight(arrow);
      delay(20);
      break;
    }
    case 1:
    {
      Keyboard.press(arrow);
      break;
    }
    case 2:
    {
      Keyboard.release(arrow);
      // delay(10);
      break;
    }
    case 3:
    {
      keyboard_fight(arrow);
      delay(20);
      Keyboard.press(arrow);
      break;
    }
    case 4:
    {
      keyboard_fight(arrow);
      delay(20);
      Keyboard.press(arrow);
      delay(500);
      Keyboard.release(arrow);
      // delay(10);
      break;
    }
    default:
      break;
  }
  Serial.println("ok");
}

void arrow_gap(int x, int y, int x1, int y1, int mode) {
  // mode: 1自动跑，2不跑，一直走
  // x轴走路状态
  // 按住500毫秒移动100像素
  // 按住100毫秒移动20像素
  // 按住10毫秒移动3像素
  // x轴跑动状态
  // 按住500毫秒移动200像素
  // 按住100毫秒移动40像素
  // y轴走路状态
  // 按住500毫秒移动70像素
  // 按住100毫秒移动14像素
  // 按住50毫秒移动7像素
  // 移动间隔不要低于10毫秒，否则很可能不移动

  uint8_t arrow_x = 0;
  uint8_t arrow_y = 0;
  const float rate_x = 0.2;  // 走路的速度
  const float rate_y = 0.14;
  int distance_x = abs(x - x1);
  int distance_y = abs(y - y1);
  float t_x = 0;
  float t_y = 0;
  if (distance_x > 0) {
    if (x > x1) {
      arrow_x = KEY_LEFT_ARROW;
    }
    else {
      arrow_x = KEY_RIGHT_ARROW;
    }
  }
  if (distance_y > 0) {
    if (y > y1) {
      arrow_y = KEY_UP_ARROW;
    }
    else {
      arrow_y = KEY_DOWN_ARROW;
    }
  }
  if (arrow_x != 0) {
    t_x = distance_x / rate_x;
  }
  if (arrow_y != 0) {
    // y轴没有跑的动作
    t_y = distance_y / rate_y;
  }
  if (arrow_x != 0) {
    if (distance_x < 100) {
      Keyboard.press(arrow_x);
    }
    else {
      if (mode == 1) {
        t_x = t_x / 2;  // 跑路的移动速度加倍
        if (distance_x >= 400) {
          t_x = t_x - 40;
        } else {
          t_x = t_x - 10;
        }
        keyboard_fight(arrow_x);
        delay(30);  // win 11 不能低于30
        Keyboard.press(arrow_x);
        delay(30);
      }
      else {
        Keyboard.press(arrow_x);
      }
    }
    if (arrow_y != 0) {
      Keyboard.press(arrow_y);
      // delay(10);
    }
    if (t_x > t_y) {
      delay_reset((int)t_y);
      Keyboard.release(arrow_y);
      delay_reset((int)(t_x - t_y));
      Keyboard.release(arrow_x);
    }
    else {
      delay_reset((int)t_x);
      Keyboard.release(arrow_x);
      delay_reset((int)(t_y - t_x));
      Keyboard.release(arrow_y);
    }
  }
  else if (arrow_y != 0) {
    Keyboard.press(arrow_y);
    delay_reset((int)t_y);
    Keyboard.release(arrow_y);
  }
  Serial.println("ok");
}

void skill_cast(const int n, const int v=1) {
  // 施放方向 v=1向右，2向左
  uint8_t right_arrow = KEY_RIGHT_ARROW;
  uint8_t left_arrow = KEY_LEFT_ARROW;
  if (v == 2) {
    right_arrow = KEY_LEFT_ARROW;
    left_arrow = KEY_RIGHT_ARROW;
  }
  const int i = GetRand(30,45);
  switch(n) {
    case 0:
    {
      arrow_action(right_arrow);
      Keyboard.press(right_arrow);
      delay(i);
      keyboard_fight(' ');
      delay(i);
      Keyboard.release(right_arrow);
      // delay(i);
      break;
    }
    case 1:
    {
      Keyboard.press(KEY_DOWN_ARROW);
      delay(i);
      keyboard_fight('c');
      delay(i);
      Keyboard.release(KEY_DOWN_ARROW);
      // delay(i);
      break;
    }
    case 2:
    {
      keyboard_fight('c');
      break;
    }
    case 3:
    {
      keyboard_fight(' ');
      break;
    }
    case 4:
    {
      Keyboard.press(KEY_UP_ARROW);
      delay(i);
      keyboard_fight('z');
      delay(i);
      Keyboard.release(KEY_UP_ARROW);
      // delay(i);
      break;
    }
    case 5:
    {
      keyboard_fight('z');
      break;
    }
    case 6:
    {
      arrow_action(right_arrow);
      Keyboard.press(right_arrow);
      delay(50);
      keyboard_fight('x');
      delay(i);
      delay(230);
      keyboard_fight('x');
      delay(i);
      Keyboard.release(right_arrow);
      // delay(i);
      break;
    }
    case 7:
    {
      Keyboard.press(right_arrow);
      delay(i);
      keyboard_fight(' ');
      delay(i);
      Keyboard.release(right_arrow);
      // delay(i);
      break;
    }
    case 8:
    {
      arrow_action(right_arrow);
      Keyboard.press(KEY_DOWN_ARROW);
      delay(i);
      keyboard_fight('z');
      delay(i);
      Keyboard.release(KEY_DOWN_ARROW);
      // delay(i);
      break;
    }
    case 9:
    {
      arrow_action(right_arrow);
      Keyboard.press(KEY_UP_ARROW);
      delay(i);
      keyboard_fight('z');
      delay(i);
      Keyboard.release(KEY_UP_ARROW);
      // delay(i);
      break;
    }
    case 10:
    {
      arrow_action(KEY_DOWN_ARROW);
      Keyboard.press(right_arrow);
      delay(i);
      keyboard_fight('z');
      delay(i);
      Keyboard.release(right_arrow);
      // delay(i);
      break;
    }
    case 11:
    {
      Keyboard.press(right_arrow);
      delay(i);
      keyboard_fight('z');
      delay(i);
      Keyboard.release(right_arrow);
      delay(320);
      Keyboard.press(left_arrow);
      delay(i);
      keyboard_fight('z');
      delay(i);
      Keyboard.release(left_arrow);
      delay(350);
      Keyboard.press(right_arrow);
      delay(i);
      keyboard_fight('z');
      delay(i);
      Keyboard.release(right_arrow);
      // delay(i);
      break;
    }
    case 12:
    {
      arrow_action(left_arrow);
      Keyboard.press(right_arrow);
      delay(i);
      keyboard_fight('z');
      delay(i);
      Keyboard.release(right_arrow);
      // delay(i);
      break;
    }
    case 13:
    {
      arrow_action(KEY_UP_ARROW);
      Keyboard.press(right_arrow);
      delay(i);
      keyboard_fight('z');
      delay(i);
      Keyboard.release(right_arrow);
      delay(150);
      arrow_action(KEY_UP_ARROW);
      Keyboard.press(right_arrow);
      delay(i);
      keyboard_fight('z');
      delay(i);
      Keyboard.release(right_arrow);
      // delay(i);
      break;
    }
    case 14:
    {
      arrow_action(left_arrow);
      arrow_action(right_arrow);
      Keyboard.press(right_arrow);
      delay(i);
      keyboard_fight('z');
      delay(i);
      Keyboard.release(right_arrow);
      // delay(i);
      break;
    }
    case 15:
    {
      if (v != 3) {
        Keyboard.press(right_arrow);
        delay(i);
      }
      // Keyboard.press(right_arrow);
      // delay(i);
      keyboard_fight('a');
      if (v != 3) {
        Keyboard.release(right_arrow);
        delay(i);
      }
      // delay(i);
      // Keyboard.release(right_arrow);
      // delay(i);
      break;
    }
    case 16:
    {
      arrow_action(KEY_UP_ARROW);
      Keyboard.press(KEY_UP_ARROW);
      delay(i);
      keyboard_fight('z');
      delay(i);
      Keyboard.release(KEY_UP_ARROW);
      // delay(i);
      break;
    }
    case 17:
    {
      if (v != 3) {
        Keyboard.press(right_arrow);
        delay(i);
      }
      // arrow_action(KEY_DOWN_ARROW);
      // Keyboard.press(right_arrow);
      // delay(i);
      keyboard_fight('q');
      if (v != 3) {
        Keyboard.release(right_arrow);
        delay(i);
      }
      // delay(i);
      // Keyboard.release(right_arrow);
      // delay(i);
      break;
    }
    case 18:
    {
      arrow_action(KEY_DOWN_ARROW);
      arrow_action(right_arrow);
      Keyboard.press(right_arrow);
      delay(i);
      keyboard_fight('z');
      delay(i);
      Keyboard.release(right_arrow);
      // delay(i);
      break;
    }
    case 19:
    {
      if (v != 3) {
        Keyboard.press(right_arrow);
        delay(i);
      }
      // arrow_action(KEY_DOWN_ARROW);
      // Keyboard.press(right_arrow);
      // delay(i);
      keyboard_fight('s');
      if (v != 3) {
        Keyboard.release(right_arrow);
        delay(i);
      }
      // delay(i);
      // Keyboard.release(right_arrow);
      break;
    }
    case 20:
    {
      arrow_action(KEY_DOWN_ARROW);
      Keyboard.press(KEY_DOWN_ARROW);
      delay(i);
      keyboard_fight(' ');
      delay(i);
      Keyboard.release(KEY_DOWN_ARROW);
      break;
    }
    case 21:
    {
      // arrow_action(KEY_UP_ARROW);
      // delay(90);
      // arrow_action(KEY_DOWN_ARROW);
      // Keyboard.press(KEY_DOWN_ARROW);
      // delay(i);
      if (v != 3) {
        Keyboard.press(right_arrow);
        delay(i);
      }
      keyboard_fight('g');
      if (v != 3) {
        Keyboard.release(right_arrow);
        delay(i);
      }
      // delay(i);
      // Keyboard.release(KEY_DOWN_ARROW);
      break;
    }
    case 22:
    {
      arrow_action(KEY_UP_ARROW);
      arrow_action(KEY_UP_ARROW);
      arrow_action(KEY_DOWN_ARROW);
      Keyboard.press(KEY_DOWN_ARROW);
      delay(i);
      keyboard_fight('z');
      delay(i);
      Keyboard.release(KEY_DOWN_ARROW);
      break;
    }
    case 23:
    {
      if (v != 3) {
        Keyboard.press(right_arrow);
        delay(i);
      }
      // arrow_action(left_arrow);
      // arrow_action(KEY_DOWN_ARROW);
      // Keyboard.press(right_arrow);
      // delay(i);
      keyboard_fight('t');
      if (v != 3) {
        Keyboard.release(right_arrow);
        delay(i);
      }
      // delay(i);
      // Keyboard.release(right_arrow);
      break;
    }
    case 24:
    {
      if (v != 3) {
        Keyboard.press(right_arrow);
        delay(i);
      }
      // arrow_action(left_arrow);
      // arrow_action(KEY_UP_ARROW);
      // Keyboard.press(right_arrow);
      // delay(i);
      keyboard_fight('f');
      if (v != 3) {
        Keyboard.release(right_arrow);
        delay(i);
      }
      // delay(i);
      // Keyboard.release(right_arrow);
      break;
    }
    case 25:
    {
      if (v != 3) {
        Keyboard.press(right_arrow);
        delay(i);
      }
      // arrow_action(left_arrow);
      // Keyboard.press(right_arrow);
      // delay(i);
      keyboard_fight('d');
      if (v != 3) {
        Keyboard.release(right_arrow);
        delay(i);
      }
      // delay(i);
      // Keyboard.release(right_arrow);
      break;
    }
    case 26:
    {
      // arrow_action(KEY_DOWN_ARROW);
      // delay(90);
      // arrow_action(KEY_UP_ARROW);
      // arrow_action(right_arrow);
      // Keyboard.press(right_arrow);
      // delay(i);
      if (v != 3) {
        Keyboard.press(right_arrow);
        delay(i);
      }
      keyboard_fight('r');
      if (v != 3) {
        Keyboard.release(right_arrow);
        delay(i);
      }
      // delay(i);
      // Keyboard.release(right_arrow);
      break;
    }
    case 27:
    {
      // arrow_action(KEY_UP_ARROW);
      // delay(90);
      // arrow_action(KEY_DOWN_ARROW);
      // arrow_action(right_arrow);
      // Keyboard.press(right_arrow);
      // delay(i);
      if (v != 3) {
        Keyboard.press(right_arrow);
        delay(i);
      }
      keyboard_fight(KEY_LEFT_ALT);
      if (v != 3) {
        Keyboard.release(right_arrow);
        delay(i);
      }
      // delay(i);
      // Keyboard.release(right_arrow);
      break;
    }
    case 28:
    {
      if (v != 3) {
        Keyboard.press(right_arrow);
        delay(i);
      }
      keyboard_fight('e');
      if (v != 3) {
        Keyboard.release(right_arrow);
        delay(i);
      }
      // delay(i);
      // Keyboard.release(KEY_DOWN_ARROW);
      break;
    }
    case 29:
    {
      if (v != 3) {
        Keyboard.press(right_arrow);
        delay(i);
      }
      keyboard_fight('y');
      if (v != 3) {
        Keyboard.release(right_arrow);
        delay(i);
      }
      break;
    }
    case 30:
    {
      if (v != 3) {
        Keyboard.press(right_arrow);
        delay(i);
      }
      keyboard_fight('h');
      if (v != 3) {
        Keyboard.release(right_arrow);
        delay(i);
      }
      break;
    }
    case 31:
    {
      if (v != 3) {
        Keyboard.press(right_arrow);
        delay(i);
      }
      keyboard_fight('w');
      if (v != 3) {
        Keyboard.release(right_arrow);
        delay(i);
      }
      break;
    }
    case 32:
    {
      arrow_action(KEY_UP_ARROW);
      Keyboard.press(KEY_UP_ARROW);
      delay(i);
      keyboard_fight(' ');
      delay(i);
      Keyboard.release(KEY_UP_ARROW);
      // delay(i);
      break;
    }
    case 33:
    {
      arrow_action(KEY_DOWN_ARROW);
      Keyboard.press(KEY_UP_ARROW);
      delay(i);
      keyboard_fight(' ');
      delay(i);
      Keyboard.release(KEY_UP_ARROW);
      // delay(i);
      break;
    }
    case 98:
    {
      keyboard_fight('x');
      break;
    }
    case 99:
    {
      for (int i = 0; i < 3; i++) {
        keyboard_fight('x');
        if (i < 2) {
          delay(GetRand(70,120));
        }
      }
      delay(GetRand(15,30));
      break;
    }
    default:
      break;
  }
}

// void mouse_release_delay() {
//   int a = 0;
//   do
//   {
//     delay((unsigned long)(20 + my_random(30)));
//     a = a + 1;
//   } while ((a < 4));
// }

void WindMouse(double xs, double ys, double xe, double ye, double gravity, double wind, double minSleep, double maxSleep, double maxStep, double targetArea)
{
  double   veloX = 1, veloY = 1, windX = 1, windY = 1, veloMag, dist, randomDist, lastDist, step;
  int       lastX, lastY;
  double   sqrt2, sqrt3, sqrt5;

  sqrt2 = sqrt(2.0);
  sqrt3 = sqrt(3.0);
  sqrt5 = sqrt(5.0);
  while (hypot(xs - xe, ys - ye) > 1)
  {
    dist = hypot(xs - xe, ys - ye);
    wind = min(wind, dist);
    if (dist >= targetArea)
    {
      windX = windX / sqrt3 + (my_random(floor(wind) * 2.0 + 1.0) - wind) / sqrt5;
      windY = windY / sqrt3 + (my_random(floor(wind) * 2.0 + 1.0) - wind) / sqrt5;
    }
    else
    {
      windX = windX / sqrt2;
      windY = windY / sqrt2;
      if ((maxStep < 3))
      {
        maxStep = my_random(3) + 3.0;
      }
      else
      {
        maxStep = maxStep / sqrt5;
      }
    }
    veloX = veloX + windX;
    veloY = veloY + windY;
    veloX = veloX + gravity * (xe - xs) / dist;
    veloY = veloY + gravity * (ye - ys) / dist;
    if (hypot(veloX, veloY) > maxStep)
    {
      randomDist = maxStep / 2.0 + my_random(floor(maxStep) / 2);
      veloMag = sqrt(veloX * veloX + veloY * veloY);
      veloX = (veloX / veloMag) * randomDist;
      veloY = (veloY / veloMag) * randomDist;
    }
    lastX = (int)floor(xs);
    lastY = (int)floor(ys);
    xs = xs + veloX;
    ys = ys + veloY;
    if ((lastX != floor(xs)) || (lastY != floor(ys)))
      AbsMouse.move((uint16_t)floor(xs), (uint16_t)floor(ys));
    step = hypot(xs - lastX, ys - lastY);
    delay((unsigned long)floor((maxSleep - minSleep) * (step / maxStep) + minSleep));
    lastDist = dist;
    wdt_reset();  /* Reset the watchdog */
  }
  if ((floor(xe) != floor(xs)) || (floor(ye) != floor(ys)))
    AbsMouse.move((uint16_t)floor(xe), (uint16_t)floor(ye));
}

void MMouse_Human(int cursorx, int cursory, int x, int y, int rx, int ry)
{
  double   randSpeed;
  // randSpeed = (my_random(13.0) / 2.0 + 13.0) / 10.0;
  randSpeed = (my_random(75.0) / 2.0 + 13.0) / 10.0;
  // Serial.println(randSpeed);
  // if (randSpeed == 0.0)
  //   randSpeed = 0.1;
  if (randSpeed <= 2.0)
    randSpeed = 2.0;
  x = x + (int)my_random(rx);
  y = y + (int)my_random(ry);
  double wind = (double)GetRand(3, 9);
  WindMouse((double)cursorx, (double)cursory, (double)x, (double)y, 9.0, wind, 10.0 / randSpeed, 15.0 / randSpeed, 10.0 * randSpeed, 10.0 * randSpeed);
}

void Mouse_Human(char *str)
{
  char* d = strtok(str, ":");
  d = strtok(NULL, ",");
  const int cursorx = atoi(d);
  d = strtok(NULL, ",");
  const int cursory = atoi(d);
  d = strtok(NULL, ",");
  const int mousex = atoi(d);
  d = strtok(NULL, ",");
  const int mousey = atoi(d);
  d = strtok(NULL, "");
  const uint8_t mode = (uint8_t)atoi(d);
  int ranx = GetRand(1, 3);
  int rany = GetRand(1, 3);
  MMouse_Human(cursorx, cursory, mousex, mousey, ranx, rany);

  if (mode == MOUSE_LEFT || mode == MOUSE_RIGHT) {
    delay((unsigned long)(60 + my_random(30)));
    AbsMouse.press(mode);
    // mouse_release_delay();
    delay(GetRand(50,75));
    AbsMouse.release(mode);
    // delay((unsigned long)(100 + my_random(100)));
    delay((unsigned long)(3 + my_random(100)));  // 通过窗口传输需要时间，这里就不做太长的延时了
    wdt_reset();  /* Reset the watchdog */
  }
  else if (mode == 3)
  {
    delay((unsigned long)(60 + my_random(30)));
    for (int i = 0; i < 2; i++) {
      AbsMouse.press(MOUSE_LEFT);
      delay(GetRand(50,75));
      AbsMouse.release(MOUSE_LEFT);
      delay(GetRand(50,75));
    }
  }
  Serial.println("ok");
}

void abs_mouse_drag_and_drop(char *str) {
  char* d = strtok(str, ":");
  d = strtok(NULL, ",");
  const uint16_t x0 = (uint16_t)atoi(d);
  d = strtok(NULL, ",");
  const uint16_t y0 = (uint16_t)atoi(d);
  d = strtok(NULL, ",");
  const uint16_t x1 = (uint16_t)atoi(d);
  d = strtok(NULL, ",");
  const uint16_t y1 = (uint16_t)atoi(d);

  AbsMouse.move(x0, y0);
  delay(10);
  AbsMouse.press();
  delay(100);
  AbsMouse.move(x1, y1);
  delay(10);
  AbsMouse.release();
}

bool prefix(const char *pre, const char *str)
{
    return strncmp(pre, str, strlen(pre)) == 0;
}

int freeRam() {
  extern int __heap_start, *__brkval;
  int v = 0;
  return (int) &v - (__brkval == 0 ? (int) &__heap_start : (int) __brkval);
}

void setup() {
  // pinMode(2, INPUT);
  // digitalWrite(4, HIGH);
  // delay(20);
  // pinMode(4, OUTPUT);   // reset
  // pinMode(LED_BUILTIN, OUTPUT);
  Serial.begin(57616);
  Serial.setTimeout(100);
  while (!Serial) {
    ;  // wait for serial port to connect. Needed for native USB port only
  }

  // initialize mouse control:
  // Mouse.begin();
  Keyboard.begin();
  AbsMouse.init(1920, 1080);

  // #if !defined(UBRR1H)
    mySerial.begin(9600);
    myMP3.begin(mySerial, false);
  // #else
  //   Serial1.begin(9600);
  //   myMP3.begin(Serial1, false);
  // #endif

  wdt_disable();  /* Disable the watchdog and wait for more than 2 seconds */
  delay(3000);  /* Done so that the Arduino doesn't keep resetting infinitely in case of wrong configuration */
  wdt_enable(WDTO_4S);  /* Enable the watchdog with a timeout of 2 seconds */
  myMP3.setTimeout(300);
}

void loop() {
  if (work) {
    // if (digitalRead(2) == HIGH) work = false;  // 用于失控时通过物理接线中断程序
    if (abs(millis() - timer) > 1200) {
      timer = millis();
      wdt_reset();  /* Reset the watchdog */
    }
    serialEvent();
    // AbsMouse.scroll(0, -1);  // 负数向下，正数向上

    if (readComplete) {
      // digitalWrite(LED_BUILTIN, HIGH);

      if (prefix("movehm:", inData)) {
        Mouse_Human(inData);
      }
      else if (strcmp("msps", inData) == 0) {
        AbsMouse.press();
        delay(GetRand(5,15));
        Serial.println("ok");
      }
      else if (strcmp("msrl", inData) == 0) {
        AbsMouse.release();
        delay(GetRand(5,15));
        Serial.println("ok");
      }
      else if (prefix("clickhm:", inData)) {
        abs_mouse_click(inData);
        Serial.println("ok");
      }
      else if (strcmp("hkey:ENTER", inData) == 0) {
        hotkey_press(KEY_KP_ENTER);
        Serial.println("ok");
      }
      else if (strcmp("hkey:ESC", inData) == 0) {
        hotkey_press(KEY_ESC);
        Serial.println("ok");
      }
      else if (strcmp("hkey:F1", inData) == 0) {
        hotkey_press(KEY_F1);
        Serial.println("ok");
      }
      else if (strcmp("hkey:F2", inData) == 0) {
        hotkey_press(KEY_F2);
        Serial.println("ok");
      }
      else if (strcmp("hkey:F3", inData) == 0) {
        hotkey_press(KEY_F3);
        Serial.println("ok");
      }
      else if (strcmp("hkey:F4", inData) == 0) {
        hotkey_press(KEY_F4);
        Serial.println("ok");
      }
      else if (strcmp("hkey:F5", inData) == 0) {
        hotkey_press(KEY_F5);
        Serial.println("ok");
      }
      else if (strcmp("hkey:F6", inData) == 0) {
        hotkey_press(KEY_F6);
        Serial.println("ok");
      }
      else if (strcmp("hkey:F7", inData) == 0) {
        hotkey_press(KEY_F7);
        Serial.println("ok");
      }
      else if (strcmp("hkey:F8", inData) == 0) {
        hotkey_press(KEY_F8);
        Serial.println("ok");
      } else if (strcmp("hkey:F9", inData) == 0) {
        hotkey_press(KEY_F9);
        Serial.println("ok");
      } else if (strcmp("hkey:TAB", inData) == 0) {
        hotkey_press(KEY_TAB);
        Serial.println("ok");
      } else if (prefix("hkey:BACKSPACE", inData)) {
        char* d = strtok(inData, "_");
        d = strtok(NULL, "");
        const int n = atoi(d);
        for (int i = 0; i < n; i++) {
          hotkey_press(KEY_BACKSPACE);
        }
        Serial.println("ok");
      } else if (strcmp("hkey:DELETE", inData) == 0) {
        hotkey_press(KEY_DELETE);
        Serial.println("ok");
      }
      else if (strcmp("hkey:WIN", inData) == 0) {
        hotkey_press(KEY_LEFT_GUI);
        Serial.println("ok");
      }
      else if (strcmp("hkey:PAGE_UP", inData) == 0) {
        hotkey_press(KEY_PAGE_UP);
        Serial.println("ok");
      }
      else if (strcmp("hkey:PAGE_DOWN", inData) == 0) {
        hotkey_press(KEY_PAGE_DOWN);
        Serial.println("ok");
      }
      else if (strcmp("hkey:UP_ARROW", inData) == 0) {
        hotkey_press(KEY_UP_ARROW);
        Serial.println("ok");
      }
      else if (strcmp("hkey:DOWN_ARROW", inData) == 0) {
        hotkey_press(KEY_DOWN_ARROW);
        Serial.println("ok");
      }
      else if (prefix("hkey:ALT_", inData)) {
        char* d = strtok(inData, "_");
        d = strtok(NULL, "");
        hotkey_alt_xxx(d[0]);
      }
      else if (prefix("hkey:ALT+", inData)) {
        char* d = strtok(inData, "+");
        d = strtok(NULL, "");
        hotkey_alt_fff(d);
      } else if (prefix("hkey:CTRL_", inData)) {
        char* d = strtok(inData, "_");
        d = strtok(NULL, "");
        hotkey_ctrl_xxx(d[0]);
      } else if (prefix("hkey:CTRL+", inData)) {
        char* d = strtok(inData, "+");
        d = strtok(NULL, "");
        hotkey_ctrl_fff(d);
      }
      else if (strcmp("hide", inData) == 0) {
        hotkey_press(KEY_F9);
        delay(GetRand(15,30));
        Keyboard.release(KEY_F9);
        delay(GetRand(5,15));
        Keyboard.press(KEY_LEFT_ALT);
        delay(GetRand(15,30));
        Keyboard.write('h');
        delay(GetRand(15,30));
        Keyboard.release(KEY_LEFT_ALT);
        Serial.println("ok");
      }
      else if (strcmp("hkeyCC", inData) == 0) {
        AbsMouse.press(MOUSE_LEFT);
        delay(GetRand(5,20));
        AbsMouse.release(MOUSE_LEFT);
        Serial.println("ok");
      }
      else if (strcmp("hkeyRCC", inData) == 0) {
        AbsMouse.press(MOUSE_RIGHT);
        delay(GetRand(5,20));
        AbsMouse.release(MOUSE_RIGHT);
        Serial.println("ok");
      }
      else if (strcmp("hkeyCCC", inData) == 0) {
        Keyboard.press(KEY_LEFT_CTRL);
        delay(GetRand(15,30));
        AbsMouse.press(MOUSE_LEFT);
        delay(30);
        AbsMouse.release(MOUSE_LEFT);
        delay((unsigned long)(100 + my_random(100)));  // 经过多次测试，这个间隔不能太短，否则点击不生效
        // delay((unsigned long)(3 + my_random(100)));  // 通过窗口传输需要时间，这里就不做太长的延时了

        Keyboard.release(KEY_LEFT_CTRL);
        Serial.println("ok");
      }
      else if (prefix("scrollhm:", inData)) {
        char* d = strtok(inData, ":");
        d = strtok(NULL, ",");
        const uint16_t x = (uint16_t)atoi(d);
        d = strtok(NULL, ",");
        const uint16_t y = (uint16_t)atoi(d);
        d = strtok(NULL, "");
        const int n = atoi(d);
        for (int i = 0; i < n; i++) {
          AbsMouse.scroll(x, y);  // 负数向下，正数向上
          delay(GetRand(50,65));
        }
      }
      else if (prefix("dnd:", inData)) {
        abs_mouse_drag_and_drop(inData);
        Serial.println("ok");
      }
      else if (prefix("mmp3:PLAY_", inData)) {
        myMP3.volume(5);
        char* d = strtok(inData, "_");
        d = strtok(NULL, "");
        const int n = atoi(d);
        delay(50);  // 经过多次测试，50是最低值，少了会出问题
        for (int i = 0; i < 3; i++) {
          myMP3.playFromMP3Folder(n);
          delay(100);
          if (myMP3.isPlaying()) break;
        }
        // myMP3.playFromMP3Folder(n);
        delay(100);
        myMP3.startRepeat();
        Serial.println("ok");
      }
      else if (strcmp("mmp3:STOP", inData) == 0) {
        // for (int i = 0; i < 5; i++) {
        //   // Serial.println(myMP3.isPlaying());
        //   if (!myMP3.isPlaying()) break;
        //   delay(100);
        //   myMP3.stop();
        // }
        myMP3.stop();
        // delay(200);
        // myMP3.stopRepeatPlay();
        Serial.println("ok");
      }
      else if (strcmp("test", inData) == 0) {
        Keyboard.press(KEY_RIGHT_ARROW);
        delay(10);
        Keyboard.release(KEY_RIGHT_ARROW);
        delay(10);
        Keyboard.press(KEY_RIGHT_ARROW);
        delay(1000);
        Keyboard.release(KEY_RIGHT_ARROW);
        Serial.println("ok");
      }
      else if (prefix("movedf:", inData)) {
        abs_mouse_move(inData);
      } else if (prefix("scroll:", inData)) {
        char* d = strtok(inData, ":");
        d = strtok(NULL, ",");
        const uint16_t x = (uint16_t)atoi(d);
        d = strtok(NULL, ",");
        const uint16_t y = (uint16_t)atoi(d);
        d = strtok(NULL, "");
        const int n = atoi(d);
        for (int i = 0; i < n; i++) {
          AbsMouse.scroll(x, y);  // 负数向下，正数向上
          delay(GetRand(50,75));
        }
      } else if (prefix("hkw:", inData)) {
        char* d = strtok(inData, ":");
        d = strtok(NULL, ",");
        const uint8_t k = d[0];
        d = strtok(NULL, "");
        const uint8_t mode = atoi(d);
        switch(mode) {
          case 0:
          {
            hotkey_press(k);
            break;
          }
          case 1:
          {
            Keyboard.press(k);
            break;
          }
          case 2:
          {
            Keyboard.release(k);
            break;
          }
          default:
            break;
        }
        Serial.println("ok");
      }
      else if (prefix("kw:", inData)) {
        char* d = strtok(inData, ":");
        d = strtok(NULL, "");
        Keyboard.write(d);
        Serial.println("ok");
      } else if (prefix("kf:", inData)) {
        char* d = strtok(inData, ":");
        d = strtok(NULL, "");
        if (strcmp("alt", d) == 0) {
          keyboard_fight(KEY_LEFT_ALT);
        }
        else if (strcmp("ctrl", d) == 0) {
          keyboard_fight(KEY_LEFT_CTRL);
        }
        else {
          keyboard_fight(d[0]);
        }
        Serial.println("ok");
      }
       else if (prefix("kp:", inData)) {
        char* d = strtok(inData, ":");
        d = strtok(NULL, "");
        const int n = atoi(d);
        switch (n) {
          case 1:
            keyboard_fight('1');
            break;
          case 2:
            keyboard_fight('2');
            break;
          case 3:
            keyboard_fight('3');
            break;
          case 4:
            keyboard_fight('4');
            break;
          case 5:
            keyboard_fight('5');
            break;
          case 6:
            keyboard_fight('6');
            break;
          default:
            break;
        }
        Serial.println("ok");
      }
      else if (prefix("arrow:", inData)) {
        char* d = strtok(inData, ":");
        d = strtok(NULL, ",");
        const int a = atoi(d);
        d = strtok(NULL, "");
        const int mode = atoi(d);
        uint8_t arrow = 0;
        switch (a) {
          case 1:
            arrow = KEY_UP_ARROW;
            break;
          case 2:
            arrow = KEY_DOWN_ARROW;
            break;
          case 3:
            arrow = KEY_LEFT_ARROW;
            break;
          case 4:
            arrow = KEY_RIGHT_ARROW;
            break;
          default:
            break;
        }
        arrow_action(arrow, (uint8_t)mode);
      }
      else if (prefix("arwgap:", inData)) {
        char* d = strtok(inData, ":");
        d = strtok(NULL, ",");
        const int x = atoi(d);
        d = strtok(NULL, ",");
        const int y = atoi(d);
        d = strtok(NULL, ",");
        const int x1 = atoi(d);
        d = strtok(NULL, ",");
        const int y1 = atoi(d);
        d = strtok(NULL, "");
        const int mode = atoi(d);
        arrow_gap(x, y, x1, y1, mode);
      }
      else if (strcmp("dkey:SPACE", inData) == 0) {
        keyboard_fight(' ');
        Serial.println("ok");
      }
      else if (prefix("sk:", inData)) {
        char* d = strtok(inData, ":");
         d = strtok(NULL, ",");
        const int n = atoi(d);
        d = strtok(NULL, "");
        const int v = atoi(d);
        skill_cast(n, v);
        Serial.println("ok");
      }
      else if (prefix("sk_sx:", inData)) {
        char* d = strtok(inData, ":");
        d = strtok(NULL, ",");
        const int cursorx = atoi(d);
        d = strtok(NULL, ",");
        const int cursory = atoi(d);
        d = strtok(NULL, ",");
        const int mousex = atoi(d);
        d = strtok(NULL, ",");
        const int mousey = atoi(d);
        d = strtok(NULL, "");
        const int n = atoi(d);
        int ranx = GetRand(1, 3);
        int rany = GetRand(1, 3);
        MMouse_Human(cursorx, cursory, mousex, mousey, ranx, rany);
        delay(20);
        for (int i = 0; i < n; i++) {
          hotkey_press('w');
          delay(200);
        }
        Serial.println("ok");
      }
      else if (strcmp("dkey:ESC", inData) == 0) {
        keyboard_fight(KEY_ESC);
        Serial.println("ok");
      }
      else if (strcmp("dkey:ENTER", inData) == 0) {
        keyboard_fight(KEY_KP_ENTER);
        Serial.println("ok");
      }
      else if (strcmp("dkey:F9", inData) == 0) {
        keyboard_fight(KEY_F9);
        Serial.println("ok");
      }
      else if (strcmp("dkey:F10", inData) == 0) {
        keyboard_fight(KEY_F10);
        Serial.println("ok");
      }
      else if (strcmp("dkey:F11", inData) == 0) {
        keyboard_fight(KEY_F11);
        Serial.println("ok");
      }
      else if (strcmp("dkey:F12", inData) == 0) {
        keyboard_fight(KEY_F12);
        Serial.println("ok");
      }
      else if (prefix("ALT+", inData)) {
        char* d = strtok(inData, "+");
        d = strtok(NULL, "");
        alt_fff(d);
      }
      else if (prefix("WIN_", inData)) {
        char* d = strtok(inData, "_");
        d = strtok(NULL, "");
        win_xxx(d[0]);
      }
      else if (prefix("dkey:BACKSPACE", inData)) {
        char* d = strtok(inData, "_");
        d = strtok(NULL, "");
        const int n = atoi(d);
        for (int i = 0; i < n; i++) {
          keyboard_press(KEY_BACKSPACE);
          delay(5);
        }
        Serial.println("ok");
      } else if (strcmp("dkey:DELETE", inData) == 0) {
        keyboard_press(KEY_DELETE);
      }
      else if (strcmp("dkey:WIN", inData) == 0) {
        keyboard_press(KEY_LEFT_GUI);
      }
      else if (strcmp("dkey:UP_ARROW", inData) == 0) {
        keyboard_press(KEY_UP_ARROW);
      }
      else if (strcmp("dkey:DOWN_ARROW", inData) == 0) {
        keyboard_press(KEY_DOWN_ARROW);
      }
      else if (prefix("kdown:", inData)) {
        char* d = strtok(inData, ":");
        d = strtok(NULL, "");
        Keyboard.press(d[0]);
        Serial.println("ok");
      }
      else if (prefix("kup:", inData)) {
        char* d = strtok(inData, ":");
        d = strtok(NULL, "");
        Keyboard.release(d[0]);
        Serial.println("ok");
      }
      else if (prefix("absmouse_init:", inData)) {
        char* d = strtok(inData, ":");
        d = strtok(NULL, ",");
        const uint16_t w = (uint16_t)atoi(d);
        d = strtok(NULL, "");
        const uint16_t h = (uint16_t)atoi(d);
        AbsMouse.init(w, h);
      }
      else if (strcmp("reset", inData) == 0) {
        memset(inData, 0, 60);
        readComplete = false;
        digitalWrite(4, LOW);  // reset
      }
    // digitalWrite(LED_BUILTIN, LOW);

      // Serial.flush();
      memset(inData, 0, 60);
      readComplete = false;
    }

    // delay(1);
    // delay(500);
  }
}

/*
  SerialEvent occurs whenever a new data comes in the hardware serial RX. This
  routine is run between each time loop() runs, so using delay inside loop can
  delay response. Multiple bytes of data may be available.
*/
void serialEvent() {
  while (Serial.available()) {
    number_of_bytes_received = Serial.readBytesUntil(13, inData, 60); // read bytes (max. 20) from buffer, untill <CR> (13). store bytes in data. count the bytes recieved.
    if (number_of_bytes_received <= 0) continue;
    inData[number_of_bytes_received - 1] = 0; // add a 0 terminator to the char array
    readComplete = true;
  }
}
