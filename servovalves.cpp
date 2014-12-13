
// include core Wiring API
#include "Arduino.h"
#include "Arducase.h"

#include "servovalves.h"
#include <Servo.h>

//============================================================================================================//
// ___________________________________________ PELTIERS __________________________________________________

Servovalve servo_vrad; //Vanne vers radiateur
Servovalve servo_vtec; //Vanne vers TECs (peltier)


Servovalve *servovalves[MAX_NB_VALVE];
int nb_servovalve = 0;


void create_servovalve(struct Servovalve &servovalve, int pin_out, int pin_close, char *name){
  if (nb_servovalve + 1 > MAX_NB_VALVE){
    return;
  }

  servovalves[nb_servovalve] = &servovalve;
  nb_servovalve++;

  servovalve.pin_out = pin_out;
  servovalve.pin_close = pin_close;
  servovalve.name = name;

  pinMode(servovalve.pin_out, OUTPUT);
  pinMode(servovalve.pin_close, INPUT);

}

void init_servovalves(){
//        variable           Pin servo  Pin close    Nom
  create_servovalve(servo_vrad    ,  11  ,  49  , "servo_vrad");
  create_servovalve(servo_vtec    ,  12  ,  51  , "servo_vtec");

  init_valves();
}


// _________________________________ Initialisation servo _____________________________________________



void init_valves(){
Servo valve_rad;
Servo valve_tec;

int val;

//valve_rad.attach(servo_vrad.pin_out);
valve_tec.attach(11);

  val = 50;            // reads the value of the potentiometer (value between 0 and 1023) 
  val = map(val, 0, 100, 60, 160);     // scale it to use it with the servo (value between 0 and 180) 
  valve_tec.write(val);                  // sets the servo position according to the scaled value 
  delay(15); 

}

/*
if (servo_vrad.pin_close != 1){ //Test de la vanne RAD
  for(pos = MIN_POS; pos < MAX_POS; pos += 1)  // goes from 0 degrees to 180 degrees 
  {                                  // in steps of 1 degree 
    valve_rad.write(pos);              // tell servo to go to position in variable 'pos' 
    delay(15);                       // waits 15ms for the servo to reach the position 
  } 
  for(pos = MAX_POS; pos>=1; pos-=1)     // goes from 180 degrees to 0 degrees 
  {                                
    valve_rad.write(pos);              // tell servo to go to position in variable 'pos' 
    delay(15);                       // waits 15ms for the servo to reach the position 
  } 

}
*/


// _________________________________ Initialisation servo _____________________________________________



// ___________________________________________ PELTIERS __________________________________________________
//============================================================================================================//
