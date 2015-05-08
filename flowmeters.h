#ifndef FLOWMETERS_H
#define FLOWMETERS_H

#include "flowmeters.h"


//============================================================================================================//
// ___________________________________________ PELTIERS __________________________________________________
#define MAX_NB_FLOW 3
#define NB_WAIT_VALUE 100
typedef struct Flowmeter{
  int pin_in;
  char *name;

  long unsigned time_start;
  int nb;
  float val;

} Flowmeter;


extern Flowmeter mainloop_flow;

void create_flowmeter(struct flowmeter &flowmeter, int pin_in, char *name);

void init_flowmeters();

void interupt_mainloop();

// ___________________________________________ PELTIERS __________________________________________________
//============================================================================================================//


#endif