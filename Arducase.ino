
#include <LiquidCrystal.h>
#include "Arducase.h"

#include "modes.h"
#include "ventilos.h"
#include "cameras.h"
#include "sondes.h"
#include "peltiers.h"

LiquidCrystal lcd(14, 15, 16, 17, 18, 19);




void set_camera(void) {

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
      }
    }

    // on ralume la camera appuyé
    digitalWrite(camera_appuye->pin_led, HIGH);

    // on affiche le changement de cam
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print(camera_appuye->name);
    lcd.setCursor(0, 1);
    lcd.print("Load");

    delay(1000);
    lcd.clear();
  }
}



// ________________________________ Entrée/sortie Arduino Mega _______________________________________




// ------------------------------------------------------------------------------------ Défilement LCD :
const int btn_next = 39; //Bouton Next LCD
const int btn_prev = 41; //Bouton Prev LCD

const int led_next = 38; //Led du bouton Next
const int led_prev = 40; //Led du bouton Prev

// --------------------------------------------------------------------------------- Mode Auto ou Manu :
const int sw_cooling_auto = 43; //Entrée switch cooling mode Auto ou Manu, Manu = 0, Auto = 1
const int sw_video_auto = 43; //Entrée switch Video mode Auto ou Manu, Manu = 0, Auto = 1
const int sw_tec = 43; //Entrée switch TEC mode On ou Off, Off = 0, On = 1

// ------------------------------------------------------------------------------------ Activation TEC :
const int tec_relay = 42; //Sortie pour activation relais TEC
const int tec_pump = 52; //Sortie pour pompe refroidissement TEC face chaude.

// ----------------------------------------------------------------------------- Gestion electrovannes :
const int servo_valve1 = 11; //Gestion du servo de vanne 1
const int servo_valve2 = 12; //Gestion du servo de vanne 2

const int valve1_close = 49; //Retour d'état vanne 1 fermé
const int valve2_close = 51; //Retour d'état vanne 2 fermé

// ---------------------------------------------------------------------------------------- Débitmetre :
const int flowmeter_read = 53; //Lecture débit

// ---------------------------------------------------------------------------------- Gestion vumetres :
const int vu_cpu = 4; //Vumetre % cpu utilisé
const int vu_ram = 6; //Vumetre % ram utilisé
const int vu_eau = 7; //Vumetre eau pc en °C
const int vu_flow = 8; //Vumetre débit eau
const int vu_eff = 9; //Vumetre efficacité refroidissement
const int vu_ech = 10; //Vumetre echauffement de l'eau après passage dans pc
// ________________________________ Entrée/sortie Arduino Mega _______________________________________





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

    delay(1000);
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
  

  // on initialise les modes et tout
  // l'ordre est important !!!!!
  init_modes();
  init_ventilos();
  init_thermals();
  init_cameras();
  init_peltiers();


}
// _______________________________________ Setup _______________________________________


// _______________________________________ Main Loop _______________________________________
void loop (void) {
  set_mode();
  set_camera();
  lcd_temp_draw();
  thermals_save();
}
// _______________________________________ Main Loop _______________________________________

