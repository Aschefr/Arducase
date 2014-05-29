
// include core Wiring API
#include "Arduino.h"
#include "Arducase.h"
#include "cameras.h"
#include "ventilos.h"

//============================================================================================================//
// ___________________________________________ CAMERAS __________________________________________________


extern Ventilo vent_pc;
extern Ventilo vent_rad;
extern Ventilo vent_wc;


// Définition des cameras  (déclaration des variables pour chaque camera)
Camera camera1;
Camera camera2;
Camera camera3;
Camera camera4;

//Camera *selected_cam = camera1; // Variable de type "pointeur" vers le camera selectionné.

Camera *cameras[MAX_NB_CAM]; // Tableau de pointeur vers l'objet "Camera", ou plutôt un tableau de pointeur vers les objets "Camera", sert à lister tout les cameras, en faire l'inventaire.
int nb_cam = 0; //Variable pour compter le nombre de caméras, utilisé plus bas


/*
 * Fonction permetant de créer un "camera" et initialiser l'objet avec les propriétés définies plus bas dans "Gestion des Cameras"
 */
void create_camera(struct Camera &camera, int pin_button, int pin_led, char *name, int groupe){
  if (nb_cam + 1 > MAX_NB_CAM){ //Si nb_cam + 1 est supérieur au nombre de camera maxi,
    return; //Arrête la fonction, ne retourne rien.
  }

  //Sinon,
  cameras[nb_cam] = &camera; // ajout du camera dans l'invertaire de tous les cameras
  nb_cam++; //Incrémenter de 1 "nb_cam"

  camera.pin_button = pin_button; //Stocker dans "pin_button" qui se trouve dans "camera" la valeur de "pin_button" -> celle lu plus bas dans "Gestion des Cameras"
  camera.pin_led = pin_led; //Pareil
  camera.name = name; //Idem
  camera.groupe = groupe;


  pinMode(camera.pin_button, INPUT); //définir le pin du bouton comme une entrée
  pinMode(camera.pin_led, OUTPUT); //et le pin de la led comme une sortie, sinon ça marche pas.
}

void init_cameras(){

  // _________________________________ Gestion des Cameras _____________________________________________
  //          variable      btn led    nom             groupe
  create_camera(camera1   , 31, 30, "Camera PC"        ,1 ); //
  create_camera(camera2   , 33, 32, "Cam watercase 1"  ,1 ); //
  create_camera(camera3   , 35, 34, "Cam radiateur"    ,2 ); //
  create_camera(camera4   , 37, 36, "Cam watercase 2"  ,2 ); //


  // _________________________________ Association des cameras avec les ventilos _____________________________________________
  //                  cam        ventilos
  add_cam_to_ventilo(camera1   , vent_pc);
  add_cam_to_ventilo(camera2   , vent_rad);
  add_cam_to_ventilo(camera3   , vent_wc);


  // alume la led du camera selectionné dès le debut
  //digitalWrite(selected_cam->pin_led, HIGH);
}
// ___________________________________________ CAMERAS __________________________________________________
//============================================================================================================//
