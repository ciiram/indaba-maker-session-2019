#include "select_program.h"

#if PROGRAM == HELLO_WORLD

#include "mbed.h"

DigitalOut led(LED1);  // led to blink
const float BLINK_PERIOD_S = 1;
int main() {
  while (1) {
    led = !led;  // change the state of LED OFF -> ON or ON -> OFF
    wait(BLINK_PERIOD_S / 2);  // wait for half the period
  }
}

#endif

