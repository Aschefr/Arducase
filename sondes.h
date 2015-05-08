#ifndef SONDES_H
#define SONDES_H

#include "ventilos.h"
#include "modes.h"
 
//=============================================================================================================//
// _______________________________________ THERMALS SENSORS __________________________________________________
#define MAX_NB_THERMAL 10
#define PRECISION_SONDE 0.1 // precision de la sonde en degree celsuis
#define NB_HISTO_SONDE 50 // precision de la sonde en degree celsuis

typedef struct Termal_sensor {
  int pin; // on c'est branché
  char *name;
  float offset; // offset pour convertion
  int seuil_haut;
  int seuil_bas;
  int raw; // valeur brute lue
  float val; // valeur traduite en celcius

  float histo_val[NB_HISTO_SONDE];
  int last_histo;

  // ventilos associé a cette sonde
  Ventilo* ventilos[MAX_NB_VENTILO];
  int nb_ventilos;

} Termal_sensor;


extern Termal_sensor *sondes[MAX_NB_THERMAL];
extern int nb_sondes;

extern Termal_sensor temp_wtr_out_pc;
extern Termal_sensor temp_wtr_out_pcrad;
extern Termal_sensor temp_wtr_in_pc;
extern Termal_sensor temp_pc_case;

extern void thermals_save();

// Definition d'une structure contenant pour chaque sonde : le pin d'entrée, le nom, l'offset, le seuil bas et haut pour la régulation
void create_thermal(struct Termal_sensor &sensor, int pin_number, char *name, int offset = 0, int seuil_bas = 25, int seuil_haut = 35);

void add_ventilo_to_sonde(struct Termal_sensor &sensor, struct Ventilo &ventilo);

void init_thermals();

// _________________________________ Functions pour sondes thermiques _____________________________________________
void read_and_convert_termal_sensor(struct Termal_sensor &sensor);


void lcd_print_sonde(struct Termal_sensor* sensor);


// _______________________________________ THERMALS SENSORS __________________________________________________
//=============================================================================================================//


#endif
