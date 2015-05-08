
// include core Wiring API
#include "Arduino.h"
#include "Arducase.h"

#include "modes.h"
#include "flowmeters.h"

//============================================================================================================//
// ___________________________________________ PELTIERS __________________________________________________

Flowmeter mainloop_flow;


Flowmeter *flowmeters[MAX_NB_FLOW];
int nb_flowmeter = 0;


void create_flowmeter(struct Flowmeter &flowmeter, int pin_in, char *name){
  if (nb_flowmeter + 1 > MAX_NB_FLOW){
    return;
  }

  flowmeters[nb_flowmeter] = &flowmeter;
  nb_flowmeter++;

  flowmeter.pin_in = pin_in;
  flowmeter.name = name;
  flowmeter.val = 0;

  flowmeter.time_start = millis();
  flowmeter.nb = 0;

  pinMode(flowmeter.pin_in, INPUT);

}

void init_flowmeters() {
//        variable           Pin   Nom
  create_flowmeter(mainloop_flow    , 20 , "Mainloop");

  attachInterrupt(3, interupt_mainloop, RISING);
  sei();
}




void interupt_mainloop() {
	mainloop_flow.nb ++;
	if(millis() - mainloop_flow.time_start > 1000) {
		mainloop_flow.val = ((mainloop_flow.nb * 60 / 7.5) * 1000 / (millis() - mainloop_flow.time_start) * 0.8);

		mainloop_flow.nb = 0;
		mainloop_flow.time_start = millis();
	}
}


//volatile int NbTopsFan; //measuring the rising edges of the signal
//int Calc;                               
//int hallsensor = 20;    //The pin location of the sensor
// 
//void rpm ()     //This is the function that the interupt calls 
//{ 
//  NbTopsFan++;  //This function measures the rising and falling edge of the 
//} 

//Flow
//  pinMode(hallsensor, INPUT); //initializes digital pin 2 as an input
//  Serial.begin(9600); //This is the setup function where the serial port is 
//  attachInterrupt(3, rpm, RISING); //and the interrupt is attached


//Flow
//  NbTopsFan = 0;	//Set NbTops to 0 ready for calculations
//  sei();		//Enables interrupts
//  delay (1000);	//Wait 1 second
//  cli();		//Disable interrupts
//  Calc = (NbTopsFan * 60 / 7.5); //(Pulse frequency x 60) / 7.5Q, = flow rate 
//  Serial.print (Calc, DEC); //Prints the number calculated above
//  Serial.print (" L/min\r\n"); //Prints "L/hour" and returns a  new line
  

//  static long unsigned last_refresh = 0;
//  if (screens > 0 && (millis()/250) - last_refresh > 1){
//    last_refresh = (millis()/250);

//    lcd_print_sonde(sondes[screens - 1]);





// ___________________________________________ PELTIERS __________________________________________________
//============================================================================================================//
