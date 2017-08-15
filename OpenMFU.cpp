#include "OpenMFU.h"
#if (ARDUINO < 100)
#include "WProgram.h"
#else
#include "Arduino.h"
#endif
#include <arduino.h>

#ifdef BOARD_ARDUINO_MEGA_DUE
  Serial2.begin(57600);
#elif BOARD_ARDUINO_ESP32
  HardwareSerial Serial2(2);
  Serial2.begin(57600);
#endif

#define DEBUG

/**
* adresse des casiers
*/
//casier 1
#define adrmemtab_veilleuses         1 //0b00000001
#define adrmemtab_feux_de_crois      2//0b00000010
#define adrmemtab_phares             3//0b00000100
#define adrmemtab_clignotant_droit   4//0b00001000
#define adrmemtab_clignotant_gauche  5//0b00010000
#define adrmemtab_longues_vues_hautes 6//0b00100000
#define adrmemtab_antibrouillards     7//0b01000000
#define adrmemtab_gyrophares          8//0b10000000
//casier 2
#define adrmemtab_klaxon              9//0b00000001



/**
* mode = bool = true==mode emetteur || false==mode recepteur
*/
OpenMFU::OpenMFU(bool mode){
  memtab[0] = 0x3F;
  memtab[1] = 0b00000000;
  memtab[2] = 0b00000000;
  memtab[3] = 0x64; // volant  HEX 64 = 0 centre de -100 à 100 (%)
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
  Serial2.write((uint8_t*)memtab,sizeof(memtab));
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

  Serial2.begin(Serial2_speed_xbee);
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
void OpenMFU::Dernier_contact_RF(){// !serialavilable >10 fois ex ou verif etat de la broche Rssi ou led asso du xbee
  if(millis()-Dernier_contact_radio>FAILSAFE || compteur_failsafe>10){
    etat_FAILSAFE();
  }
}
void OpenMFU::etat_FAILSAFE(){}
void OpenMFU::Recoie(){
  if (Serial2.available() >= 6 ) { // wait for 6 characters
    for (int i=0; i < 6; i++){
      commandes[i] = Serial2.read();
      Serial.print(commandes[i],HEX);
      Serial.print(" ");
    }
    HEX_to_BIN(commandes[2],2);
    Serial.println(" ");
  }
}
void OpenMFU::setveil_crois_phares(ModePhare){//enum !!!! http://www.locoduino.org/spip.php?article102
  /* typedef enum { ModeJour = 000b, ModeVeilleuse= 001b, ModeFeuxCroisement = 011b, ModePleinPhare = 111b} ModePhare;
   donc tu peux utiliser les masques, justement pour éviter le switch
   je te conseilles très fortement de faire un #define pour chaque masque*/
  switch(ModePhare){
    case ModeVeilleuse:// mode veilleuses
    memtab[1]= memtab[1] | ModeVeilleuse;
    veilleuses=true;
    phares=feux_de_croisements=false;
    break;
    case ModeFeuxCroisement://mode feux de croisement
    memtab[1]= memtab[1] | ModeFeuxCroisement;
    veilleuses=feux_de_croisements=true;
    phares=false;
    break;
    case ModePleinPhare://mode pleins phares
    memtab[1]= memtab[1] | ModePleinPhare;
    veilleuses=feux_de_croisements=phares=true;
    break;
    case ModeJour://mode jour (off)
    memtab[1]= memtab[1] ^ ModeJour;
    veilleuses=phares=feux_de_croisements=false;
    break;
    default:
    break;
  }
}
void OpenMFU::setClignotants(ModeClignotants){
  switch(ModeClignotants){
    case Gauche:
    memtab[1] = memtab[1] | Gauche;
    /*
    bitWrite(memtab[1],adrmemtab_clignotant_gauche,true);
    bitWrite(memtab[1],adrmemtab_clignotant_droit,false);
    */
    clignotant_droit=false;
    clignotant_gauche=true;
    break;
    case Droit:
    memtab[1] = memtab[1] | Droit;
    /*
    bitWrite(memtab[1],adrmemtab_clignotant_gauche,false);
    bitWrite(memtab[1],adrmemtab_clignotant_droit,true);
    */
    clignotant_droit=true;
    clignotant_gauche=false;
    break;
    case Warnings:
    memtab[1] = memtab[1] | Warnings;
    /*
    bitWrite(memtab[1],adrmemtab_clignotant_gauche,true);
    bitWrite(memtab[1],adrmemtab_clignotant_droit,true);
    */
    clignotant_droit=true;
    clignotant_gauche=true;
    break;
    default:
    break;
  }
}
void OpenMFU::longues_vues(bool A){
  longues_vues_hautes=A;
  if(A){
    bitWrite(memtab[1],adrmemtab_longues_vues_hautes,true);
    longues_vues_hautes=true;
  }else{
    bitWrite(memtab[1],adrmemtab_longues_vues_hautes,false);
    longues_vues_hautes=false;
  }
}
void OpenMFU::setKlaxon(bool B){
  klaxon_sirene=B;
  switch (B) {
    case 1:
    bitWrite(memtab[1],adrmemtab_klaxon,true);
    break;
    case 0:
    bitWrite(memtab[1],adrmemtab_klaxon,false);
    break;
    default:
    break;
  }
}
void OpenMFU::setDirection(int vol){
  volant=constrain(vol,-100,100);
  memtab[3]=map(vol,-100,100,0,0xC8);
}
void OpenMFU::setContact_moteur(bool M){
  moteur = M;
  if(moteur){
    bitWrite(memtab[2],4,1);
  }else{
    bitWrite(memtab[2],4,0);
  }
}
void OpenMFU::setTraction(int acc){
  traction=constrain(acc,-100,100);
  memtab[0]=map(traction,-100,100,0,0xC8);
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
