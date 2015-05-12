
// include core Wiring API
#include "Arduino.h"
#include "Arducase.h"
#include "sondes.h"

//=============================================================================================================//
// _______________________________________ THERMALS SENSORS __________________________________________________


extern Ventilo vent_pc;
extern Ventilo vent_rad;
extern Ventilo vent_tec;
extern int nb_TEC_ON;

// ----------------------------------------------------------------------------- Sonde de températures :
// Définition des sondes de température
Termal_sensor temp_wtr_in_pc; 
Termal_sensor temp_wtr_out_pc; 
Termal_sensor temp_cpu; 
Termal_sensor temp_gpu; 
Termal_sensor temp_wtr_out_pcrad; 
Termal_sensor temp_pc_case; 
Termal_sensor temp_tec_hot; 
Termal_sensor temp_tec_cold;
Termal_sensor temp_wtr_tec_hot; 
Termal_sensor temp_wc_case; 


Termal_sensor *sondes[MAX_NB_THERMAL];
int nb_sondes = 0;

// Definition d'une structure contenant pour chaque sonde : le pin d'entrée, le nom, l'offset, le seuil bas et haut pour la régulation
void create_thermal(struct Termal_sensor &sensor, int pin_number, char *name, int offset, int seuil_bas, int seuil_haut){
  if (nb_sondes + 1 > MAX_NB_THERMAL){
    return;
  }

  sondes[nb_sondes] = &sensor;
  nb_sondes++;

  sensor.raw = -1;
  sensor.val = -300.00;
  
  sensor.pin = pin_number;
  sensor.name = name;
  sensor.offset = offset;
  sensor.seuil_haut = seuil_haut;
  sensor.seuil_bas = seuil_bas;

  sensor.nb_ventilos = 0;

  for (int i = 0; i < NB_HISTO_SONDE; ++i) {
    sensor.histo_val[i] = 0;
  }
  sensor.last_histo = 0;

}

void add_ventilo_to_sonde(struct Termal_sensor &sensor, struct Ventilo &ventilo){
  if( sensor.nb_ventilos + 1 > MAX_NB_VENTILO ){
    return;
  }

  sensor.ventilos[ sensor.nb_ventilos ] = &ventilo;
  sensor.nb_ventilos ++;

}

void init_thermals(){

  // _________________________________ Gestion des sondes thermiques _____________________________________________
  //              variable            pin  name              offset     seuil_bas     seuil_haut
  create_thermal(temp_wtr_in_pc      , 1 , "wtr_in_pc"      , 0         , 28          , 33); //température eau entrée pc
  create_thermal(temp_wtr_out_pc     , 2 , "wtr_out_pc"     , 0         , 27          , 32); //température eau sortie pc
  //create_thermal(temp_cpu          , 3 , "cpu"            , 0         , 30          , 40); //température au contact du CPU
  create_thermal(temp_gpu            , 4 , "gpu"            , 4         , 40          , 50); //température au contact des ram GPU
  create_thermal(temp_wtr_out_pcrad  , 5 , "wtr_out_pcrad"  , 0         , 29          , 35); //température eau sortie du radiateur PC
  create_thermal(temp_pc_case        , 6 , "pc_case"        , 0         , 26          , 30); //température à l'intérieur du PC
  create_thermal(temp_tec_hot        , 7 , "tec_hot"        , 0         , 25          , 35); //température du waterblock peltier face chaude
  create_thermal(temp_tec_cold       , 8 , "tec_cold"       , 0         , 27          , 30); //température du waterblock peltier face froide
  create_thermal(temp_wtr_tec_hot    , 9 , "wtr_tec_hot"    , 0         , 28          , 40); //température eau boucle peltier chaud
  create_thermal(temp_wc_case        , 10, "wc_case"        , -2        , 24          , 30); //température dans la watercase

  // _________________________________ Association des sondes thermiques avec les ventilos _____________________________________________
  //                   sonde              ventilos
  
  add_ventilo_to_sonde(temp_wtr_out_pc   , vent_rad);
  //add_ventilo_to_sonde(temp_cpu          , vent_rad);
  
  add_ventilo_to_sonde(temp_gpu          , vent_pc);
  add_ventilo_to_sonde(temp_pc_case      , vent_pc );

  add_ventilo_to_sonde(temp_tec_hot      , vent_tec );
  add_ventilo_to_sonde(temp_wtr_tec_hot  , vent_tec );
  add_ventilo_to_sonde(temp_wc_case      , vent_tec );

  thermals_save();
}



// _________________________________ Functions pour sondes thermiques _____________________________________________
void read_and_convert_termal_sensor(struct Termal_sensor &sensor){

  sensor.raw = analogRead(sensor.pin);

  double Temp;
  // See http://en.wikipedia.org/wiki/Thermistor for explanation of formula
  Temp = log(((10240000/(double)sensor.raw) - 10000));
  //Serial.println(Temp);
  Temp = 1 / (0.001129148 + (0.000234125 * Temp) + (0.0000000876741 * Temp * Temp * Temp));
  //Serial.println(Temp);
  Temp = Temp - 273.15;           // Convert Kelvin to Celcius
  //Serial.println(Temp);

  Temp = Temp + sensor.offset; // on applique l'offset

  // sauvegarde de l'historique
  sensor.last_histo++;
  if(sensor.last_histo == NB_HISTO_SONDE) {
    sensor.last_histo = 0;
  }
  sensor.histo_val[sensor.last_histo] = Temp;
  
  // reprise de la valeur moyenne pour la suite :-)
  Temp = 0;
  for (int i = 0; i < NB_HISTO_SONDE; ++i) {
    Temp += sensor.histo_val[i];
  }
  Temp = Temp / NB_HISTO_SONDE;

  // apprication du filtrage de precision de sonde (petites fluctuations)
  if (sensor.val < -299.00) {
    sensor.val = Temp; // on initialise sensor.val pour la première fois !
  } else if (Temp > sensor.val + PRECISION_SONDE || Temp < sensor.val - PRECISION_SONDE) {// si on s'éloigne de plus de X de la dernière valeur
    sensor.val = Temp; // alors on change la valeur
  }

  if (sensor.val > sensor.seuil_haut) {
    int max = percent_to_PWM(selected_mode->pourcentage_max);
    for (int i = 0; i < sensor.nb_ventilos; ++i) {
      sensor.ventilos[i]->curent_PWM = max;
    }
  }
  else if( sensor.val > sensor.seuil_bas) {

    for (int i = 0; i < sensor.nb_ventilos; ++i) {
      
      int output_PWM = mapfloat(sensor.val, sensor.seuil_bas, sensor.seuil_haut, 0 , 255 );
      if(output_PWM < percent_to_PWM(selected_mode->pourcentage_min) && (!sensor.ventilos[i]->is_tec || nb_TEC_ON > 0)) {
        output_PWM = percent_to_PWM(selected_mode->pourcentage_min);
      } else if(output_PWM > percent_to_PWM(selected_mode->pourcentage_max)) {
        output_PWM = percent_to_PWM(selected_mode->pourcentage_max);
      }

      if (output_PWM > sensor.ventilos[i]->curent_PWM){
        sensor.ventilos[i]->curent_PWM = output_PWM;
      }
    }
  }
}


void lcd_print_sonde(struct Termal_sensor* sensor){

  //Affichage nom de la sonde, température en face

  lcd.clear();
  lcd.setCursor(0, 0); //selection première ligne
  lcd.print(sensor->name); //afficher le contenu de "nom_sonde"
  lcd.setCursor(11, 0); //selection deuxième ligne
  lcd.print(sensor->val); //afficher le contenu de "temperature"
  //Affichage ventilateur commandé, pourcentage de commande en face
  if (sensor->nb_ventilos == 0){
    lcd.setCursor(0, 1);
    lcd.print("Aucun ventilos");
  }
  else if (sensor->nb_ventilos == 1){
    lcd.setCursor(0, 1); 
    lcd.print(sensor->ventilos[0]->name); //Vent_XXX________

    lcd.setCursor(0, 1); 
    lcd.print(sensor->seuil_bas); //CCC__
    lcd.setCursor(2, 1);
    lcd.print("C"); 
    lcd.setCursor(3, 1); //_______________
    lcd.print("  "); 

    lcd.setCursor(10, 1);
    lcd.print("%"); 

    lcd.setCursor(8, 1);
    lcd.print(PWM_to_percent(sensor->ventilos[0]->curent_PWM)); 

    lcd.setCursor(13, 1); 
    lcd.print(sensor->seuil_haut); //CC___
    lcd.setCursor(15, 1);
    lcd.print("C"); 

/*
    lcd.setCursor(0, 1); 
    lcd.print(sensor->ventilos[0]->name); 
    lcd.setCursor(12, 1);
    lcd.print(PWM_to_percent(sensor->ventilos[0]->curent_PWM)); 
    lcd.setCursor(15, 1);
    lcd.print("%"); 
    */
  }
  else if (sensor->nb_ventilos == 2){
    lcd.setCursor(0, 1);
    lcd.print("Ventilo: "); 

    lcd.setCursor(9, 1);
    lcd.print(PWM_to_percent(sensor->ventilos[0]->curent_PWM)); 
    lcd.setCursor(11, 1);
    lcd.print("%");

    lcd.setCursor(12, 1);
    lcd.print(PWM_to_percent(sensor->ventilos[1]->curent_PWM)); 
    lcd.setCursor(15, 1);
    lcd.print("%");
  }
  else {
    lcd.setCursor(0, 1);
    lcd.print("Trop de ventilos");
  }

}


// _______________________________________ THERMALS SENSORS __________________________________________________
//=============================================================================================================//
