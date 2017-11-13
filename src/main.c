/*
 * File: main.c
 * Author: Martin Löfgren <martin.c.lofgren@gmail.com>
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include "types.h"
#include "sim.h"

// Trafic lights
walkLight_t w1 = {
  .state = stay,
  .redLight = 1,
  .greenLight = 0
};

walkLight_t w2 = {
  .state = stay,
  .redLight = 1,
  .greenLight = 0
};

carLight_t c1 = {
  .state = stop,
  .redLight = 1,
  .yellowLight = 0,
  .greenLight = 0,
  .nextCarLight = NULL,
  .walkLight = &w2
};

carLight_t c2 = {
  .state = stop,
  .redLight = 1,
  .yellowLight = 0,
  .greenLight = 0,
  .nextCarLight = NULL,
  .walkLight = &w1
};

carLight_t* curCar = &c1;

/* Function definitions */
void changeState() {
  walkLight_t* w = curCar->walkLight;
  switch (curCar->state) {
  case stop:
    curCar->redLight = 1;
    curCar->yellowLight = 0;
    curCar = curCar->nextCarLight;
    curCar->state = drive_soon;
    w->state = stay;
    break;
  case drive_soon:
    curCar->yellowLight = 1;
    curCar->state = drive;
    w->state = go;
    break;
  case drive:
    curCar->redLight = 0;
    curCar->yellowLight = 0;
    curCar->greenLight = 1;
    curCar->state = stop_soon;
    break;
  case stop_soon:
    curCar->yellowLight = 1;
    curCar->greenLight = 0;
    curCar->state = stop;
    break;
  }

  switch (w->state) {
  case stay:
    w->redLight = 1;
    w->greenLight = 0;
    break;
  case go:
    w->redLight = 0;
    w->greenLight = 1;
    break;
  }
}

int main() {
  c1.nextCarLight = &c2;
  c2.nextCarLight = &c1;

  irqRegister(0, changeState);

  run();

  return 0;
}
