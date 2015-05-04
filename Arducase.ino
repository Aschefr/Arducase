
#include <LiquidCrystal.h>
#include <Servo.h>
#include "Arducase.h"


#include "modes.h"
#include "ventilos.h"
#include "cameras.h"
#include "sondes.h"
#include "peltiers.h"
#include "vumetre.h"
#include "servovalves.h"
#include "flowmeters.h"

LiquidCrystal lcd(14, 15, 16, 17, 18, 19);


// ________________________________ Entrée/sortie Arduino Mega _______________________________________




// ------------------------------------------------------------------------------------ Défilement LCD :
const int btn_next = 39; //Bouton Next LCD
const int btn_prev = 41; //Bouton Prev LCD

const int led_next = 38; //Led du bouton Next
const int led_prev = 40; //Led du bouton Prev

// --------------------------------------------------------------------------------- Mode Auto ou Manu :
const int sw_cooling_auto = 43; //Entrée switch cooling mode Auto ou Manu, Manu = 0, Auto = 1
const int sw_video_auto = 45; //Entrée switch Video mode Auto ou Manu, Manu = 0, Auto = 1
const int sw_tec = 47; //Entrée switch TEC mode On ou Off, Off = 0, On = 1

// ------------------------------------------------------------------------------------ Activation TEC :

const int tec_pump = 52; //Sortie pour pompe refroidissement TEC face chaude.

// ---------------------------------------------------------------------------------------- Débitmetre :
//const int flow = 11; //Lecture débit


// ________________________________ Entrée/sortie Arduino Mega _______________________________________


// ________________________________ Fonction gesion des temps/tempo _______________________________________

//void tempo(int action, int tempo)



// ________________________________ Fonction gesion des temps/tempo _______________________________________




// ________________________________ Gestion caméras _______________________________________

void set_camera(void) {

  if (digitalRead(sw_video_auto) == 0) { //Mode Manuel

    int nb_appuye = 0;
    Camera *camera_appuye;
    for (int i = 0; i < nb_cam; ++i){
      if (digitalRead(cameras[i]->pin_button) == HIGH){
        nb_appuye++;
        camera_appuye = cameras[i];
      }
    }

    // si on a bien un boutons appuyé
    if (nb_appuye == 1){

      // on eteinds toutes les leds du même groupe
      for (int i = 0; i < nb_cam; ++i){
        if (cameras[i]->groupe == camera_appuye->groupe){
          digitalWrite(cameras[i]->pin_led, LOW);
          if (cameras[i]->pin_led != camera_appuye->pin_led){
            cameras[i]->current_val = 0;
          }
        }
      }

      // on ralume la camera appuyé si elle n'était pas déjà selectionné
      if(camera_appuye->current_val == 0) {
        digitalWrite(camera_appuye->pin_led, HIGH);
      }
      camera_appuye->current_val = 1 - camera_appuye->current_val;

      // on affiche le changement de cam
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.println(camera_appuye->name);
      //lcd.setCursor(0, 1);
      //lcd.print("Load");

      delay(500);
      lcd.clear();
    }
  }
  else if (digitalRead(sw_video_auto) == 1) { //Mode Auto

    unsigned long now = millis();

    for (int i = 0; i < nb_cam; ++i) {
      if( cameras[i]->fresh_boosted > 0 ) {

        for (int n = 0; n < nb_cam; ++n) {
          if ( cameras[n]->groupe == cameras[i]->groupe && n != i ){
            cameras[n]->fresh_boosted = 0;
            digitalWrite(cameras[n]->pin_led, LOW);
            cameras[n]->current_val = 0;
            cameras[n]->auto_time = 0;
          }
        }

        digitalWrite(cameras[i]->pin_led, HIGH);
        cameras[i]->current_val = 1;
        cameras[i]->fresh_boosted = 0;
        cameras[i]->auto_time = now + AUTO_TIME_CAM;
        //Serial.println(cameras[i]->fresh_boosted);
      }
    }

    for (int i = 0; i < nb_cam; ++i) {
      if (cameras[i]->current_val && now > cameras[i]->auto_time){
        digitalWrite(cameras[i]->pin_led, LOW);
        cameras[i]->current_val = 0;
        cameras[i]->auto_time = 0;
      }
    }
  }

}

// ________________________________ Gestion caméras _______________________________________





// __________ Gestion mode de fonctionnement, allumage LED et inscription LCD __________
//


void set_mode (void) { 

  int nb_appuye = 0;
  Mode *mode_appuye;
  for (int i = 0; i < nb_modes; ++i){
    if (digitalRead(modes[i]->pin_button) == HIGH){
      nb_appuye++;
      mode_appuye = modes[i];
    }
  }

  // si on a bien un seul bouton appuyé
  if (nb_appuye == 1){
    // on set le mode actuel
    selected_mode = mode_appuye;

    // on eteinds toutes les leds
    for (int i = 0; i < nb_modes; ++i){
      digitalWrite(modes[i]->pin_led, LOW);
    }
    // on ralume le mode selectionné
    digitalWrite(selected_mode->pin_led, HIGH);

    // on affiche engage
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print(selected_mode->name);
    lcd.setCursor(0, 1);
    lcd.print("Engage");

    delay(500);
    lcd.clear();
  }
}
// __________ Gestion mode de fonctionnement, allumage LED et inscription LCD __________



void thermals_save (void) { //Enregistrement température en Celsius dans chaque variables

  init_ventilo_regulation();

  for (int i = 0; i < nb_sondes; ++i){
    read_and_convert_termal_sensor(*sondes[i]);
  }

  finish_ventilo_regulation();
}


//=========================================================================================//
// ________________ Affichage des temps sur LCD avec bouton Refresh ______________________

void lcd_temp_draw (void) {
 
  static int screens=0; //Compteur pour changer d'écran
  static int yazop = 0; //Pour fonctionnement du bouton "next" au relachement

  if (digitalRead(btn_next) == HIGH)
    yazop = 1;

  if ( digitalRead(btn_next)==LOW && yazop==1 ) {
    //Serial.println(screens);
    
    screens++;
    if (screens < 0 || screens > nb_sondes){
      screens = 0;
    }

    if (screens == 0){ // on eteind l'ecran
      lcd.clear();
    }
    else {
      lcd_print_sonde(sondes[screens - 1]); //Affichage de l'écran demandé
    }

    yazop=0;
  }
  
  static long unsigned last_refresh = 0;
  if (screens > 0 && (millis()/250) - last_refresh > 1){
    last_refresh = (millis()/250);

    lcd_print_sonde(sondes[screens - 1]);

  /*if (last_refresh > 30000)
    last_refresh = 0;
    */

    #if DEBUG 
      Serial.print(sondes[screens - 1]->name);
      Serial.print(" = ");
      Serial.print(sondes[screens - 1]->val);
      Serial.println(" °C");

      Serial.print("millis_func = ");
      Serial.println(millis());

      Serial.print("last_refresh = ");
      Serial.println(last_refresh);

      for (int i = 0; i < sondes[screens - 1]->nb_ventilos; ++i){

        Serial.print("=> ");
        Serial.print(sondes[screens - 1]->ventilos[i]->name);
        Serial.print(" = ");
        Serial.print(PWM_to_percent( sondes[screens - 1]->ventilos[i]->curent_PWM ));
        Serial.println("%");
      }
    #endif
  }

}
// ________________ Affichage des temps sur LCD avec bouton Refresh ______________________
//=========================================================================================//


// ________________ Calcul flow ______________________
//=====================================================//
//volatile int NbTopsFan; //measuring the rising edges of the signal
//int Calc;                               
//int hallsensor = 20;    //The pin location of the sensor
// 
//void rpm ()     //This is the function that the interupt calls 
//{ 
//  NbTopsFan++;  //This function measures the rising and falling edge of the 
//} 
//=====================================================//
// ________________ Calcul flow ______________________




// _______________________________________ Setup _______________________________________
void setup (void)
{
  // set up the LCD's number of columns and rows: 
  lcd.begin(16, 2);
  
  // initialize serial communication at 9600 bits per second :
  Serial.begin(9600);
  
  /*
  Fréquence de la PWM :
  TCCR0B est pour le pin 4
  TCCR1B est pour le pin 11, 12, 13 (le pwm 13 est déconseillé, il fonctionne mal)
  TCCR2B est pour le pin 9, 10
  TCCR3B est pour le pin 2, 3, 5
  TCCR4B est pour le pin 6, 7, 8

  Inclure ce code dans le setup :
  int prescalerVal = 0x07;
  TCCRnB &= ~prescalerVal;
  
  prescalerVal = 1;
  TCCRnB |= prescalerVal;
  Remplacer le "n" de "TRCCRnB" par 0, 1, 2, 3 ou 4 selon les pin que vous souhaitez modifier.
  Ce code set la fréquence maximal de la PWM sur les pin concernés.
  */

  //First clear all three prescaler bits:
  int prescalerVal = 0x07; //create a variable called prescalerVal and set it equal to the binary number "00000111"
  TCCR3B &= ~prescalerVal; //AND the value in TCCR0B with binary number "11111000"

  //Now set the appropriate prescaler bits:
  prescalerVal = 1; //set prescalerVal equal to binary number "00000001"
  TCCR3B |= prescalerVal; //OR the value in TCCR0B with binary number "00000001"
  

  pinMode(sw_video_auto, INPUT);
  pinMode(sw_cooling_auto, INPUT);
  pinMode(sw_tec, INPUT);
  pinMode(tec_pump, OUTPUT);

//Flow
//  pinMode(hallsensor, INPUT); //initializes digital pin 2 as an input
//  Serial.begin(9600); //This is the setup function where the serial port is 
//  attachInterrupt(3, rpm, RISING); //and the interrupt is attached

  // on initialise les modes et tout
  // l'ordre est important !!!!!
  init_modes();
  init_ventilos();
  init_thermals();
  init_cameras();
  init_peltiers();
  init_vumetres();
  init_servovalves();


}
// _______________________________________ Setup _______________________________________


// _______________________________________ Main Loop _______________________________________
void loop (void) {
  set_mode();
  lcd_temp_draw();
  thermals_save();
  set_camera(); // après thermal save
  handle_cpu_mem();

//Flow
//  NbTopsFan = 0;	//Set NbTops to 0 ready for calculations
//  sei();		//Enables interrupts
//  delay (1000);	//Wait 1 second
//  cli();		//Disable interrupts
//  Calc = (NbTopsFan * 60 / 7.5); //(Pulse frequency x 60) / 7.5Q, = flow rate 
//  Serial.print (Calc, DEC); //Prints the number calculated above
//  Serial.print (" L/min\r\n"); //Prints "L/hour" and returns a  new line
  

  //digitalWrite(tec_pump, HIGH); //Allume pompe TEC

}
// _______________________________________ Main Loop _______________________________________

