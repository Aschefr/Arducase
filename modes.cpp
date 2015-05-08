
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
  create_mode(mode_silent   , 23, 22, "Silent"    , 0 , 30  ); //En mode Silent, pourcentage de marche ventilateur maximal
  create_mode(mode_normal   , 25, 24, "Normal"    , 0 , 80  ); //En mode Normal, pourcentage de marche ventilateur maximal
  create_mode(mode_heavy    , 27, 26, "Heavy"     , 20 , 100  ); //En mode Heavy, pourcentage de marche ventilateur maximal
  create_mode(mode_extreme  , 29, 28, "Extreme"   , 90 , 100 ); //En mode Extreme, pourcentage de marche ventilateur maximal

  // alume la led du mode selectionné dès le debut
  digitalWrite(selected_mode->pin_led, HIGH);
}
// _______________________________________ REGULATION MODE __________________________________________________
//============================================================================================================//
