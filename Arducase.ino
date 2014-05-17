// via gestion chaine de caractères
// "$btn_silent:$led_silent:0||1"
// "$btn_normal:$led_normal:0||1"
// "$btn_heavy:$led_heavy:0||1"
// "$btn_extreme:$led_extreme:0||1"
// 
// if 

// led_silent = 1 alors led_normal = led_heavy = led_extreme = 0
// led_normal = 1 alors led_silent = led_heavy = led_extreme = 0
// led_heavy = 1 alors led_silent = led_normal = led_extreme = 0
// led_extreme = 1 alors led_silent = led_normal = led_heavy = 0
// btn_silent => led_silent
// btn_normal => led_normal
// btn_heavy => led_heavy
// btn_extreme => led_extreme


// 0x3 ==> 0000 à 1111 => 0001 0010 0100 1000
// LSB => led_silent
// MSB => led_extreme

//int table_btn_silent[4] = {23, 25, 27, 29};
//int table_led[10]={22, 24, 26, 28, 30, 32, 34, 36, 38, 40};

// table[1] => 24
// table[2] => 26
// Serial.println(table[1]);

// int i = 0;
// for (i=0; i<10 ; i++)
//    Serial.println(table[i]);
//

//void 
//set_led (void) {
//    int i = 0;
//    for ( i=0 ; i<4 ; i++ ) {
//      digitalWrite(led_silent, i);
//      digitalWrite(led_normal, i);
//      digitalWrite(led_heavy, i);
//      digitalWrite(led_extreme, i);
//      sleep(1000);
//    }
//}






// _______________________________________ Library __________________________________________________
#include <LiquidCrystal.h>
// initialize the library with the numbers of the interface pins
LiquidCrystal lcd(14, 15, 16, 17, 18, 19);

// _______________________________________ Library __________________________________________________










// _________________________________ Gestion des Offset _____________________________________________
const int offset_wtr_in_pc = 0; //Offset sonde eau entrée pc
const int offset_wtr_out_pc = 0.00; //Offset sondee eau sortie pc
const int offset_cpu = 0.00; //Offset sonde au contact du CPU
const int offset_gpu = 0.00; //Offset sonde au contact du GPU
const int offset_wtr_out_pcrad = 0.00; //Offset sonde eau sortie du radiateur PC
const int offset_pc_case = 0.00; //Offset sonde à l'intérieur du PC
const int offset_tec_hot = 0.00; //Offset sonde du waterblock peltier face chaude
const int offset_tec_cold = 0.00; //Offset sonde du waterblock peltier face froide
const int offset_wtr_tec_hot = 0.00; //Offset sonde eau boucle peltier chaud
const int offset_wc_case = 0.00; //Offset sonde dans la watercase


// _________________________________ Gestion des Offset _____________________________________________











// _________________________________ Gestion des Seuils _____________________________________________
const int mode_silent = 20; //En mode Silent, pourcentage de marche ventilateur maximal
const int mode_normal = 50; //En mode Normal, pourcentage de marche ventilateur maximal
const int mode_heavy = 70; //En mode Heavy, pourcentage de marche ventilateur maximal
const int mode_extreme = 100; //En mode Extreme, pourcentage de marche ventilateur maximal

const int seuilh_wtr_in_pc = 35; //seuil haut température eau entrée pc
const int seuilb_wtr_in_pc = 25; //seuil bas température eau entrée pc

const int seuilh_wtr_out_pc = 35; //seuil haut température eau sortie pc
const int seuilb_wtr_out_pc = 25; //seuil bas température eau sortie pc

const int seuilh_cpu = 35; //seuil haut température au contact du CPU
const int seuilb_cpu = 25; //seuil bas température au contact du CPU

const int seuilh_gpu = 35; //seuil haut température au contact du GPU
const int seuilb_gpu = 25; //seuil bas température au contact du GPU

const int seuilh_wtr_out_pcrad = 35; //seuil haut température eau sortie du radiateur PC
const int seuilb_wtr_out_pcrad = 25; //seuil bas température eau sortie du radiateur PC

const int seuilh_pc_case = 35; //seuil haut température à l'intérieur du PC
const int seuilb_pc_case = 25; //seuil bas température à l'intérieur du PC

const int seuilh_tec_hot = 35; //seuil haut température du waterblock peltier face chaude
const int seuilb_tec_hot = 25; //seuil bas température du waterblock peltier face chaude

const int seuilh_tec_cold = 35; //seuil haut température du waterblock peltier face froide
const int seuilb_tec_cold = 25; //seuil bas température du waterblock peltier face froide

const int seuilh_wtr_tec_hot = 35; //seuil haut température eau boucle peltier chaud
const int seuilb_wtr_tec_hot = 25; //seuil bas température eau boucle peltier chaud

const int seuilh_wc_case = 35; //seuil haut température dans la watercase
const int seuilb_wc_case = 25; //seuil bas température dans la watercase



// _________________________________ Gestion des Seuils _____________________________________________












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
const int btn_prev = 41; //Bouton Prev LCD

const int led_next = 38; //Led du bouton Next
const int led_prev = 40; //Led du bouton prev

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

// ----------------------------------------------------------------------------- Sonde de températures :
const int temp_wtr_in_pc = 1; //température eau entrée pc
const int temp_wtr_out_pc = 2; //température eau sortie pc
const int temp_cpu = 3; //température au contact du CPU
const int temp_gpu = 4; //température au contact du GPU
const int temp_wtr_out_pcrad = 5; //température eau sortie du radiateur PC
const int temp_pc_case = 6; //température à l'intérieur du PC
const int temp_tec_hot = 7; //température du waterblock peltier face chaude
const int temp_tec_cold = 8; //température du waterblock peltier face froide
const int temp_wtr_tec_hot = 9; //température eau boucle peltier chaud
const int temp_wc_case = 10; //température dans la watercase

// ---------------------------------------------------------------------------------- Gestion vumetres :
const int vu_cpu = 4; //Vumetre % cpu utilisé
const int vu_ram = 6; //Vumetre % ram utilisé
const int vu_eau = 7; //Vumetre eau pc en °C
const int vu_flow = 8; //Vumetre débit eau
const int vu_eff = 9; //Vumetre efficacité refroidissement
const int vu_ech = 10; //Vumetre echauffement de l'eau après passage dans pc
// ________________________________ Entrée/sortie Arduino Mega _______________________________________
















// _______________________________________ Setup _______________________________________
void 
setup (void)
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
  pinMode(btn_prev, INPUT);
  pinMode(led_silent, OUTPUT); 
  pinMode(led_normal, OUTPUT); 
  pinMode(led_heavy, OUTPUT); 
  pinMode(led_extreme, OUTPUT);
}
// _______________________________________ Setup _______________________________________

















// __________ Gestion mode de fonctionnement, allumage LED et inscription LCD __________
//
void 
set_led (void) { 
  if (digitalRead(btn_silent) == HIGH &&
      digitalRead(btn_normal) == LOW &&
      digitalRead(btn_heavy) == LOW &&
      digitalRead(btn_extreme) == LOW ) {
    digitalWrite(led_silent, HIGH);
    digitalWrite(led_normal, LOW);
    digitalWrite(led_heavy, LOW);
    digitalWrite(led_extreme, LOW);
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








// _______________________ Lecture sonde et convertion en C° ____________________________
//Lecture sonde "Température eau entré pc
double Thermister_read(int RawADC) {
  double Temp;
  // See http://en.wikipedia.org/wiki/Thermistor for explanation of formula
  Temp = log(((10240000/RawADC) - 10000));
  Serial.println(Temp);
  Temp = 1 / (0.001129148 + (0.000234125 * Temp) + (0.0000000876741 * Temp * Temp * Temp));
  Serial.println(Temp);
  Temp = Temp - 273.15;           // Convert Kelvin to Celcius
  Serial.println(Temp);
  return Temp;
}

//Déclaration des variable pour lecture température
double wtr_in_pc;
double wtr_out_pc;
double cpu;
double gpu;
double wtr_out_pcrad;
double pc_case;
double tec_hot;
double tec_cold;
double wtr_tec_hot;
double wc_case;


void
temps_save (void) { //Enregistrement température en Celsius dans chaque variables

 wtr_in_pc = Thermister_read(analogRead(temp_wtr_in_pc)); //Enregistrement
 wtr_in_pc = arrondie(wtr_in_pc + offset_wtr_in_pc); //Application de l'offset
 
 wtr_out_pc = Thermister_read(analogRead(temp_wtr_out_pc));
 wtr_out_pc = wtr_out_pc + offset_wtr_out_pc;
 
 cpu = Thermister_read(analogRead(temp_cpu));
 cpu = cpu + offset_cpu;
 
 gpu = Thermister_read(analogRead(temp_gpu));
 gpu = gpu + offset_gpu;
 
 wtr_out_pcrad = Thermister_read(analogRead(temp_wtr_out_pcrad));
 wtr_out_pcrad = wtr_out_pcrad + offset_wtr_out_pcrad;
 
 pc_case = Thermister_read(analogRead(temp_pc_case));
 pc_case = pc_case + offset_pc_case;
 
 tec_hot = Thermister_read(analogRead(temp_tec_hot));
 tec_hot = tec_hot + offset_tec_hot;
 
 tec_cold = Thermister_read(analogRead(temp_tec_cold));
 tec_cold = tec_cold + offset_tec_cold;
 
 wtr_tec_hot = Thermister_read(analogRead(temp_wtr_tec_hot));
 wtr_tec_hot = wtr_tec_hot + offset_wtr_tec_hot;
 
 wc_case = Thermister_read(analogRead(temp_wc_case));
 wc_case = wc_case + offset_wc_case;

}
// _______________________ Lecture sonde et convertion en C° ____________________________


/* this piece of code is inspired by:
 * http://fr.openclassrooms.com/forum/sujet/arrondir-a-0-5-pres-96734#.U3cacSiGV8s
 */
double
arrondie ( double temp) 
{
  int a = (int) temp;
  double dif = temp - (double) a;
  
  if (dif<0.25)
    temp = (double) a;
    
  else if ((dif>0.25) && (dif<0.5))
    temp = (double) a + 0.25;
    
  else if ((dif>0.5) && (dif<0.75))
    temp = (double) a + 0.5;
    
  else if ((dif>0.75) && (dif<1))
    temp = (double) a + 0.75;
   
   else 
     temp = (double)a +1;
     
   return temp;
}









// _______________________________________ Main Loop _______________________________________
//  switch(variable) {
//     case value: instruction;
//     default: instruction;
//  }

/*long counter = 0;
long test = 0x7fff;

void
button_stat (byte button, long time) 
{
  while(digitalRead(btn_next)==LOW) {
    
  }
}
*/
/*
 *
 */
void
lcd_set (char *text, double variable)
{
   lcd.clear();
   lcd.setCursor(0, 0);
   lcd.print(text);
   lcd.setCursor(0, 1);
   //Serial.println(variable);
   lcd.print(variable);
   delay(200);
}

int screens=0;
int yazop = 0;

/*
 * 
 */
void 
loop (void)
{
  set_led();
  //temps_save();
  
  if (digitalRead(btn_next) == HIGH)
    yazop = 1;

  if ( digitalRead(btn_next)==LOW && yazop==1 ) {
    Serial.println(screens);
    screens++;
    switch(screens) {
        case 0: lcd.clear(); 
                screens=0; break;
        case 1: lcd_set("wtr_in_pc :", wtr_in_pc); 
                temps_save(); break; 
        case 2: lcd_set("wtr_out_pc :", wtr_out_pc); 
                temps_save();break;
        case 3: lcd_set("cpu :", cpu); 
                temps_save(); break; 
        case 4: lcd_set("gpup :", gpu); 
                temps_save(); break;
        case 5: lcd_set("wtr_out_pcrad :", wtr_out_pcrad); 
                temps_save(); break; 
        case 6: lcd_set("pc_case :", pc_case); 
                temps_save(); break; 
        case 7: lcd_set("tec_hot :", tec_hot); 
                temps_save(); break; 
        case 8: lcd_set("tec_cold :", tec_cold); 
                temps_save(); break;
        case 9: lcd_set("wtr_tec_hot :", wtr_tec_hot); 
                temps_save(); break; 
        case 10: lcd_set("wc_case :", wc_case); 
                 temps_save(); break;
        default: screens=0; 
                 lcd.clear();
                 delay(200);
                 break;
    }
     yazop=0;
  }
  
  if (digitalRead(btn_prev) == HIGH) {
    temps_save();
        switch(screens) {
        case 1: lcd_set("wtr_in_pc :", wtr_in_pc); 
                temps_save(); break; 
        case 2: lcd_set("wtr_out_pc :", wtr_out_pc); 
                temps_save();break;
        case 3: lcd_set("cpu :", cpu); 
                temps_save(); break; 
        case 4: lcd_set("gpup :", gpu); 
                temps_save(); break;
        case 5: lcd_set("wtr_out_pcrad :", wtr_out_pcrad); 
                temps_save(); break; 
        case 6: lcd_set("pc_case :", pc_case); 
                temps_save(); break; 
        case 7: lcd_set("tec_hot :", tec_hot); 
                temps_save(); break; 
        case 8: lcd_set("tec_cold :", tec_cold); 
                temps_save(); break;
        case 9: lcd_set("wtr_tec_hot :", wtr_tec_hot); 
                temps_save(); break; 
        case 10: lcd_set("wc_case :", wc_case); 
                 temps_save(); break;
        default: break;
    }
  }
  else {
     // code low
  }
}









