#ifndef ARDUCASE_H
#define ARDUCASE_H



//================================================================================================//
// _______________________________________ LCD __________________________________________________
#include <LiquidCrystal.h>
//#include <string.h>
// initialize the library with the numbers of the interface pins
extern LiquidCrystal lcd;
// _______________________________________ LCD __________________________________________________
//================================================================================================//




//====================================================================================================//
// _______________________________________ HELPERS __________________________________________________

#define DEBUG 0 //Debug mode = 1, sinon 0


#define percent_to_PWM(val) ((val * 255)/100) //Conversion d'un pourcentage en un signal de sortie Arduino 0-255pt, en l'occurence pour le contrôle d'une PWM
#define PWM_to_percent(val) ((val * 100)/255) //Exactement l'inverse


#define mapfloat(x, in_min, in_max, out_min, out_max) ((x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min)

// _______________________________________ HELPERS __________________________________________________
//====================================================================================================//


#endif