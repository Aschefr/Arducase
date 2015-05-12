
// include core Wiring API
#include "Arduino.h"
#include "Arducase.h"

#include "vumetre.h"
#include "sondes.h"
#include "flowmeters.h"

//============================================================================================================//
// ___________________________________________ VUMETRES __________________________________________________

Vumetre vu_cpu;
Vumetre vu_eau;
Vumetre vu_ram;
Vumetre vu_flow;
Vumetre vu_heat;
Vumetre vu_tec;

extern int screens;

Vumetre *vumetres[MAX_NB_VUMETRE];
int nb_vumetre = 0;


void create_vumetre(struct Vumetre &vumetre, int pin_out, char *name){
  if (nb_vumetre + 1 > MAX_NB_VUMETRE){
    return;
  }

  vumetres[nb_vumetre] = &vumetre;
  nb_vumetre++;

  vumetre.pin_out = pin_out;
  vumetre.name = name;

  pinMode(vumetre.pin_out, OUTPUT);

}

void init_vumetres(){
//        variable           Pin   Nom
  create_vumetre(vu_cpu      , 4  , "cpu"    );
  create_vumetre(vu_eau      , 7  , "eau"    );
  create_vumetre(vu_ram      , 6  , "ram"    );
  create_vumetre(vu_flow     , 8  , "flow"   );
  create_vumetre(vu_heat     , 9  , "heat"   );
  create_vumetre(vu_tec      , 10 , "tec"    );
}

// Section pour affichage CPU et MEM Usage -------------------------
void handle_cpu_mem(){

  int val = 0;                  //reading

  static char buffer[5];         //buffer
  static int cpuIndex = 0;
  static int cpuTotal = 0;
  static int cpuReadings[NB_CPU_SMOOTH];        //array of cpu load% readings

  static int first = 1;
  if(first){
    for (int i = 0; i < NB_CPU_SMOOTH; ++i) {
      cpuReadings[i] = 0;
    }
    buffer[4] = '\0';
    first = 0;
  }
  
  if(Serial.available() > 0) {

    Serial.readBytesUntil('\r', buffer, 5);

    switch (buffer[0])
    {
      case 'C':
        //CPU - value is "smoothed".  See tutorial for more info:
        //http://arduino.cc/en/Tutorial/Smoothing
        cpuTotal = cpuTotal - cpuReadings[cpuIndex];
        cpuReadings[cpuIndex] = min(atoi(&buffer[1]), 100);
        cpuTotal = cpuTotal + cpuReadings[cpuIndex];
        val = cpuTotal / NB_CPU_SMOOTH;

        //Set meter
        analogWrite(vu_cpu.pin_out, percent_to_PWM(val));

        //Advance index
        cpuIndex = cpuIndex + 1;
        if (cpuIndex >= NB_CPU_SMOOTH)
          cpuIndex = 0;
        break;
      case 'M':
        //Memory
        val = min(atoi(&buffer[1]), 100);

        //Set meter
        analogWrite(vu_ram.pin_out, percent_to_PWM(val));

        break;
    }

    memset(buffer, ' ', 5);
  }

//__________Pour vumeter "Water Heating", calcul de l'écart entre x lectures_______



  static unsigned long last_time = millis(); //use to be last_time
  static float last_vals[TIME_BTW_TWO_READ];
  static int curent = TIME_BTW_TWO_READ;

  Termal_sensor *sonde;

  if(screens == 0) {
    sonde = (&temp_wtr_out_pc);
  } else {
    sonde = sondes[screens - 1];
  }

  if (curent == TIME_BTW_TWO_READ && sonde->val > 1 ) {
    for (int i = 0; i < TIME_BTW_TWO_READ; ++i) {
      last_vals[i] = sonde->val;
    }
    curent = 0;
  }

  if(millis() >= last_time + (1000)) {
    
    float ecart = sonde->val - last_vals[curent];

    analogWrite( vu_heat.pin_out, mapfloat (ecart , -2.5, 2.5, 0, 249));
    //Serial.println(ecart);

    last_time = millis();
    last_vals[curent] = sonde->val;
    curent++;
    if(curent >= TIME_BTW_TWO_READ) {
      curent = 0;
    }
  }



//__________Pour vumeter "Water Heating", calcul de l'écart entre deux lectures_______



//if(is_TEC_ON == 0) {
//
//    analogWrite( vu_heat.pin_out, mapfloat (temp_wtr_out_pc.val - temp_wtr_out_pcrad.val , -2.5, 2.5, 0, 255));
//  
//  } else {
//
//    analogWrite( vu_heat.pin_out, mapfloat (temp_tec_cold.val - temp_wtr_out_pc.val, -2.5, 2.5, 0, 255));
//
//  }

  analogWrite(vu_eau.pin_out, mapfloat( temp_wtr_out_pc.val, 15, 40, 0, 255) );
  //analogWrite( vu_heat.pin_out, mapfloat (temp_wtr_out_pc.val - temp_wtr_out_pcrad.val , -2.5, 2.5, 0, 255));
  analogWrite(vu_flow.pin_out, mapfloat( mainloop_flow.val, 0, 100, 0, 255));
  //analogWrite(vu_heat.pin_out, mapfloat( (temp_wtr_out_pc.val - temp_wtr_in_pc.val), -2.5, 2.5, 0, 255));

}

void screenSaver() {
  static int n = 0;
  for (int i = 0; i < nb_vumetre; ++i) {
    analogWrite(vumetres[i]->pin_out, abs(255-n));
  }

  n++;
  if (n >= 512){
    n = 0;
  }
}
// Section pour affichage CPU et MEM Usage -------------------------
// ___________________________________________ VUMETRES __________________________________________________
//============================================================================================================//
