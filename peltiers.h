#ifndef PELTIERS_H
#define PELTIERS_H

#include "peltiers.h"


//============================================================================================================//
// ___________________________________________ PELTIERS __________________________________________________
#define MAX_NB_TEC 6
typedef struct Peltier{
  int pin_out;
  char *name;
} Peltier;


void create_peltier(struct Peltier &peltier, int pin_out, char *name);

void init_peltiers();

// ___________________________________________ PELTIERS __________________________________________________
//============================================================================================================//


#endif