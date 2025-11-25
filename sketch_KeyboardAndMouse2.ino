/*
arduino
*/
#include<math.h>
#include "Keyboard.h"
#include "AbsMouse.h"
// #include "Mouse.h"
#include<avr/wdt.h> /* Header for watchdog timers in AVR */
// #include "Arduino.h"

char inData[60];  // Allocate some space for the string
int number_of_bytes_received;
bool readComplete = false;  // whether read is complete
bool work = true;
unsigned long timer = millis();

double my_random(double x)
{
    srand((unsigned)millis());
    if (x < 1)
        return 0;
    return (double)((rand() % (int)x));
}

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
      // Serial.println(floor((maxSleep - minSleep) * (step / maxStep) + minSleep));
      delay((unsigned long)floor((maxSleep - minSleep) * (step / maxStep) + minSleep));
      lastDist = dist;
// wdt_reset();  /* Reset the watchdog */
    }
    Serial.println("floor");
    if ((floor(xe) != floor(xs)) || (floor(ye) != floor(ys))) {
      AbsMouse.move((uint16_t)floor(xe), (uint16_t)floor(ye));
      Serial.println("AbsMouse.moveok");
    }
}
void MMouse_Human(int cursorx, int cursory, int x, int y, int rx, int ry)
{
    double   randSpeed;
    randSpeed = (my_random(13.0) / 2.0 + 13.0) / 10.0;
    if (randSpeed == 0.0)
        randSpeed = 0.1;
    x = x + (int)my_random(rx);
    y = y + (int)my_random(ry);
    WindMouse((double)cursorx, (double)cursory, (double)x, (double)y, 9.0, 3.0, 10.0 / randSpeed, 15.0 / randSpeed, 10.0 * randSpeed, 10.0 * randSpeed);
}
void Mouse_Human(int cursorx, int cursory, int mousex, int mousey, int ranx, int rany, uint8_t mode = MOUSE_LEFT)
{
    int a = 0;
    MMouse_Human(cursorx, cursory, mousex, mousey, ranx, rany);
    // if (mode == MOUSE_LEFT || mode == MOUSE_RIGHT) {
    //   delay((unsigned long)(60 + my_random(30)));
    //   AbsMouse.press(mode);
    //   do
    //   {
    //       delay((unsigned long)(20 + my_random(30)));
    //       a = a + 1;
    //   } while ((a < 4));
    //   AbsMouse.release(mode);
    //   delay((unsigned long)(100 + my_random(100)));
    // }
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

bool prefix(const char *pre, const char *str)
{
    return strncmp(pre, str, strlen(pre)) == 0;
}

void setup() {
  pinMode(2, INPUT);
  digitalWrite(4, HIGH);
  delay(20);
  pinMode(4, OUTPUT);   // reset
  pinMode(LED_BUILTIN, OUTPUT);

  Serial.begin(57616);
  Serial.setTimeout(100);
  while (!Serial) {
    ;  // wait for serial port to connect. Needed for native USB port only
  }

  // initialize mouse control:
  // Mouse.begin();
  Keyboard.begin();
  AbsMouse.init(1920, 1080);

  wdt_disable();  /* Disable the watchdog and wait for more than 2 seconds */
  delay(3000);  /* Done so that the Arduino doesn't keep resetting infinitely in case of wrong configuration */
  wdt_enable(WDTO_4S);  /* Enable the watchdog with a timeout of 2 seconds */
}

void loop() {
  if (work) {
    if (digitalRead(2) == HIGH) work = false;  // 用于失控时通过物理接线中断程序

    if (millis() - timer > 1000) {
      timer = millis();
      wdt_reset();  /* Reset the watchdog */
    }

    serialEvent();
    // AbsMouse.move(1354, 624);
    // AbsMouse.scroll(0, -1);  // 负数向下，正数向上
    // Mouse.move(0, 0, 1);

    if (readComplete) {
      // Serial.println(inData);
      // digitalWrite(LED_BUILTIN, HIGH);
      if (prefix("windmouse:", inData)) {
        int c, d;

        c = GetRand(1, 5);
        d = GetRand(1, 5);

      unsigned long m_timer = millis();
        Mouse_Human(0, 0, 1600, 900, c, d, 0);
        Serial.println(millis() - m_timer);
      }

    // digitalWrite(LED_BUILTIN, LOW);

      // Serial.flush();
      memset(inData, 0, 60);
      readComplete = false;

      // int free_ram = freeRam();
      // Serial.println(free_ram);
      // if (free_ram == NULL or free_ram <= 100) {
      //   Serial.println("reset");
      //   memset(inData, 0, 60);
      //   readComplete = false;
      //   delay(200);
      //   digitalWrite(4, LOW);  // reset
      // }

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
