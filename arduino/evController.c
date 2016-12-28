#include "evController.h"

void setupEVController(){
  int i;
  for(i = 0; i < N_EV; i++){
  	pinMode(relayPins[i], OUTPUT);
  	digitalWrite(relayPins[i], LOW);
  	currentState[i] = false;
  }
}

bool openEVCS(int valveID){
	if(valveID <= N_EV){
		return false;
	}
	digitalWrite(relayPins[valveID], HIGH);
	currentState[valveID] = true;
	return true;
}

bool closeEVCS(int valveID){
	if(valveID <= N_EV){
		return false;
	}
 	digitalWrite(relayPins[valveID], LOW);
	currentState[valveID] = false;
	return true;
}

