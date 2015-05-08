#ifndef MODES_H
#define MODES_H

 
//============================================================================================================//
// _______________________________________ REGULATION MODE __________________________________________________
#define MAX_NB_MODES 5 //Nombre de modes de régulation maximum
typedef struct Mode { //définition d'une structure qui comporte :
  int pin_button; //le pin du bouton
  int pin_led; //le pin de la led
  char *name; //le nom du mode
  int pourcentage_min; // pourcentage min des ventilateurs dans le mode
  int pourcentage_max; // pourcentage max des ventilateurs dans le mode
} Mode;

// Définition des modes  (déclaration des variables pour chaque mode)
extern Mode *selected_mode; // Variable de type "pointeur" vers le mode selectionné, ici, = &mode_silent pour démarré avec le mode "silent" au lancement de la carte.

extern Mode *modes[MAX_NB_MODES]; // Tableau de pointeur vers l'objet "Mode", ou plutôt un tableau de pointeur vers les objets "Mode", sert à lister tout les modes, en faire l'inventaire.
extern int nb_modes; //Variable pour compter le nombre de modes, utilisé plus bas

extern Mode mode_extreme;

/*
 * Fonction permetant de créer un "mode" et initialiser l'objet avec les propriétés définies plus bas dans "Gestion des Modes"
 */
void create_mode(struct Mode &mode, int pin_button, int pin_led, char *name, int pourcentage_min = 0, int pourcentage_max = 100);
void init_modes();
// _______________________________________ REGULATION MODE __________________________________________________
//============================================================================================================//


#endif