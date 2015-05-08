#ifndef VENTILOS_H
#define VENTILOS_H

#include "cameras.h"

//=========================================================================================================//
// _______________________________________ VENTILATEURS __________________________________________________
#define MAX_NB_VENTILO 4
#define PWM_VENTILATEUR_MIN 25
typedef struct Ventilo {
  int pin;
  char *name;

  int is_tec;

  int curent_PWM;
  int last_PWM; // ancien curent_PWM (pour gestion du boost)

  int nb_cam;
  Camera *cameras[MAX_NB_CAM];
} Ventilo;

extern int nb_ventilos;
extern Ventilo *ventilos[MAX_NB_VENTILO];

void create_ventilo(struct Ventilo &ventilo, int pin, char *name);

void init_ventilos();

void init_ventilo_regulation();

void finish_ventilo_regulation();

void add_cam_to_ventilo(struct Camera &camera, struct Ventilo &ventilo);

// _______________________________________ VENTILATEURS __________________________________________________
//=========================================================================================================//


#endif
