#ifndef SERVOVALVES_H
#define SERVOVALVES_H

#include "servovalves.h"
#include <Servo.h>


//============================================================================================================//
// ___________________________________________ PELTIERS __________________________________________________
#define MAX_NB_VALVE 3
#define MIN_POS 0
#define MAX_POS 180
typedef struct Servovalve{
  Servo servo_pin; //Sortie de commande servomoteur
  int pin_close; //pin d'entrée du capteur position fermé
  int pin_servo;
  int pos_min;
  int pos_max;
  int offset;
  char *name; //nom
} Servovalve;


void create_servovalve(struct Servovalve &servovalve, int pin_out, int pin_close, char *name);

void init_servovalves();

// ___________________________________________ PELTIERS __________________________________________________
//============================================================================================================//


#endif