#ifndef CAMERAS_H
#define CAMERAS_H

 
//============================================================================================================//
// ___________________________________________ CAMERAS __________________________________________________
#define MAX_NB_CAM 5 //Nombre de cameras de régulation maximum

typedef struct Camera { //définition d'une structure qui comporte :
  int pin_button; //le pin du bouton
  int pin_led; //le pin de la led
  char *name; //le nom de la caméra
  int groupe;
  int current_val; 
} Camera;


extern Camera *cameras[MAX_NB_CAM]; // Tableau de pointeur vers l'objet "Camera", ou plutôt un tableau de pointeur vers les objets "Camera", sert à lister tout les cameras, en faire l'inventaire.
extern int nb_cam; //Variable pour compter le nombre de caméras, utilisé plus bas


/*
 * Fonction permetant de créer un "camera" et initialiser l'objet avec les propriétés définies plus bas dans "Gestion des Cameras"
 */
void create_camera(struct Camera &camera, int pin_button, int pin_led, char *name, int groupe);

void init_cameras();

// ___________________________________________ CAMERAS __________________________________________________
//============================================================================================================//


#endif