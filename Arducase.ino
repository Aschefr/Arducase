
// _______________________________________ Library __________________________________________________
#include <LiquidCrystal.h>
//#include <string.h>
// initialize the library with the numbers of the interface pins
LiquidCrystal lcd(14, 15, 16, 17, 18, 19);
// _______________________________________ Library __________________________________________________


//====================================================================================================//
// _______________________________________ HELPERS __________________________________________________

#define DEBUG 0 //Debug mode = 1, sinon 0


#define percent_to_PWM(val) ((val * 255)/100) //Conversion d'un pourcentage en un signal de sortie Arduino 0-255pt, en l'occurence pour le contrôle d'une PWM
#define PWM_to_percent(val) ((val * 100)/255) //Exactement l'inverse

#define MAX_NB_CAM 5 //Nombre de cameras de régulation maximum

// _______________________________________ HELPERS __________________________________________________
//====================================================================================================//



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
Mode mode_silent;
Mode mode_normal;
Mode mode_heavy;
Mode mode_extreme;

Mode *selected_mode = &mode_silent; // Variable de type "pointeur" vers le mode selectionné, ici, = &mode_silent pour démarré avec le mode "silent" au lancement de la carte.

Mode *modes[MAX_NB_MODES]; // Tableau de pointeur vers l'objet "Mode", ou plutôt un tableau de pointeur vers les objets "Mode", sert à lister tout les modes, en faire l'inventaire.
int nb_modes = 0; //Variable pour compter le nombre de modes, utilisé plus bas


/*
 * Fonction permetant de créer un "mode" et initialiser l'objet avec les propriétés définies plus bas dans "Gestion des Modes"
 */
void create_mode(struct Mode &mode, int pin_button, int pin_led, char *name, int pourcentage_min = 0, int pourcentage_max = 100){
  if (nb_modes + 1 > MAX_NB_MODES){ //Si nb_mode + 1 est supérieur au nombre de mode maxi,
    return; //Arrête la fonction, ne retourne rien.
  }

  //Sinon,
  modes[nb_modes] = &mode; // ajout du mode dans l'invertaire de tous les modes
  nb_modes++; //Incrémenter de 1 "nb_mode"

  mode.pin_button = pin_button; //Stocker dans "pin_button" qui se trouve dans "mode" la valeur de "pin_button" -> celle lu plus bas dans "Gestion des Modes"
  mode.pin_led = pin_led; //Pareil
  mode.name = name; //Idem
  mode.pourcentage_min = pourcentage_min; //Idem
  mode.pourcentage_max = pourcentage_max; //Idem


  pinMode(mode.pin_button, INPUT); //définir le pin du bouton comme une entrée
  pinMode(mode.pin_led, OUTPUT); //et le pin de la led comme une sortie, sinon ça marche pas.

}

void init_modes(){

  // _________________________________ Gestion des Modes _____________________________________________
  //          variable        btn led    nom        min  max
  create_mode(mode_silent   , 23, 22, "Silent"    , 20 , 100  ); //En mode Silent, pourcentage de marche ventilateur maximal
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
#define MAX_NB_VENTILO 4
#define PWM_VENTILATEUR_MIN 85
typedef struct Ventilo {
  int pin;
  char *name;

  int curent_PWM;
  int last_PWM; // ancien curent_PWM (pour gestion du boost)

  int nb_cam;
  Camera *cameras[MAX_NB_CAM];
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

  ventilo.nb_cam = 0;
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
      if (ventilos[i]->last_PWM < PWM_VENTILATEUR_MIN ){ //On est juste au dessus de la capacité du ventilateur, on le démarre avec un boost à fond pendant 200ms
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








//============================================================================================================//
// ___________________________________________ PELTIERS __________________________________________________
#define MAX_NB_TEC 6
typedef struct Peltier{
  int pin_out;
  char *name;
} Peltier;

Peltier tec_0_1;
Peltier tec_2;
Peltier tec_3;
Peltier tec_4;
Peltier tec_5;

Peltier *peltiers[MAX_NB_TEC];
int nb_peltier = 0;


void create_peltier(struct Peltier &peltier, int pin_out, char *name){
  if (nb_peltier + 1 > MAX_NB_TEC){
    return;
  }

peltiers[nb_peltier] = &peltier;
nb_peltier++;

peltier.pin_out = pin_out;
peltier.name = name;

pinMode(peltier.pin_out, OUTPUT);

}

void init_peltiers(){
//        variable           Pin   Nom
  create_peltier(tec_0_1    , 42 , "1, 2");
  create_peltier(tec_2      , 44 , "3"   );
  create_peltier(tec_3      , 46 , "4"   );
  create_peltier(tec_4      , 48 , "5"   );
  create_peltier(tec_5      , 50 , "6"   );

}
// ___________________________________________ PELTIERS __________________________________________________
//============================================================================================================//






//============================================================================================================//
// ___________________________________________ CAMERAS __________________________________________________

typedef struct Camera { //définition d'une structure qui comporte :
  int pin_button; //le pin du bouton
  int pin_led; //le pin de la led
  char *name; //le nom de la caméra
  int groupe;
  //int pin_camera; 
} Camera;

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

// __________ Gestion caméra avec affiche sur LCD __________
//



void set_camera(void) {

  int nb_appuye = 0;
  Camera *camera_appuye;
  for (int i = 0; i < nb_cam; ++i){
    if (digitalRead(cameras[i]->pin_button) == HIGH){
      nb_appuye++;
      camera_appuye = cameras[i];
    }
  }

  // si on a bien un boutons appuyé
  if (nb_appuye == 1){

    // on eteinds toutes les leds du même groupe
    for (int i = 0; i < nb_cam; ++i){
      if (cameras[i]->groupe == camera_appuye->groupe){
        digitalWrite(cameras[i]->pin_led, LOW);
      }
    }

    // on ralume la camera appuyé
    digitalWrite(camera_appuye->pin_led, HIGH);

    // on affiche le changement de cam
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print(camera_appuye->name);
    lcd.setCursor(0, 1);
    lcd.print("Load");

    delay(1000);
    lcd.clear();
  }
}


// __________ Gestion caméra avec affiche sur LCD __________















//=============================================================================================================//
// _______________________________________ THERMALS SENSORS __________________________________________________
#define MAX_NB_THERMAL 10
#define PRECISION_SONDE 0.5 // precision de la sonde en degree celsuis

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

void add_cam_to_ventilo(struct Camera &camera, struct Ventilo &ventilo){ //Ajout du ventilateur à la caméra correspondante à allumé lorsque celui-ci se mets en route.
  if( ventilo.nb_cam + 1 > MAX_NB_CAM ){
    return;
  }

  ventilo.cameras[ ventilo.nb_cam ] = &camera;
  ventilo.nb_cam ++;

}

void init_thermals(){

  // _________________________________ Gestion des sondes thermiques _____________________________________________
  //              variable            pin  name              offset     seuil_bas      seuil_haut
  create_thermal(temp_wtr_in_pc      , 1 , "wtr_in_pc"      , 0        , 25              , 30); //température eau entrée pc
  create_thermal(temp_wtr_out_pc     , 2 , "wtr_out_pc"     , -1.70    , 21              , 25); //température eau sortie pc
  create_thermal(temp_cpu            , 3 , "cpu"            , 0        , 30              , 40); //température au contact du CPU
  create_thermal(temp_gpu            , 4 , "gpu"            , 0        , 45              , 60); //température au contact du GPU
  create_thermal(temp_wtr_out_pcrad  , 5 , "wtr_out_pcrad"  , 0        , 25              , 30); //température eau sortie du radiateur PC
  create_thermal(temp_pc_case        , 6 , "pc_case"        , 0        , 28              , 35); //température à l'intérieur du PC
  create_thermal(temp_tec_hot        , 7 , "tec_hot"        , 0        , 30              , 35); //température du waterblock peltier face chaude
  create_thermal(temp_tec_cold       , 8 , "tec_cold"       , 0        , 20              , 30); //température du waterblock peltier face froide
  create_thermal(temp_wtr_tec_hot    , 9 , "wtr_tec_hot"    , 0        , 32              , 40); //température eau boucle peltier chaud
  create_thermal(temp_wc_case        , 10, "wc_case"        , 0        , 28              , 35); //température dans la watercase

  // _________________________________ Association des sondes thermiques avec les ventilos _____________________________________________
  //                   sonde              ventilos
  
  add_ventilo_to_sonde(temp_wtr_out_pc   , vent_rad);
  add_ventilo_to_sonde(temp_cpu          , vent_rad);
  add_ventilo_to_sonde(temp_gpu          , vent_rad);
  add_ventilo_to_sonde(temp_pc_case      , vent_pc );
  add_ventilo_to_sonde(temp_tec_hot      , vent_wc );
  add_ventilo_to_sonde(temp_wtr_tec_hot  , vent_wc );
  add_ventilo_to_sonde(temp_wc_case      , vent_wc );


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
  else if (Temp + PRECISION_SONDE > sensor.val || Temp - PRECISION_SONDE < sensor.val) // si on s'éloigne de plus de 0.50 de la dernière valeur
    sensor.val = Temp; // alors on change la valeur

  if (sensor.val > sensor.seuil_haut){
    int max = percent_to_PWM(selected_mode->pourcentage_max);
    for (int i = 0; i < sensor.nb_ventilos; ++i) {
      sensor.ventilos[i]->curent_PWM = max;
    }
  }

  else if( sensor.val > sensor.seuil_bas){

    int output_PWM = map(sensor.val, sensor.seuil_bas, sensor.seuil_haut, percent_to_PWM(selected_mode->pourcentage_min) , percent_to_PWM(selected_mode->pourcentage_max) );


    for (int i = 0; i < sensor.nb_ventilos; ++i) {
      if (output_PWM > sensor.ventilos[i]->curent_PWM){
        sensor.ventilos[i]->curent_PWM = output_PWM;
      }
    }
  }
}


void lcd_print_sonde (struct Termal_sensor* sensor){

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
    lcd.print(sensor->ventilos[0]->name); 
    lcd.setCursor(12, 1);
    lcd.print(PWM_to_percent(sensor->ventilos[0]->curent_PWM)); 
    lcd.setCursor(15, 1);
    lcd.print("%"); 
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








// ________________________________ Entrée/sortie Arduino Mega _______________________________________




// ------------------------------------------------------------------------------------ Défilement LCD :
const int btn_next = 39; //Bouton Next LCD
const int btn_prev = 41; //Bouton Prev LCD

const int led_next = 38; //Led du bouton Next
const int led_prev = 40; //Led du bouton Prev

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





// __________ Gestion mode de fonctionnement, allumage LED et inscription LCD __________
//


void set_mode (void) { 

  int nb_appuye = 0;
  Mode *mode_appuye;
  for (int i = 0; i < nb_modes; ++i){
    if (digitalRead(modes[i]->pin_button) == HIGH){
      nb_appuye++;
      mode_appuye = modes[i];
    }
  }

  // si on a bien un seul bouton appuyé
  if (nb_appuye == 1){
    // on set le mode actuel
    selected_mode = mode_appuye;

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
      lcd_print_sonde(sondes[screens - 1]); //Affichage de l'écran demandé
    }

    yazop=0;
  }
  
  static long unsigned last_refresh = 0;
  if (screens > 0 && (millis()/250) - last_refresh > 1){
    last_refresh = (millis()/250);

    lcd_print_sonde(sondes[screens - 1]);

  /*if (last_refresh > 30000)
    last_refresh = 0;
    */

    #if DEBUG 
      Serial.print(sondes[screens - 1]->name);
      Serial.print(" = ");
      Serial.print(sondes[screens - 1]->val);
      Serial.println(" °C");

      Serial.print("millis_func = ");
      Serial.println(millis());

      Serial.print("last_refresh = ");
      Serial.println(last_refresh);

      for (int i = 0; i < sondes[screens - 1]->nb_ventilos; ++i){

        Serial.print("=> ");
        Serial.print(sondes[screens - 1]->ventilos[i]->name);
        Serial.print(" = ");
        Serial.print(PWM_to_percent( sondes[screens - 1]->ventilos[i]->curent_PWM ));
        Serial.println("%");
      }
    #endif
  }

}
// ________________ Affichage des temps sur LCD avec bouton Refresh ______________________
//=========================================================================================//





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
  

  // on initialise les modes et tout
  // l'ordre est important !!!!!
  init_modes();
  init_ventilos();
  init_thermals();
  init_cameras();
  init_peltiers();


}
// _______________________________________ Setup _______________________________________


// _______________________________________ Main Loop _______________________________________
void loop (void) {
  set_mode();
  set_camera();
  lcd_temp_draw();
  thermals_save();
}
// _______________________________________ Main Loop _______________________________________

