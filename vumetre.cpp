
// include core Wiring API
#include "Arduino.h"
#include "Arducase.h"

#include "vumetre.h"
#include "sondes.h"

//============================================================================================================//
// ___________________________________________ VUMETRES __________________________________________________

Vumetre vu_cpu;
Vumetre vu_eau;
Vumetre vu_ram;
Vumetre vu_flow;
Vumetre vu_heat;
Vumetre vu_tec;

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


  analogWrite(vu_eau.pin_out, mapfloat( temp_wtr_out_pc.val, 15, 40, 0, 255) );
  analogWrite(vu_heat.pin_out, mapfloat( temp_wtr_out_pcrad.val - temp_wtr_out_pc.val, -2.5, 2.5, 0, 255));

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

// ___________________________________________ VUMETRES __________________________________________________
//============================================================================================================//
