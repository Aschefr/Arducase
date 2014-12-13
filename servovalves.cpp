
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


void create_servovalve(struct Servovalve &servovalve, int pin_out, int pin_close, int pos_min, int pos_max, char *name){
  if (nb_servovalve + 1 > MAX_NB_VALVE){
    return;
  }

  servovalves[nb_servovalve] = &servovalve;
  nb_servovalve++;

  servovalve.servo_pin.attach(pin_out);
  servovalve.pin_close = pin_close;
  servovalve.pos_min = pos_min;
  servovalve.pos_max = pos_max;
  servovalve.name = name;

  pinMode(servovalve.pin_close, INPUT);

  servovalve.servo_pin.write(pos_max);
}

void init_servovalves(){
//        variable           Pin servo  Pin close Pos min  Pos max   Nom
  create_servovalve(servo_vrad    ,  11  ,  49   ,    0  ,  160    , "servo_vrad");
  create_servovalve(servo_vtec    ,  12  ,  51   ,  180  ,   60    , "servo_vtec");

  // bouger de max vers min et s'arreter des qu'on trouve la fin de course

  Servovalve servo;
  for (int i = 0; i < nb_servovalve; ++i)
  {
    servo = *servovalves[i];

    int j = servo.pos_max;
    do {
      if(servo.pos_max > servo.pos_min) {
        j = j - 5;
      } else {
        j = j + 5;
      }
      servo.servo_pin.write(j);
      delay(15);
    } while(digitalRead(servo.pin_close) != HIGH || j != servo.pos_min );

    if(j != servo.pos_min) {
      servo.servo_pin.write(servo.pos_max);
      servo.pos_min = servo.pos_max;
      // PROBLEME LECTURE CAPTEUR ou SERVO !!!
    } else {
      servo.pos_min = j;
    }
  }

}


// _________________________________ Initialisation servo _____________________________________________



/*
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
