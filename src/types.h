/*
 * File: types.h
 * Author: Martin Löfgren <martin.c.lofgren@gmail.com>
 */

#ifndef TRAFIC_SIGNAL_TYPES_H
#define TRAFIC_SIGNAL_TYPES_H

typedef void (*voidFunction)(void);

typedef enum {
  stop,
  drive_soon,
  drive,
  stop_soon,
} carLightState;

typedef enum {
  stay,
  go
} walkLightState;

typedef enum {
  automatic,
  semiAutomatic,
  manual
} toggleMode;

typedef struct walkLight {
  walkLightState state;
  uint8_t redLight;
  uint8_t greenLight;
} walkLight_t;

typedef struct carLight {
  carLightState state;
  uint8_t redLight;
  uint8_t yellowLight;
  uint8_t greenLight;
  struct carLight* nextCarLight;
  struct walkLight* walkLight;
} carLight_t;

typedef struct {
  enum {walk, car} type;
  union {
    walkLight_t* w;
    carLight_t* c;
  };
  struct {
    uint8_t x;
    uint8_t y;
  } pos;
} light_t;

#endif // TRAFIC_SIGNAL_TYPES_H
