/*
 * File: sim.h
 * Author: Martin Löfgren <martin.c.lofgren@gmail.com>
 */

#ifndef SIM_H
#define SIM_H

#include "types.h"

extern walkLight_t w1;
extern walkLight_t w2;
extern carLight_t c1;
extern carLight_t c2;

void run();
void irqRegister(int irqNum, voidFunction fn);

#endif //SIM_H
