#include "OpenMFU.h"
#include <arduino.h>

#ifdef BOARD_ARDUINO_MEGA_DUE
  Serial2.begin(57600);
#elif BOARD_ARDUINO_ESP32
  HardwareSerial Serial2(2);
  Serial2.begin(57600);
#endif

#define DEBUG
//creation de la trame de transport
Payload Trame;

/**
* mode = bool = true==mode emetteur || false==mode recepteur
*/
OpenMFU::OpenMFU(bool mode){
  xbee_mode_tx=mode;
  Serial2_speed_xbee=57900;// = int = vitesse du port serie xbee
  //Etablir_liason(xbee_mode_tx);
}

void OpenMFU::HEX_to_BIN(byte monByte,uint8_t casier_RAM){
  unsigned int compteurHEx=7;
  for(byte mask = 0x80; mask; mask >>= 1){
    if(mask  & monByte){
      //true
      switch (compteurHEx) {
        case 7:{gyrophares=true;}break;
        case 6:{antibrouillards=true;}break;
        case 5:{longues_vues_hautes=true;}break;
        case 4:{clignotant_gauche=true;}break;
        case 3:{clignotant_droit=true;}break;
        case 2:{ phares=true;}break;
        case 1:{ feux_de_croisements=true;}break;
        case 0:{ veilleuses=true;}break;
        default:
        break;
      }
    }else{
      //false
      switch (compteurHEx) {
        case 7:{gyrophares=false;}break;
        case 6:{antibrouillards=false;}break;
        case 5:{longues_vues_hautes=false;}break;
        case 4:{clignotant_gauche=false;}break;
        case 3:{clignotant_droit=false;}break;
        case 2:{ phares=false;}break;
        case 1:{ feux_de_croisements=false;}break;
        case 0:{ veilleuses=false;}break;
        default:
        break;
      }
      compteurHEx--;
    }
  }
}
void OpenMFU::Envoie(){
  Serial2.write(0x3F);
  //Serial2.write((uint8_t*)Trame,sizeof(Trame));
  #ifdef DEBUG
  Serial.write(0x01);
  Serial.print(memtab[0],HEX);
  Serial.print(memtab[1],HEX);
  Serial.print(memtab[2],HEX);
  Serial.print(memtab[3],HEX);
  Serial.print('\r');
  #endif //DEBUG

}
void OpenMFU::init(){
  #ifdef DEBUG
  Serial.begin(57600);

  Serial.print("lib OpenMFU ,etablissement de la liason ...");
  #endif // DEBUG
  Serial2.print("+++");
  Serial.print("+++");
  char Byte_de_test = 0;
  while (Byte_de_test != '\r') {
    if (Serial2.available() > 0) {
      Byte_de_test = Serial2.read();
    }
  }
  /*
    la suite contient le necessaire pour programer un xbee sur un reseau donné
    une seule fois suffit,

     creer une methode pour config cela ?
   */
  //Serial2.print("ATRE\r");//reset cfg
  //Serial2.print("ATPL4\r");//puissance TX max(0à4)
  //Serial.print("ATDL0\r"); // sent to Serial2 0
  //Serial.print("ATMY1\r"); // this xbee is 1
  //Serial2.print("ATID0B58\r");// reseau xbee adresse == 041986 en HEX
  //Serial2.print("ATWR\r");//enregistre la cfg sur le module
  Serial2.print("ATCN\r");//quitte le mode commande

  #ifdef DEBUG
  Serial.println("Xbee connecté !");
  #endif // DEBUG
}
void OpenMFU::Dernier_contact_RF(){
  // !serialavilable >10 fois ex ou verif etat de la broche Rssi ou led asso du xbee
  if(millis()-Dernier_contact_radio>FAILSAFE || compteur_failsafe>10){
    etat_FAILSAFE();
  }
}
void OpenMFU::etat_FAILSAFE(){}

void OpenMFU::Recoie(){
  if (Serial2.available() >= 8 ) { // wait for 6 characters
    for (int i=0; i < 8; i++){
      commandes[i] = Serial2.read();
      Serial.print(commandes[i],HEX);
      Serial.print(" ");
    }
    HEX_to_BIN(commandes[2],2);
    Serial.println(" ");
  }
}
void OpenMFU::set_veil_crois_phares(ModePhare MP){
  //enum !!!! http://www.locoduino.org/spip.php?article102
  /* typedef enum{ModeJour,ModeVeilleuse,ModeFeuxCroisement,ModePleinPhare}ModePhare;
   donc tu peux utiliser les masques, justement pour éviter le switch
   je te conseilles très fortement de faire un #define pour chaque masque*/
  switch(MP){
    case ModeVeilleuse:// mode veilleuses
    Trame.veilleuses=veilleuses=true;
    Trame.phares=Trame.feux_de_croisements=phares=feux_de_croisements=false;
    break;
    case ModeFeuxCroisement://mode feux de croisement
    Trame.veilleuses=Trame.feux_de_croisements=veilleuses=feux_de_croisements=true;
    Trame.phares=phares=false;
    break;
    case ModePleinPhare://mode pleins phares
    Trame.veilleuses=veilleuses=Trame.feux_de_croisements=feux_de_croisements=Trame.phares=phares=true;
    break;
    case ModeJour://mode jour (off)
    Trame.veilleuses=Trame.phares=Trame.feux_de_croisements=veilleuses=phares=feux_de_croisements=false;
    break;
    default:
    break;
  }
}
void OpenMFU::set_Clignotants(ModeClignotants MC){
  switch(MC){
    case Gauche:
    Trame.clignotant_droit=clignotant_droit=false;
    Trame.clignotant_gauche=clignotant_gauche=true;
    break;
    case Droit:
    Trame.clignotant_droit=clignotant_droit=true;
    Trame.clignotant_gauche=clignotant_gauche=false;
    break;
    case Warnings:
    Trame.clignotant_droit=clignotant_droit=true;
    Trame.clignotant_gauche=clignotant_gauche=true;
    break;
    default:
    break;
  }
}
void OpenMFU::set_longues_vues(bool LV){
  Trame.longues_vues_hautes=longues_vues_hautes=LV;
}
void OpenMFU::set_Klaxon(bool K){
  Trame.klaxon_sirene=klaxon_sirene=K;
}
void OpenMFU::set_Direction(int vol){
  Trame.volant=volant=constrain(vol,-100,100);
}
void OpenMFU::set_Contact_moteur(bool CM){
  Trame.moteur=moteur = CM;
}
void OpenMFU::set_Traction(int acc){
  Trame.traction=traction=constrain(acc,-100,100);
}
void OpenMFU::printBits(byte myByte){
  for(byte mask = 0x80; mask; mask >>= 1){
    if(mask  & myByte)
    Serial2.print('1');
    else
    Serial2.print('0');
  }
  Serial2.println();
}
