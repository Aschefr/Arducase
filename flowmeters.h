#ifndef FLOWMETERS_H
#define FLOWMETERS_H

#include "flowmeters.h"


//============================================================================================================//
// ___________________________________________ PELTIERS __________________________________________________
#define MAX_NB_FLOW 3
typedef struct Flowmeter{
  int pin_in;
  char *name;
} Flowmeter;



void create_flowmeter(struct flowmeter &flowmeter, int pin_in, char *name);

void init_flowmeters();

// ___________________________________________ PELTIERS __________________________________________________
//============================================================================================================//


#endif