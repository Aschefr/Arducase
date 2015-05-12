#ifndef VUMETRE_H
#define VUMETRE_H



//============================================================================================================//
// ___________________________________________ VUMETRES __________________________________________________
#define MAX_NB_VUMETRE 6
#define NB_CPU_SMOOTH 10
#define TIME_BTW_TWO_READ 10 //temps entre deux lectures sonde en secondes
typedef struct Vumetre{
  int pin_out;
  char *name;
} Vumetre;


extern Vumetre vu_tec;

void create_vumetre(struct Vumetre &vumetre, int pin_out, char *name);

void init_vumetres();

void handle_cpu_mem();

void screenSaver();

// ___________________________________________ VUMETRES __________________________________________________
//============================================================================================================//


#endif
