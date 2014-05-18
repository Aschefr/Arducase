
// _______________________________________ Library __________________________________________________
#include <LiquidCrystal.h>
//#include <string.h>
// initialize the library with the numbers of the interface pins
LiquidCrystal lcd(14, 15, 16, 17, 18, 19);
// _______________________________________ Library __________________________________________________


//====================================================================================================//
// _______________________________________ HELPERS __________________________________________________

#define DEBUG 1


#define percent_to_PWM(val) ((val * 255)/100)
#define PWM_to_percent(val) ((val * 100)/255)

void lcd_set (char *text, double variable){
   lcd.clear();
   lcd.setCursor(0, 0);
   lcd.print(text);
   lcd.setCursor(0, 1);
   lcd.print(variable);
   delay(200);
}


// _______________________________________ HELPERS __________________________________________________
//====================================================================================================//



//============================================================================================================//
// _______________________________________ REGULATION MODE __________________________________________________
#define MAX_NB_MODES 5
typedef struct Mode {
  int pin_button;
  int pin_led;
  char *name;
  int pourcentage_min; // pourcentage min des ventilateurs
  int pourcentage_max; // pourcentage max des ventilateurs
} Mode;


Mode mode_silent; //En mode Silent, pourcentage de marche ventilateur maximal
Mode mode_normal; //En mode Normal, pourcentage de marche ventilateur maximal
Mode mode_heavy; //En mode Heavy, pourcentage de marche ventilateur maximal
Mode mode_extreme; //En mode Extreme, pourcentage de marche ventilateur maximal

Mode *selected_mode = &mode_silent;

Mode *modes[MAX_NB_MODES];
int nb_modes = 0;


void create_mode(struct Mode &mode, int pin_button, int pin_led, char *name, int pourcentage_min = 0, int pourcentage_max = 100){
  if (nb_modes + 1 > MAX_NB_MODES){
    return;
  }

  modes[nb_modes] = &mode;
  nb_modes++;

  mode.pin_button = pin_button;
  mode.pin_led = pin_led;
  mode.name = name;
  mode.pourcentage_min = pourcentage_min;
  mode.pourcentage_max = pourcentage_max;


  pinMode(mode.pin_button, INPUT);
  pinMode(mode.pin_led, OUTPUT);

}

void init_modes(){

  // _________________________________ Gestion des Modes _____________________________________________
  //          variable        btn led    nom        min  max
  create_mode(mode_silent   , 23, 22, "Silent"    , 30 , 40  ); //En mode Silent, pourcentage de marche ventilateur maximal
  create_mode(mode_normal   , 25, 24, "Normal"    , 30 , 60  ); //En mode Normal, pourcentage de marche ventilateur maximal
  create_mode(mode_heavy    , 27, 26, "Heavy"     , 50 , 80  ); //En mode Heavy, pourcentage de marche ventilateur maximal
  create_mode(mode_extreme  , 29, 28, "Extreme"   , 70 , 100 ); //En mode Extreme, pourcentage de marche ventilateur maximal

  // alume la led du mode selectionné dès le debut
  digitalWrite(selected_mode->pin_led, HIGH);
}
// _______________________________________ REGULATION MODE __________________________________________________
//============================================================================================================//




//=========================================================================================================//
// _______________________________________ VENTILATEURS __________________________________________________
#define MAX_NB_VENTILO 3
#define PWM_VENTILATEUR_MIN 85
typedef struct Ventilo {
  int pin;
  char *name;

  int curent_PWM;
  int last_PWM; // ancien curent_PWM (pour gestion du boost)
} Ventilo;


Ventilo vent_pc;
Ventilo vent_rad;
Ventilo vent_wc;

Ventilo *ventilos[MAX_NB_VENTILO];
int nb_ventilos = 0;


void create_ventilo(struct Ventilo &ventilo, int pin, char *name){
  if (nb_ventilos + 1 > MAX_NB_VENTILO){
    return;
  }

  ventilos[nb_ventilos] = &ventilo;
  nb_ventilos++;

  ventilo.pin = pin;
  ventilo.name = name;
  ventilo.curent_PWM = 0;
  ventilo.last_PWM = 0;
}

void init_ventilos(){

  // _________________________________ Gestion des Ventilos _____________________________________________
  //             variable   pin   nom
  create_ventilo(vent_pc  , 2  , "vent_pc"  ); //Groupe de ventilateur à l'intérieur du pc
  create_ventilo(vent_rad , 3  , "vent_rad" ); //Groupe de ventilateur du radiateur PC
  create_ventilo(vent_wc  , 5  , "vent_wc"  ); //Groupe de ventilateur de la watercase
}


void init_ventilo_regulation(){
  int min = percent_to_PWM(selected_mode->pourcentage_min);
  for (int i = 0; i < nb_ventilos; ++i) {
    ventilos[i]->last_PWM = ventilos[i]->curent_PWM;
    ventilos[i]->curent_PWM = min;
  }
}
void finish_ventilo_regulation(){

  int nb_boost = 0;
  for (int i = 0; i < nb_ventilos; ++i) {
    if (ventilos[i]->curent_PWM < PWM_VENTILATEUR_MIN){ // on est en dessous de la capacite du ventilateur, on l'arrete
      analogWrite(ventilos[i]->pin, 0);
      ventilos[i]->curent_PWM = 0;
    }
    else {
      if (ventilos[i]->last_PWM < PWM_VENTILATEUR_MIN ){
        analogWrite(ventilos[i]->pin, 255);
        nb_boost ++;
      }
      else {
        analogWrite(ventilos[i]->pin, ventilos[i]->curent_PWM);
      }  
    }
  }

  if (nb_boost > 0){
    delay(200);
  }
}

// _______________________________________ VENTILATEURS __________________________________________________
//=========================================================================================================//


//=============================================================================================================//
// _______________________________________ THERMALS SENSORS __________________________________________________
#define MAX_NB_THERMAL 10

typedef struct Termal_sensor {
  int pin; // on c'est branché
  char *name;
  float offset; // offset pour convertion
  int seuil_haut;
  int seuil_bas;
  int raw; // valeur brute lue
  float val; // valeur traduite en celcius

  // ventilos associé a cette sonde
  Ventilo* ventilos[MAX_NB_VENTILO];
  int nb_ventilos;
} Termal_sensor;



// ----------------------------------------------------------------------------- Sonde de températures :
Termal_sensor temp_wtr_in_pc; //température eau entrée pc
Termal_sensor temp_wtr_out_pc; //température eau sortie pc
Termal_sensor temp_cpu; //température au contact du CPU
Termal_sensor temp_gpu; //température au contact du GPU
Termal_sensor temp_wtr_out_pcrad; //température eau sortie du radiateur PC
Termal_sensor temp_pc_case; //température à l'intérieur du PC
Termal_sensor temp_tec_hot; //température du waterblock peltier face chaude
Termal_sensor temp_tec_cold; //température du waterblock peltier face froide
Termal_sensor temp_wtr_tec_hot; //température eau boucle peltier chaud
Termal_sensor temp_wc_case; //température dans la watercase


Termal_sensor *sondes[MAX_NB_THERMAL];
int nb_sondes = 0;

void create_thermal(struct Termal_sensor &sensor, int pin_number, char *name, int offset = 0, int seuil_bas = 25, int seuil_haut = 35){
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
  //              variable            pin  name              offset     seuil_haut       seuil_bas
  create_thermal(temp_wtr_in_pc      , 1 , "wtr_in_pc"      , 0);
  create_thermal(temp_wtr_out_pc     , 2 , "wtr_out_pc"     , -2.00);
  create_thermal(temp_cpu            , 3 , "cpu"            , 0);
  create_thermal(temp_gpu            , 4 , "gpu"            , 0);
  create_thermal(temp_wtr_out_pcrad  , 5 , "wtr_out_pcrad"  , 0);
  create_thermal(temp_pc_case        , 6 , "pc_case"        , 0);
  create_thermal(temp_tec_hot        , 7 , "tec_hot"        , 0);
  create_thermal(temp_tec_cold       , 8 , "tec_cold"       , 0);
  create_thermal(temp_wtr_tec_hot    , 9 , "wtr_tec_hot"    , 0);
  create_thermal(temp_wc_case        , 10, "wc_case"        , 0);

  // _________________________________ Association des sondes thermiques avec les ventilos _____________________________________________
  //                   sonde              ventilos
  add_ventilo_to_sonde(temp_wtr_out_pc  , vent_rad);
  add_ventilo_to_sonde(temp_cpu         , vent_rad);
  add_ventilo_to_sonde(temp_pc_case     , vent_pc );
  add_ventilo_to_sonde(temp_gpu         , vent_rad);
  add_ventilo_to_sonde(temp_tec_hot     , vent_wc );
  add_ventilo_to_sonde(temp_wtr_tec_hot , vent_wc );

  thermals_save();
}

// _________________________________ Functions pour sondes thermiques _____________________________________________
void read_and_convert_termal_sensor(struct Termal_sensor &sensor){

  sensor.raw = analogRead(sensor.pin);

  double Temp;
  // See http://en.wikipedia.org/wiki/Thermistor for explanation of formula
  Temp = log(((10240000/sensor.raw) - 10000));
  //Serial.println(Temp);
  Temp = 1 / (0.001129148 + (0.000234125 * Temp) + (0.0000000876741 * Temp * Temp * Temp));
  //Serial.println(Temp);
  Temp = Temp - 273.15;           // Convert Kelvin to Celcius
  //Serial.println(Temp);

  Temp = Temp + sensor.offset; // on applique l'offset

  if (sensor.val < -299.00)
    sensor.val = Temp; // on initialise sensor.val pour la première fois !
  else if (Temp + 0.25 > sensor.val || Temp - 0.25 < sensor.val) // si on s'éloigne de plus de 0.25 de la dernière valeur
    sensor.val = Temp; // alors on change la valeur

  if (sensor.val > sensor.seuil_haut){
    int max = percent_to_PWM(selected_mode->pourcentage_max);
    for (int i = 0; i < sensor.nb_ventilos; ++i) {
      sensor.ventilos[i]->curent_PWM = max;
    }
  }
  else if( sensor.val > sensor.seuil_bas ){

    int output_PWM = map(sensor.val, sensor.seuil_bas, sensor.seuil_haut, percent_to_PWM(selected_mode->pourcentage_min) , percent_to_PWM(selected_mode->pourcentage_max) );

    for (int i = 0; i < sensor.nb_ventilos; ++i) {
      if (output_PWM > sensor.ventilos[i]->curent_PWM){
        sensor.ventilos[i]->curent_PWM = output_PWM;
      }
    }
  }

}

// _______________________________________ THERMALS SENSORS __________________________________________________
//=============================================================================================================//


// ________________________________ Entrée/sortie Arduino Mega _______________________________________

// -------------------------------------------------------------------------------- Choix de la caméra :
const int btn_cam1 = 31; //Bouton pour choix caméra 1
const int btn_cam2 = 31; //Bouton pour choix caméra 2
const int btn_cam3 = 31; //Bouton pour choix caméra 3
const int btn_cam4 = 31; //Bouton pour choix caméra 4

const int led_cam1 = 30; //Led du bouton Caméra 1
const int led_cam2 = 32; //Led du bouton caméra 2
const int led_cam3 = 34; //Led du bouton caméra 3
const int led_cam4 = 36; //Led du bouton caméra 4

const int activ_cam1 = 44; //Sortie pour activation caméra 1
const int activ_cam2 = 46; //Sortie pour activation caméra 2
const int activ_cam3 = 48; //Sortie pour activation caméra 3
const int activ_cam4 = 50; //Sortie pour activation caméra 4


// ------------------------------------------------------------------------------------ Défilement LCD :
const int btn_next = 39; //Bouton Next LCD
const int btn_refrsh = 41; //Bouton Refresh LCD

const int led_next = 38; //Led du bouton Next
const int led_refrsh = 40; //Led du bouton Refresh

// --------------------------------------------------------------------------------- Mode Auto ou Manu :
const int sw_cooling_auto = 43; //Entrée switch cooling mode Auto ou Manu, Manu = 0, Auto = 1
const int sw_video_auto = 43; //Entrée switch Video mode Auto ou Manu, Manu = 0, Auto = 1
const int sw_tec = 43; //Entrée switch TEC mode On ou Off, Off = 0, On = 1

// ------------------------------------------------------------------------------------ Activation TEC :
const int tec_relay = 42; //Sortie pour activation relais TEC
const int tec_pump = 52; //Sortie pour pompe refroidissement TEC face chaude.

// ----------------------------------------------------------------------------- Gestion electrovannes :
const int servo_valve1 = 11; //Gestion du servo de vanne 1
const int servo_valve2 = 12; //Gestion du servo de vanne 2

const int valve1_close = 49; //Retour d'état vanne 1 fermé
const int valve2_close = 51; //Retour d'état vanne 2 fermé

// ---------------------------------------------------------------------------------------- Débitmetre :
const int flowmeter_read = 53; //Lecture débit

// ---------------------------------------------------------------------------------- Gestion vumetres :
const int vu_cpu = 4; //Vumetre % cpu utilisé
const int vu_ram = 6; //Vumetre % ram utilisé
const int vu_eau = 7; //Vumetre eau pc en °C
const int vu_flow = 8; //Vumetre débit eau
const int vu_eff = 9; //Vumetre efficacité refroidissement
const int vu_ech = 10; //Vumetre echauffement de l'eau après passage dans pc
// ________________________________ Entrée/sortie Arduino Mega _______________________________________




// _______________________________________ Setup _______________________________________
void setup (void)
{
  // set up the LCD's number of columns and rows: 
  lcd.begin(16, 2);
  
  // initialize serial communication at 9600 bits per second :
  Serial.begin(9600);
  
  /*
  Fréquence de la PWM :
  TCCR0B est pour le pin 4
  TCCR1B est pour le pin 11, 12, 13 (le pwm 13 est déconseillé, il fonctionne mal)
  TCCR2B est pour le pin 9, 10
  TCCR3B est pour le pin 2, 3, 5
  TCCR4B est pour le pin 6, 7, 8

  Inclure ce code dans le setup :
  int prescalerVal = 0x07;
  TCCRnB &= ~prescalerVal;
  
  prescalerVal = 1;
  TCCRnB |= prescalerVal;
  Remplacer le "n" de "TRCCRnB" par 0, 1, 2, 3 ou 4 selon les pin que vous souhaitez modifier.
  Ce code set la fréquence maximal de la PWM sur les pin concernés.
  */

  //First clear all three prescaler bits:
  int prescalerVal = 0x07; //create a variable called prescalerVal and set it equal to the binary number "00000111"
  TCCR3B &= ~prescalerVal; //AND the value in TCCR0B with binary number "11111000"

  //Now set the appropriate prescaler bits:
  prescalerVal = 1; //set prescalerVal equal to binary number "00000001"
  TCCR3B |= prescalerVal; //OR the value in TCCR0B with binary number "00000001"
  
  init_modes();
  init_ventilos();
  init_thermals();

}
// _______________________________________ Setup _______________________________________




// __________ Gestion mode de fonctionnement, allumage LED et inscription LCD __________
//


void set_mode (void) { 

  int nb = 0;
  Mode *temp_mode;
  for (int i = 0; i < nb_modes; ++i){
    if (digitalRead(modes[i]->pin_button) == HIGH){
      nb++;
      temp_mode = modes[i];
    }
  }

  // si on a bien un bouton appuyé
  if (nb == 1){
    // on set le mode actuel
    selected_mode = temp_mode;

    // on eteinds toutes les leds
    for (int i = 0; i < nb_modes; ++i){
      digitalWrite(modes[i]->pin_led, LOW);
    }
    // on ralume le mode selectionné
    digitalWrite(selected_mode->pin_led, HIGH);

    // on affiche engage
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print(selected_mode->name);
    lcd.setCursor(0, 1);
    lcd.print("Engage");

    delay(1000);
    lcd.clear();
  }
}
// __________ Gestion mode de fonctionnement, allumage LED et inscription LCD __________



void thermals_save (void) { //Enregistrement température en Celsius dans chaque variables

  init_ventilo_regulation();

  for (int i = 0; i < nb_sondes; ++i){
    read_and_convert_termal_sensor(*sondes[i]);
  }

  finish_ventilo_regulation();
}


//=========================================================================================//
// ________________ Affichage des temps sur LCD avec bouton Refresh ______________________

void lcd_temp_draw (void) {
 
  static int screens=0; //Compteur pour changer d'écran
  static int yazop = 0; //Pour fonctionnement du bouton "next" au relachement

  if (digitalRead(btn_next) == HIGH)
    yazop = 1;

  if ( digitalRead(btn_next)==LOW && yazop==1 ) {
    //Serial.println(screens);
    
    screens++;
    if (screens < 0 || screens > nb_sondes){
      screens = 0;
    }

    if (screens == 0){ // on eteind l'ecran
      lcd.clear();
    }
    else {
      lcd_set(sondes[screens - 1]->name, sondes[screens - 1]->val);
    }

    yazop=0;
  }
  
  if (digitalRead(btn_refrsh) == HIGH) {

    if (screens > 0){
      lcd_set(sondes[screens - 1]->name, sondes[screens - 1]->val);

      #if DEBUG 
        Serial.print(sondes[screens - 1]->name);
        Serial.print(' = ');
        Serial.print(sondes[screens - 1]->val);
        Serial.println('°C');

        for (int i = 0; i < sondes[screens - 1]->nb_ventilos; ++i){

          Serial.print('=> ');
          Serial.print(sondes[screens - 1]->ventilos[i]->name);
          Serial.print(' = ');
          Serial.print(PWM_to_percent( sondes[screens - 1]->ventilos[i]->curent_PWM ));
          Serial.println('%');
        }
      #endif
    }

  }

}
// ________________ Affichage des temps sur LCD avec bouton Refresh ______________________
//=========================================================================================//



// _______________________________________ Main Loop _______________________________________
void loop (void) {
  set_mode();
  lcd_temp_draw();
  thermals_save();
}
// _______________________________________ Main Loop _______________________________________

