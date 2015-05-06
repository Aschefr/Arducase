
// include core Wiring API
#include "Arduino.h"
#include "Arducase.h"

#include "modes.h"
#include "flowmeters.h"

//============================================================================================================//
// ___________________________________________ PELTIERS __________________________________________________

Flowmeter mainloop_flow;


Flowmeter *flowmeters[MAX_NB_FLOW];
int nb_flowmeter = 0;


void create_flowmeter(struct Flowmeter &flowmeter, int pin_in, char *name){
  if (nb_flowmeter + 1 > MAX_NB_FLOW){
    return;
  }

  flowmeters[nb_flowmeter] = &flowmeter;
  nb_flowmeter++;

  flowmeter.pin_in = pin_in;
  flowmeter.name = name;

  pinMode(flowmeter.pin_in, OUTPUT);

}

void init_flowmeters(){
//        variable           Pin   Nom
  create_flowmeter(mainloop_flow    , 20 , "Mainloop");

}







// ___________________________________________ PELTIERS __________________________________________________
//============================================================================================================//
