
// include core Wiring API
#include "Arduino.h"
#include "Arducase.h"
#include "modes.h"

//============================================================================================================//
// _______________________________________ REGULATION MODE __________________________________________________

// Définition des modes  (déclaration des variables pour chaque mode)
Mode mode_silent;
Mode mode_normal;
Mode mode_heavy;
Mode mode_extreme;

Mode *selected_mode = &mode_silent; // Variable de type "pointeur" vers le mode selectionné, ici, = &mode_silent pour démarré avec le mode "silent" au lancement de la carte.

Mode *modes[MAX_NB_MODES]; // Tableau de pointeur vers l'objet "Mode", ou plutôt un tableau de pointeur vers les objets "Mode", sert à lister tout les modes, en faire l'inventaire.
int nb_modes = 0; //Variable pour compter le nombre de modes, utilisé plus bas


/*
 * Fonction permetant de créer un "mode" et initialiser l'objet avec les propriétés définies plus bas dans "Gestion des Modes"
 */
void create_mode(struct Mode &mode, int pin_button, int pin_led, char *name, int pourcentage_min, int pourcentage_max){
  if (nb_modes + 1 > MAX_NB_MODES){ //Si nb_mode + 1 est supérieur au nombre de mode maxi,
    return; //Arrête la fonction, ne retourne rien.
  }

  //Sinon,
  modes[nb_modes] = &mode; // ajout du mode dans l'invertaire de tous les modes
  nb_modes++; //Incrémenter de 1 "nb_mode"

  mode.pin_button = pin_button; //Stocker dans "pin_button" qui se trouve dans "mode" la valeur de "pin_button" -> celle lu plus bas dans "Gestion des Modes"
  mode.pin_led = pin_led; //Pareil
  mode.name = name; //Idem
  mode.pourcentage_min = pourcentage_min; //Idem
  mode.pourcentage_max = pourcentage_max; //Idem


  pinMode(mode.pin_button, INPUT); //définir le pin du bouton comme une entrée
  pinMode(mode.pin_led, OUTPUT); //et le pin de la led comme une sortie, sinon ça marche pas.

}

void init_modes(){

  // _________________________________ Gestion des Modes _____________________________________________
  //          variable        btn led    nom        min  max
  create_mode(mode_silent   , 23, 22, "Silent"    , 0  , 30  ); //En mode Silent, pourcentage de marche ventilateur maximal
  create_mode(mode_normal   , 25, 24, "Normal"    , 30 , 50  ); //En mode Normal, pourcentage de marche ventilateur maximal
  create_mode(mode_heavy    , 27, 26, "Heavy"     , 50 , 70  ); //En mode Heavy, pourcentage de marche ventilateur maximal
  create_mode(mode_extreme  , 29, 28, "Extreme"   , 70 , 100 ); //En mode Extreme, pourcentage de marche ventilateur maximal

  // alume la led du mode selectionné dès le debut
  digitalWrite(selected_mode->pin_led, HIGH);
}

void mode_up() {
  for (int i = 0; i < nb_modes - 1; ++i){
    if(selected_mode == modes[ i ]) {
      switch_mode(modes[ i + 1 ]);
      return;
    }
  }
}
void mode_down() {
  for (int i = 1; i < nb_modes ; ++i){
    if(selected_mode == modes[ i ]) {
      switch_mode(modes[ i - 1 ]);
      return;
    }
  }
}

void switch_mode(Mode *mode_appuye) {

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

// _______________________________________ REGULATION MODE __________________________________________________
//============================================================================================================//
