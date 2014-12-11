
// include core Wiring API
#include "Arduino.h"
#include "Arducase.h"

#include "modes.h"
#include "peltiers.h"

//============================================================================================================//
// ___________________________________________ PELTIERS __________________________________________________

Peltier tec_0_1;
Peltier tec_2;
Peltier tec_3;
Peltier tec_4;
Peltier tec_5;

Peltier *peltiers[MAX_NB_TEC];
int nb_peltier = 0;


void create_peltier(struct Peltier &peltier, int pin_out, char *name){
  if (nb_peltier + 1 > MAX_NB_TEC){
    return;
  }

  peltiers[nb_peltier] = &peltier;
  nb_peltier++;

  peltier.pin_out = pin_out;
  peltier.name = name;

  pinMode(peltier.pin_out, OUTPUT);

}

void init_peltiers(){
//        variable           Pin   Nom
  create_peltier(tec_0_1    , 42 , "1, 2");
  create_peltier(tec_2      , 44 , "3"   );
  create_peltier(tec_3      , 46 , "4"   );
  create_peltier(tec_4      , 48 , "5"   );
  create_peltier(tec_5      , 50 , "6"   );

}

void tec_drive(){
  if (digitalRead(sw_tec) == 1 && nb_modes == 4){
    if (digitalRead(sw_cooling_auto) == 1){

    }
  }
}








// ___________________________________________ PELTIERS __________________________________________________
//============================================================================================================//
