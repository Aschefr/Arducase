




// _______________________________________ Library __________________________________________________
#include <LiquidCrystal.h>
//#include <string.h>
// initialize the library with the numbers of the interface pins
LiquidCrystal lcd(14, 15, 16, 17, 18, 19);
// _______________________________________ Library __________________________________________________


typedef struct Termal_sensor {
  float offset; // offset pour convertion
  int raw; // valeur brute lue
  int pin; // on c'est branché
  float val; // valeur traduite en celcius
  int seuil_haut;
  int seuil_bas;
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


void init_thermals(){

  // default values
  Termal_sensor temp_default;
  temp_default.raw = -1;
  temp_default.val = -300.00;
  temp_default.seuil_haut = 35;
  temp_default.seuil_bas = 25;

  // DAMM C LANGUAGE FUCK YOU !!!
  temp_wtr_in_pc = temp_default;
  temp_wtr_out_pc = temp_default;
  temp_cpu = temp_default;
  temp_gpu = temp_default;
  temp_wtr_out_pcrad = temp_default;
  temp_pc_case = temp_default;
  temp_tec_hot = temp_default;
  temp_tec_cold = temp_default;
  temp_wtr_tec_hot = temp_default;
  temp_wc_case = temp_default;
  // END DAMM C LANGUAGE FUCK YOU !!

  // _________________________________ Gestion des Pins d'entrées de sondes thermiques _____________________________________________
  temp_wtr_in_pc.pin = 1;
  temp_wtr_out_pc.pin = 2;
  temp_cpu.pin = 3;
  temp_gpu.pin = 4;
  temp_wtr_out_pcrad.pin = 5;
  temp_pc_case.pin = 6;
  temp_tec_hot.pin = 7;
  temp_tec_cold.pin = 8;
  temp_wtr_tec_hot.pin = 9;
  temp_wc_case.pin = 10;

  // _________________________________ Gestion des Offset de sondes thermiques _____________________________________________
  temp_wtr_in_pc.offset = 0;
  temp_wtr_out_pc.offset = -5.00;
  temp_cpu.offset = 0;
  temp_gpu.offset = 0;
  temp_wtr_out_pcrad.offset = 0;
  temp_pc_case.offset = 0;
  temp_tec_hot.offset = 0;
  temp_tec_cold.offset = 0;
  temp_wtr_tec_hot.offset = 0;
  temp_wc_case.offset = 0;

  // _________________________________ Gestion des Seuils de sondes thermiques _____________________________________________

  // exemple d'overwrite des seuils par defaut (par defaut : haut = 35, bas = 25)
  //temp_wtr_tec_hot.seuil_haut = 45;
  //temp_wtr_tec_hot.seuil_bas = 20;

  thermals_save();
}

// _________________________________ Functions pour sondes thermiques _____________________________________________
void read_and_convert_termal_sensor(struct Termal_sensor sensor){

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

  if (sensor.val == -300.00)
    sensor.val = Temp; // on initialise sensor.val pour la première fois !
  else if (Temp + 0.25 > sensor.val || Temp - 0.25 < sensor.val) // si on s'éloigne de plus de 0.25 de la dernière valeur
    sensor.val = Temp; // alors on change la valeur
}








// _________________________________ Gestion des Seuils _____________________________________________
const int mode_silent = 20; //En mode Silent, pourcentage de marche ventilateur maximal
const int mode_normal = 50; //En mode Normal, pourcentage de marche ventilateur maximal
const int mode_heavy = 70; //En mode Heavy, pourcentage de marche ventilateur maximal
const int mode_extreme = 100; //En mode Extreme, pourcentage de marche ventilateur maximal



// ________________________________ Entrée/sortie Arduino Mega _______________________________________
// ----------------------------------------------------------------Gestion et mode de refroidissement :
const int btn_silent = 23; //Bouton mode "Silencieux"
const int btn_normal = 25; //Bouton mode "Normale"
const int btn_heavy = 27; //Bouton mode "Heavy"
const int btn_extreme = 29; //Bouton mode "Extreme"

const int led_silent = 22; //Led du bouton "Silencieu"
const int led_normal = 24; //Led du bouton "Normale"
const int led_heavy = 26; //Led du bouton "Heavy"
const int led_extreme = 28; //Led du bouton "Extreme"

const int vent_grp_pc = 2; //Groupe de ventilateur à l'intérieur du pc
const int vent_grp_rad = 3; //Groupe de ventilateur du radiateur PC
const int vent_grp_wc = 5; //Groupe de ventilateur de la watercase

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
  // Rendre les boutons, les led comme entrée ou sortie :
  pinMode(btn_silent, INPUT);
  pinMode(btn_normal, INPUT);
  pinMode(btn_heavy, INPUT);
  pinMode(btn_extreme, INPUT);
  pinMode(btn_next, INPUT);
  pinMode(btn_refrsh, INPUT);
  pinMode(led_silent, OUTPUT); 
  pinMode(led_normal, OUTPUT); 
  pinMode(led_heavy, OUTPUT); 
  pinMode(led_extreme, OUTPUT);
  
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
  
  
  init_thermals();

}
// _______________________________________ Setup _______________________________________














// __________ Gestion mode de fonctionnement, allumage LED et inscription LCD __________
//

int selected_mode = 0; //sauvegarde du mode de fonctionnement

void set_led (void) { 
  if (digitalRead(btn_silent) == HIGH &&
      digitalRead(btn_normal) == LOW &&
      digitalRead(btn_heavy) == LOW &&
      digitalRead(btn_extreme) == LOW ) {
    digitalWrite(led_silent, HIGH);
    digitalWrite(led_normal, LOW);
    digitalWrite(led_heavy, LOW);
    digitalWrite(led_extreme, LOW);
    selected_mode = mode_silent;
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Silent mode");
    lcd.setCursor(0, 1);
    lcd.print("Engage");
    //int c=0;
    //for (c=0 ; c<3;c++) {
      //lcd.setCursor(6+c,1);
      //lcd.print(".");
      //delay(500);
    //}
    delay(1000);
    lcd.clear();
  }
  
  if (digitalRead(btn_silent) == LOW &&
      digitalRead(btn_normal) == HIGH &&
      digitalRead(btn_heavy) == LOW &&
      digitalRead(btn_extreme) == LOW ) {
    digitalWrite(led_silent, LOW);
    digitalWrite(led_normal, HIGH);
    digitalWrite(led_heavy, LOW);
    digitalWrite(led_extreme, LOW);
    selected_mode = mode_normal;
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Normal mode");
    lcd.setCursor(0, 1);
    lcd.print("Engage");
    //int c=0;
    //for (c=0 ; c<3;c++) {
      //lcd.setCursor(6+c,1);
      //lcd.print(".");
      //delay(500);
    //}
    delay(1000);
    lcd.clear();
  }
  
  if (digitalRead(btn_silent) == LOW &&
      digitalRead(btn_normal) == LOW &&
      digitalRead(btn_heavy) == HIGH &&
      digitalRead(btn_extreme) == LOW ) {
    digitalWrite(led_silent, LOW);
    digitalWrite(led_normal, LOW);
    digitalWrite(led_heavy, HIGH);
    digitalWrite(led_extreme, LOW);
    selected_mode = mode_heavy;
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Heavy mode");
    lcd.setCursor(0, 1);
    lcd.print("Engage");
    //int c=0;
    //for (c=0 ; c<3;c++) {
      //lcd.setCursor(6+c,1);
      //lcd.print(".");
      //delay(500);
    //}
    delay(1000);
    lcd.clear();
  }
  
  if (digitalRead(btn_silent) == LOW &&
      digitalRead(btn_normal) == LOW &&
      digitalRead(btn_heavy) == LOW &&
      digitalRead(btn_extreme) == HIGH ) {
    digitalWrite(led_silent, LOW);
    digitalWrite(led_normal, LOW);
    digitalWrite(led_heavy, LOW);
    digitalWrite(led_extreme, HIGH);
    selected_mode = mode_extreme;
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Extreme mode");
    lcd.setCursor(0, 1);
    lcd.print("Engage");
    //int c=0;
    //for (c=0 ; c<3;c++) {
      //lcd.setCursor(6+c,1);
      //lcd.print(".");
      //delay(500);
    //}
    delay(1000);
    lcd.clear();
  }
  
}
// __________ Gestion mode de fonctionnement, allumage LED et inscription LCD __________









void thermals_save (void) { //Enregistrement température en Celsius dans chaque variables
  read_and_convert_termal_sensor( temp_wtr_in_pc );
  read_and_convert_termal_sensor( temp_wtr_out_pc );
  read_and_convert_termal_sensor( temp_cpu );
  read_and_convert_termal_sensor( temp_gpu );
  read_and_convert_termal_sensor( temp_wtr_out_pcrad );
  read_and_convert_termal_sensor( temp_pc_case );
  read_and_convert_termal_sensor( temp_tec_hot );
  read_and_convert_termal_sensor( temp_tec_cold );
  read_and_convert_termal_sensor( temp_wtr_tec_hot );
  read_and_convert_termal_sensor( temp_wc_case );
}







// ________________ Affichage des temps sur LCD avec bouton Refresh ______________________
int screens=0; //Compteur pour changer d'écran
int yazop = 0; //Pour fonctionnement du bouton "next" au relachement

void lcd_set (char *text, double variable){
   lcd.clear();
   lcd.setCursor(0, 0);
   lcd.print(text);
   lcd.setCursor(0, 1);
   lcd.print(variable);
   delay(200);
}

int amount_vent_rad; //affichage en % de la commande du ventilateur sortie 3
double outputValue = 0;

void lcd_temp_draw (void) {
 
  if (digitalRead(btn_next) == HIGH)
    yazop = 1;

  if ( digitalRead(btn_next)==LOW && yazop==1 ) {
    //Serial.println(screens);
    screens++;
    switch(screens) {
        case 0: lcd.clear(); 
                screens=0; break;
        case 1: lcd_set("wtr_in_pc :", temp_wtr_in_pc.val);
                thermals_save(); break; 
        case 2: lcd_set("wtr_out_pc :", temp_wtr_out_pc.val);
                thermals_save(); 
                lcd.setCursor(6, 1);
                lcd.print("vent=");
                //lcd.setCursor(6+length("vent="), 1);
                lcd.setCursor(6+5, 1);
                lcd.print(outputValue);
                break;
        case 3: lcd_set("cpu :", temp_cpu.val);
                thermals_save(); break; 
        case 4: lcd_set("gpup :", temp_gpu.val);
                thermals_save(); break;
        case 5: lcd_set("wtr_out_pcrad :", temp_wtr_out_pcrad.val);
                thermals_save(); break; 
        case 6: lcd_set("pc_case :", temp_pc_case.val);
                thermals_save(); break; 
        case 7: lcd_set("tec_hot :", temp_tec_hot.val);
                thermals_save(); break; 
        case 8: lcd_set("tec_cold :", temp_tec_cold.val);
                thermals_save(); break;
        case 9: lcd_set("wtr_tec_hot :", temp_wtr_tec_hot.val);
                thermals_save(); break; 
        case 10: lcd_set("wc_case :", temp_wc_case.val);
                 thermals_save(); break;
        default: screens=0; 
                 lcd.clear();
                 delay(200);
                 break;
    }
     yazop=0;
  }
  
  if (digitalRead(btn_refrsh) == HIGH) {
    thermals_save();
        switch(screens) {
        case 1: lcd_set("wtr_in_pc :", temp_wtr_in_pc.val);
                thermals_save(); break; 
        case 2: lcd_set("wtr_out_pc :", temp_wtr_out_pc.val);
                thermals_save();
                lcd.setCursor(6, 1);
                lcd.print("vent=");
                //lcd.setCursor(6+length("vent="), 1);
                lcd.setCursor(6+5, 1);
                lcd.print(outputValue);
                break;
        case 3: lcd_set("cpu :", temp_cpu.val);
                thermals_save(); break; 
        case 4: lcd_set("gpup :", temp_gpu.val);
                thermals_save(); break;
        case 5: lcd_set("wtr_out_pcrad :", temp_wtr_out_pcrad.val);
                thermals_save(); break; 
        case 6: lcd_set("pc_case :", temp_pc_case.val);
                thermals_save(); break; 
        case 7: lcd_set("tec_hot :", temp_tec_hot.val);
                thermals_save(); break; 
        case 8: lcd_set("tec_cold :", temp_tec_cold.val);
                thermals_save(); break;
        case 9: lcd_set("wtr_tec_hot :", temp_wtr_tec_hot.val);
                thermals_save(); break; 
        case 10: lcd_set("wc_case :", temp_wc_case.val);
                 thermals_save(); break;
        default: break;
    }
  }
  else {
     // ignore
  }    
}
// ________________ Affichage des temps sur LCD avec bouton Refresh ______________________




//régulation selon mode de refroidissement choisi.
/*
Pour la sonde wtr_out_pc
1) Lire la température en Celsius
2) Connaitre le mode de fonctionnement (Silent, normal, heavy, extreme)
3) Piloter entre 0 et x% (selon le monde de fonctionnement) la sortie PWM3
Penser à ajouter une boost start à 100% pendant un delay(200) pour être certain de démarrer le ventilateur.
4) Ecrire dans une variable nommé "amount_vent_rad" le % de commande pour affichage ultérieur sur LCD


1) wtr_out_pc contien la valeur en degres celsius
2) Il faut convertire cette valeur



*/






void regulation () {
//  int buf = value % 255; //sécurité pour ne pas dépasser une certaine valeur
  Serial.print("selected_mode=");Serial.println(selected_mode);
  int avg = (selected_mode*255)/100;
  Serial.print("avg=");Serial.println(avg);
  
  if (temp_wtr_out_pc.val > temp_wtr_out_pc.seuil_bas) {
    outputValue = map(temp_wtr_out_pc.val, temp_wtr_out_pc.seuil_bas, temp_wtr_out_pc.seuil_haut, 0, avg);
    analogWrite(vent_grp_rad, outputValue); 
  }
  else {
    analogWrite(vent_grp_rad, 0);
  }
}












 
// _______________________________________ Main Loop _______________________________________
void loop (void) {
  set_led();
  lcd_temp_draw();
  thermals_save();
  regulation();

}
// _______________________________________ Main Loop _______________________________________








