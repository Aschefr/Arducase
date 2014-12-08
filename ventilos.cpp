
// include core Wiring API
#include "Arduino.h"
#include "Arducase.h"
#include "ventilos.h"
#include "modes.h"

//=========================================================================================================//
// _______________________________________ VENTILATEURS __________________________________________________

Ventilo vent_pc;
Ventilo vent_rad;
Ventilo vent_wc;

Ventilo *ventilos[MAX_NB_VENTILO];
int nb_ventilos = 0;


void create_ventilo(struct Ventilo &ventilo, int pin, char *name){
  if (nb_ventilos + 1 > MAX_NB_VENTILO){
    return;
  }

  ventilos[nb_ventilos] = &ventilo;
  nb_ventilos++;

  ventilo.pin = pin;
  ventilo.name = name;
  ventilo.curent_PWM = 0;
  ventilo.last_PWM = 0;

  ventilo.nb_cam = 0;
}

void init_ventilos(){

  // _________________________________ Gestion des Ventilos _____________________________________________
  //             variable   pin   nom
  create_ventilo(vent_pc  , 2  , "vent_pc"  ); //Groupe de ventilateur à l'intérieur du pc
  create_ventilo(vent_rad , 3  , "vent_rad" ); //Groupe de ventilateur du radiateur PC
  create_ventilo(vent_wc  , 5  , "vent_wc"  ); //Groupe de ventilateur de la watercase
}


void init_ventilo_regulation(){
  int min = percent_to_PWM(selected_mode->pourcentage_min);
  for (int i = 0; i < nb_ventilos; ++i) {
    ventilos[i]->last_PWM = ventilos[i]->curent_PWM;
    ventilos[i]->curent_PWM = min;
  }
}
void finish_ventilo_regulation(){

  int nb_boost = 0;
  for (int i = 0; i < nb_ventilos; ++i) {
    if (ventilos[i]->curent_PWM < PWM_VENTILATEUR_MIN){ // on est en dessous de la capacite du ventilateur, on l'arrete
      analogWrite(ventilos[i]->pin, 0);
      ventilos[i]->curent_PWM = 0;
    }
    else {
      if (ventilos[i]->last_PWM < PWM_VENTILATEUR_MIN ){ //On est juste au dessus de la capacité du ventilateur, on le démarre avec un boost à fond pendant 200ms
        analogWrite(ventilos[i]->pin, 255);
        for (int n = 0; n < ventilos[i]->nb_cam; ++n) {
          ventilos[i]->cameras[n]->fresh_boosted ++;
        }

        nb_boost ++;
      }
      else {
        analogWrite(ventilos[i]->pin, ventilos[i]->curent_PWM);
      }  
    }
  }

  if (nb_boost > 0){
    delay(200);
  }
}

void add_cam_to_ventilo(struct Camera &camera, struct Ventilo &ventilo){ //Ajout du ventilateur à la caméra correspondante à allumer lorsque celui-ci se mets en route.
  if( ventilo.nb_cam + 1 > MAX_NB_CAM ){
    return;
  }

  ventilo.cameras[ ventilo.nb_cam ] = &camera;
  ventilo.nb_cam ++;

}

// _______________________________________ VENTILATEURS __________________________________________________
//=========================================================================================================//
