
// include core Wiring API
#include "Arduino.h"
#include "Arducase.h"

#include "servovalves.h"

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
  servovalve.name = name;

  pinMode(servovalve.pin_out, OUTPUT);
  pinMode(servovalve.pin_close, INPUT);

}

void init_servovalves(){
//        variable           Pin servo  Pin close    Nom
  create_servovalve(servo_vrad    ,  11  ,  49  , "servo_vrad");
  create_servovalve(servo_vtec    ,  12  ,  51  , "servo_vtec");



// _________________________________ Initialisation servo _____________________________________________




int val;


  valve_rad.attach(servovalve.servo_vrad.pin_out);
  valve_tec.attach(servovalve.servo_vtec.pin_out);


  val = 500;            //test value
  val = map(val, 0, 1023, 0, 179);     // scale it to use it with the servo (value between 0 and 180) 
  valve_tec.write(val);                  // sets the servo position according to the scaled value 


// Controlling a servo position using a potentiometer (variable resistor) 
// by Michal Rinott <http://people.interaction-ivrea.it/m.rinott> 
/*
Servo myservo;  // create servo object to control a servo 
 
int potpin = 0;  // analog pin used to connect the potentiometer
int val;    // variable to read the value from the analog pin 
 
void setup() 
{ 
  myservo.attach(9);  // attaches the servo on pin 9 to the servo object 
} 
 
void loop() 
{ 
  val = analogRead(potpin);            // reads the value of the potentiometer (value between 0 and 1023) 
  val = map(val, 0, 1023, 0, 179);     // scale it to use it with the servo (value between 0 and 180) 
  myservo.write(val);                  // sets the servo position according to the scaled value 
  delay(15);                           // waits for the servo to get there 
} 
*/


// _________________________________ Initialisation servo _____________________________________________


}
// ___________________________________________ PELTIERS __________________________________________________
//============================================================================================================//
