
// include core Wiring API
#include "Arduino.h"
#include "Arducase.h"

#include "modes.h"
#include "flowmeters.h"

//============================================================================================================//
// ___________________________________________ PELTIERS __________________________________________________

Flowmeter mainloop_low;


Flowmeter *flowmeters[MAX_NB_TEC];
int nb_flowmeter = 0;


void create_flowmeter(struct Flowmeter &flowmeter, int pin_out, char *name){
  if (nb_flowmeter + 1 > MAX_NB_TEC){
    return;
  }

  flowmeters[nb_flowmeter] = &flowmeter;
  nb_flowmeter++;

  flowmeter.pin_out = pin_out;
  flowmeter.name = name;

  pinMode(flowmeter.pin_out, OUTPUT);

}

void init_flowmeters(){
//        variable           Pin   Nom
  create_flowmeter(tec_0_1    , 42 , "1, 2");
  create_flowmeter(tec_2      , 44 , "3"   );
  create_flowmeter(tec_3      , 46 , "4"   );
  create_flowmeter(tec_4      , 48 , "5"   );
  create_flowmeter(tec_5      , 50 , "6"   );

}
/*
void tec_drive(){
  if (digitalRead(sw_tec) == 1 && nb_modes == 4){
    if (digitalRead(sw_cooling_auto) == 1){

    }
  }
}
*/







// ___________________________________________ PELTIERS __________________________________________________
//============================================================================================================//
