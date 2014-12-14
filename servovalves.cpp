
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


void create_servovalve(struct Servovalve &servovalve, int pin_out, int pin_close, int pos_min, int pos_max, int offset, char *name){
  if (nb_servovalve + 1 > MAX_NB_VALVE){
    return;
  }

  servovalves[nb_servovalve] = &servovalve;
  nb_servovalve++;

  servovalve.servo_pin.attach(pin_out);
  servovalve.pin_close = pin_close;
  servovalve.pin_servo = pin_out;
  servovalve.pos_min = pos_min;
  servovalve.pos_max = pos_max;
  servovalve.offset = offset;
  servovalve.name = name;

  pinMode(servovalve.pin_close, INPUT);

  servovalve.servo_pin.write(pos_max);
}

void use_servo(struct Servovalve &servo_to_use, int cmd_value) { //se rappeler
  servo_to_use.servo_pin.attach(servo_to_use.pin_servo);
  servo_to_use.servo_pin.write(cmd_value);
  delay(500);
  servo_to_use.servo_pin.detach();
}

void init_servovalves(){
//        variable              Pin servo|Pin close|Pos min|Pos max | Offset |    Nom
  create_servovalve(servo_vrad  ,   11   ,   49   ,  155   ,   50   ,   0   , "Rad");
  create_servovalve(servo_vtec  ,   12   ,   51   ,   30   ,  140   ,  -6   , "TEC");

      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.println("Initialisation    ");

      lcd.setCursor(0, 1);
      lcd.println("valve position    ");

  // bouger de max vers min et s'arreter des qu'on trouve la fin de course, et enregistrer cette position.
  Servovalve* servo; //Servovalve* est un type, comme "int"
  for(int i = 0; i < nb_servovalve; ++i)
  {
    servo = servovalves[i];

    int j = servo->pos_max;
    do {
      if(servo->pos_max > servo->pos_min) {
        j = j - 1;
      } else {
        j = j + 1;
      }
      servo->servo_pin.write(j);
      delay(15);
    } while(digitalRead(servo->pin_close) != HIGH && j != servo->pos_min );

    if(j == servo->pos_min) {
      servo->pos_min = servo->pos_max; // PROBLEME LECTURE CAPTEUR ou SERVO !!!
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.println("Def ");
      lcd.setCursor(4, 0);
      lcd.println(servo->name);
      lcd.setCursor(14, 0);
      lcd.println("  ");      
      lcd.setCursor(0, 1);
      lcd.println("Open bypass vlve");
      
    } else {
      servo->pos_min = j + servo->offset;
      //affichage pos_min premier servo
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.println(servo_vrad.name);
      lcd.setCursor(3, 0);
      lcd.println(" clse pos  ");
      lcd.setCursor(13, 0);
      lcd.println(servo_vrad.pos_min);

      //affichage pos_min second servo
      lcd.setCursor(0, 1);
      lcd.println(servo_vtec.name);
      lcd.setCursor(3, 1);
      lcd.println(" clse pos  ");
      lcd.setCursor(13, 1);
      lcd.println(servo_vtec.pos_min);
      lcd.setCursor(15, 1);
      lcd.println("   ");

    }

    servo->servo_pin.write(servo->pos_max);
  }

  // on ferme vtec et on ouvre vrad au démarrage
  use_servo(servo_vtec, servo_vtec.pos_min + servo_vtec.offset); //fermeture vtec
  use_servo(servo_vrad, servo_vrad.pos_max + servo_vrad.offset); //ouverture vrad
  delay(15);
  //servo_vtec.servo_pin.write(servo_vtec.pos_min + 9); //ouverture de quelques points pour évité au servomoteur d'être mécaniquement en contrainte (évite aussi les parasites)
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
