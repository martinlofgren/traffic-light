/*
 * File: main.c
 * Author: Martin Löfgren <martin.c.lofgren@gmail.com>
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include <unistd.h>

#include "types.h"
#include "sim.h"

// Traffic lights
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

toggleMode mode = semiAutomatic;

/*****************************************************************************
 *
 *  FUNCTION DEFINITIONS
 *
 ****************************************************************************/

/**
 * @brief  Switch between traffic light states. When in stop state, change
 *         to next traffic light and start state cycle for it.
 */ 
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

  writeIO();
}

void buttonFn(void) {
  if (mode == manual ||
      (mode == semiAutomatic && curCar->state == stop))
    changeState();
}

void changeMode(void) {
  switch (mode) {
  case automatic:
    mode = semiAutomatic;
    break;
  case semiAutomatic:
    mode = manual;
    break;
  case manual:
    mode = automatic;
    break;
  }
}

int main() {
  c1.nextCarLight = &c2;
  c2.nextCarLight = &c1;

  irqRegister(0, buttonFn);
  irqRegister(1, changeMode);

  initIO();

  while(1) {
    readIO();
    if (mode == automatic ||
	(mode == semiAutomatic &&
	 curCar->state != stop)) {
      changeState();
      sleep(1);
    }
  }

  return 0;
}
