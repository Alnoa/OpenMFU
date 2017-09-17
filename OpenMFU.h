
#ifndef OpenMFU_h
#define OpenMFU_h

#include <inttypes.h>
#include <arduino.h>
#include <HardwareSerial.h>
typedef enum{	ModeJour,
							ModeVeilleuse,
							ModeFeuxCroisement,
							ModePleinPhare
						} ModePhare;

typedef enum{ Gauche,
							Droit,
							Warnings
						} ModeClignotants;

/**
 * Message
 */
typedef struct {
	//byte ENTETE = 0xFA;
	//1er groupe de bool dans un octet
	bool veilleuses:1;
	bool feux_de_croisements:1;
	bool phares:1;
	bool clignotant_droit:1;
	bool clignotant_gauche:1;
	bool longues_vues_hautes:1;
	bool antibrouillards:1;
	bool klaxon_sirene:1;
	//2eme groupe
	bool moteur:2;
	bool marche_avant_moteur:2;

	int8_t volant;
	int8_t traction;
	int8_t rapport_boite_de_vitesse;
	unsigned char CRC;
} Payload;

class OpenMFU{
public:
	//constructeur
	OpenMFU(bool mode);
	//modificateurs
	void init();
	void Dernier_contact_RF();
	void etat_FAILSAFE();
	void Envoie();
	void Recoie();
	void set_veil_crois_phares(ModePhare MP);
	void set_longues_vues(bool LV);
	void set_Klaxon(bool K);
	void set_Contact_moteur(bool CM);
	void set_Traction(int acc);
	void set_Direction(int vol);
	void set_Clignotants(ModeClignotants MC);

	//accesseurs
	//Péripheriques
	bool remorque(){return remorque_etat;}
	uint8_t batterie(){return niveau_batterie;}
	//eclairages
	bool get_Veilleuse(){return veilleuses;}
	bool get_Phares(){return phares;}
	bool get_Feux_de_croisements(){return feux_de_croisements;}
	bool get_Gyrophares(){return gyrophares;}
	bool get_Longues_vues_hautes(){return longues_vues_hautes;}
	bool get_Antibrouillards(){return antibrouillards;}
	bool get_Klaxon(){return klaxon_sirene;}
	bool get_Clignotant_droit(){return clignotant_droit;}
	bool get_Clignotant_gauche(){return clignotant_gauche;}
	//Motorisation
	int8_t get_Traction(){return traction;}
	int8_t get_Direction(){return volant;}
	bool get_Contact_moteur(){return moteur;}
	bool get_Marche_avant_moteur(){return marche_avant_moteur;}
	uint8_t get_Rapport_de_BV(){return rapport_boite_de_vitesse;}

	void printBits(byte myByte);


protected:
	unsigned long Serial2_speed_xbee;
	bool xbee_mode_tx;
	unsigned long Dernier_contact_radio=0;
	unsigned long FAILSAFE =1000;
	unsigned int compteur_failsafe=0;
private:
	byte memtab[6];
	/**
	* Périphériques
	*/
	bool remorque_etat;
	uint8_t type_remorque;
	uint8_t niveau_batterie;
	/**
	* Eclairage
	*/
	bool veilleuses;
	bool gyrophares;
	bool phares;
	bool feux_de_croisements;
	bool clignotant_droit;
	bool clignotant_gauche;
	bool longues_vues_hautes;
	bool antibrouillards;
	bool klaxon_sirene;
	//beep de marche arriere 2900hz tone(PIN, FREQUENCE hz, DURÉE ms (opt) );  arrêter fonction noTone(PIN)

	/**
	* Motorisation
	*/
	int8_t	traction = 0;
	int8_t volant = 0;
	bool moteur;
	bool marche_avant_moteur;
	uint8_t rapport_boite_de_vitesse;

	/**
	* variables de methodes de classes
	*/
	byte commandes[10];
	void HEX_to_BIN(byte monByte,uint8_t casier_RAM);

};
#endif
