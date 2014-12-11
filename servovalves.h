#ifndef SERVOVALVES_H
#define SERVOVALVES_H

#include "servovalves.h"
#include <Servo.h>


//============================================================================================================//
// ___________________________________________ PELTIERS __________________________________________________
#define MAX_NB_VALVE 3
#define MIN_POS 0
#define MAX_POS 120
typedef struct Servovalve{
  int pin_out; //Sortie de commande servomoteur
  int pin_close; //pin d'entrée du capteur position fermé
  char *name; //nom
} Servovalve;


void create_servovalve(struct Servovalve &servovalve, int pin_out, int pin_close, char *name);

void init_servovalves();
void init_valves();

// ___________________________________________ PELTIERS __________________________________________________
//============================================================================================================//


#endif