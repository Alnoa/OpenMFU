#include "OpenMFU.h"
#if (ARDUINO < 100)
#include "WProgram.h"
#else
#include "Arduino.h"
#endif
#include <arduino.h>
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
* speed_xbee = int = vitesse du port serie xbee
* mode = bool = true==mode emetteur || false==mode recepteur
*/
TruckBee::TruckBee(int speed_xbee,bool mode){
  memtab[0] = 0x00;
  memtab[1] = 0b00000000;
  memtab[2] = 0b00000000;
  memtab[3] = 0x64; // volant  HEX 64 = 0 centre de -100 à 100 (%)
  xbee_mode_tx=mode;
  Serial2_speed_xbee=speed_xbee;
  //Etablir_liason(xbee_mode_tx);
}

void TruckBee::HEX_to_BIN(byte monByte,uint8_t casier_RAM){
  unsigned int compteurHEx=7;
  for(byte mask = 0x80; mask; mask >>= 1){
    if(mask  & monByte){
      //true
      switch (compteurHEx) {
        case 7:{}break;
        case 6:{}break;
        case 5:{}break;
        case 4:{}break;
        case 3:{}break;
        case 2:{ phares=true;}break;
        case 1:{ feux_de_croisements=true;}break;
        case 0:{ veilleuses=true;}break;
        default:
        break;
      }
    }else{
      //false
      switch (compteurHEx) {
        case 7:{}break;
        case 6:{}break;
        case 5:{}break;
        case 4:{}break;
        case 3:{}break;
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

void TruckBee::Envoie(){
  Serial2.write(0x01);
  Serial2.write(memtab[0]);
  Serial2.write(memtab[1]);
  Serial2.write(memtab[2]);
  Serial2.write(memtab[3]);
  Serial2.write('\r');
  #ifdef DEBUG
  Serial.write(0x01);
  Serial.print(memtab[0],HEX);
  Serial.print(memtab[1],HEX);
  Serial.print(memtab[2],HEX);
  Serial.print(memtab[3],HEX);
  Serial.print('\r');
  #endif //DEBUG

}
void TruckBee::Etablir_liason(bool mode){
  #ifdef DEBUG
  Serial.begin(57600);

  Serial.println("lib truckbee , demarrage ...");
  #endif // DEBUG
  Serial2.begin(Serial2_speed_xbee);
  Serial2.print("+++");
  Serial.print("+++");
  char thisByte = 0;
  while (thisByte != '\r') {
    if (Serial2.available() > 0) {
      thisByte = Serial2.read();
    }
  }
  //Serial2.print("ATRE\r");//reset cfg
  //Serial2.print("ATPL4\r");//puissance TX max(0à4)
  //Serial.print("ATDL0\r"); // sent to Serial2 0
  //Serial.print("ATMY1\r"); // this xbee is 1
  //Serial2.print("ATID0B58\r");// reseau xbee adresse == 041986 en HEX
  //Serial2.print("ATWR\r");//enregistre la cfg sur le module
  Serial2.print("ATCN\r");//quitte le mode commande

  #ifdef DEBUG
  Serial.println("Xbee connecté et configuré");
  #endif // DEBUG
}
void TruckBee::Dernier_contact_RF(){// !serialavilable >10 fois ex ou verif etat de la broche Rssi ou led asso du xbee
  if(millis()-Dernier_contact_radio>FAILSAFE || compteur_failsafe>10){
    etat_FAILSAFE();
  }
}
void TruckBee::etat_FAILSAFE(){}
void TruckBee::Recoie(){
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
void TruckBee::setveil_crois_phares(ModePhare){//enum !!!! http://www.locoduino.org/spip.php?article102
  /*<jerome-> typedef enum { ModeJour = 000b, ModeVeilleuse= 001b, ModeFeuxCroisement = 011b, ModePleinPhare = 111b} ModePhare;
  <jerome-> donc tu peux utiliser les masques, justement pour éviter le switch
  <jerome-> je te conseilles très fortement de faire un #define pour chaque masque*/
  switch (ModePhare) {
    case ModeVeilleuse:// mode veilleuses
    memtab[1]= memtab[1] | ModeVeilleuse;
    /*
    bitWrite(memtab[1],adrmemtab_veilleuses,true);
    bitWrite(memtab[1],adrmemtab_phares,false);
    bitWrite(memtab[1],adrmemtab_feux_de_crois,false);
    */
    veilleuses=true;
    phares=feux_de_croisements=false;
    break;
    case ModeFeuxCroisement://mode feux de croisement
    memtab[1]= memtab[1] | ModeFeuxCroisement;
    /*
    bitWrite(memtab[1],adrmemtab_veilleuses,true);
    bitWrite(memtab[1],adrmemtab_feux_de_crois,true);
    bitWrite(memtab[1],adrmemtab_phares,false);
    */
    veilleuses=feux_de_croisements=true;
    phares=false;
    break;
    case ModePhare://mode pleins phares
    memtab[1]= memtab[1] | ModePhare;
    /*
    bitWrite(memtab[1],adrmemtab_veilleuses,true);
    bitWrite(memtab[1],adrmemtab_feux_de_crois,true);
    bitWrite(memtab[1],adrmemtab_phares,true);
    */
    veilleuses=feux_de_croisements=phares=true;
    break;
    case ModeJour://mode jour (off)
    memtab[1]= memtab[1] ^ ModeJour;
    /*
    bitWrite(memtab[1],adrmemtab_veilleuses,false);
    bitWrite(memtab[1],adrmemtab_phares,false);
    bitWrite(memtab[1],adrmemtab_feux_de_crois,false);
    */
    veilleuses=phares=feux_de_croisements=false;
    break;
    default:
    break;
  }
}
void TruckBee::setClignotants(char B){
  switch(B){
    case 'G'://gauche
    bitWrite(memtab[1],adrmemtab_clignotant_gauche,true);
    bitWrite(memtab[1],adrmemtab_clignotant_droit,false);
    clignotant_droit=false;
    clignotant_gauche=true;
    break;
    case 'D'://droit
    bitWrite(memtab[1],adrmemtab_clignotant_gauche,false);
    bitWrite(memtab[1],adrmemtab_clignotant_droit,true);
    clignotant_droit=true;
    clignotant_gauche=false;
    break;
    case 'W':// warnings
    bitWrite(memtab[1],adrmemtab_clignotant_gauche,true);
    bitWrite(memtab[1],adrmemtab_clignotant_droit,true);
    clignotant_droit=true;
    clignotant_gauche=true;
    break;
    default:
    break;
  }
}
void TruckBee::longues_vues(bool A){
  longues_vues_hautes=A;
  if(A){
    bitWrite(memtab[1],adrmemtab_longues_vues_hautes,true);
    longues_vues_hautes=true;
  }else{
    bitWrite(memtab[1],adrmemtab_longues_vues_hautes,false);
    longues_vues_hautes=false;
  }
}
void TruckBee::setKlaxon(bool B){
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
void TruckBee::setDirection(int vol){
  volant=constrain(vol,-100,100);
  memtab[3]=map(vol,-100,100,0,0xC8),0,0xC8;
}
void TruckBee::setContact_moteur(bool M){
  moteur = M;
  if(moteur){
    bitWrite(memtab[1],4,1);
  }else{
    bitWrite(memtab[1],4,0);
  }
}
void TruckBee::setTraction(int acc){
  traction=constrain(acc,-100,100);
  memtab[0]=map(traction,-100,100,0,0xC8);
}
void TruckBee::printBits(byte myByte){
  for(byte mask = 0x80; mask; mask >>= 1){
    if(mask  & myByte)
    Serial2.print('1');
    else
    Serial2.print('0');
  }
  Serial2.println();
}

/*
#define TAILLE_MAX 32

char texte[TAILLE_MAX / 2], unite;
int valeur, index, diviseur;

void setup() {
  Serial.begin(9600);
}

void loop() {
  if(recupInfo(texte, &valeur, &index, &diviseur, &unite)) {
    Serial.println("Erreur de trame !");
    return;
  }
  Serial.print("Texte: ");
  Serial.println(texte);
  Serial.print("Valeur: ");
  Serial.println(valeur);
  Serial.print("Index: ");
  Serial.println(index);
  Serial.print("Diviseur: ");
  Serial.println(diviseur);
  Serial.print("Unité: ");
  Serial.println(unite);
}

//
// Parse une chaine au format $ texte;valeur;index;diviseur;°c/f;
// Et retourne la valeur des differents champs.
//
int recupInfo(char *texte, int *valeur, int *index, int *diviseur, char *unite) {
  char c, buf[TAILLE_MAX + 1];
  unsigned char i = 0;

  // Attente du $ espace
  do {
    // Attente de 2 char sur le port série
    while(Serial.available() < 2);

    // Tant que chaine != $ espace -> boucle
  }
  while(Serial.read() != '$' && Serial.read() != ' ');

  // Remplissage du buffer
  do{
    // Si la chaine a dépassé la taille max du buffer
    if(i == (TAILLE_MAX + 1))
      // retourne 1 -> erreur
      return 1;

    // Attente d'un char sur le port série
    while(Serial.available() < 1);

    // Tant que char != 0x1A (fléche) -> boucle
  }
  while((buf[i++] = Serial.read()) != 0x1A);

  // Cloture la chaine de char
  buf[i] = '\0';

  // Copie le texte au début de buf[] dans texte[]
  i = 0;
  while((texte[i] = buf[i]) != ';') i++;
  texte[i] = '\0';

  // Parse la chaine de caractére et extrait les champs
  if(sscanf(buf + i, ";%d;%d;%d;%*c%c;", valeur, index, diviseur, unite) != 4)
    // Si sscanf n'as pas pu extraire les 4 champs -> erreur
    return 1;

  // retourne 0 -> pas d'erreur
  return 0;
}*/
