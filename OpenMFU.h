
#ifndef OpenMFU_h
#define OpenMFU_h

#include <inttypes.h>
#include <arduino.h>

typedef enum{
	ModeJour = 0b00000111,
	ModeVeilleuse= 0b00000001,
	ModeFeuxCroisement = 0b00000011,
	ModePleinPhare = 0b00000111
} ModePhare;

typedef enum{
	Gauche =   0b00010000,
	Droit =  	 0b00001000,
	Warnings = 0b00011000
} ModeClignotants;

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
	void setveil_crois_phares(char MP);
	void longues_vues(bool A);
	void setKlaxon(bool B);
	void setContact_moteur(bool M);
	void setTraction(int acc);
	void setDirection(int vol);
	void setClignotants(char B);

	//accesseurs
	//Péripheriques
	bool remorque(){return remorque_etat;}
	uint8_t batterie(){return niveau_batterie;}
	//eclairages
	bool getVeilleuse(){return veilleuses;}
	bool getPhares(){return phares;}
	bool getFeux_de_croisements(){return feux_de_croisements;}
	bool getGyrophares(){return gyrophares;}
	bool getLongues_vues_hautes(){return longues_vues_hautes;}
	bool getAntibrouillards(){return antibrouillards;}
	bool getKlaxon(){return klaxon_sirene;}
	bool getClignotant_droit(){return clignotant_droit;}
	bool getClignotant_gauche(){return clignotant_gauche;}
	//Motorisation
	int8_t getTraction(){return traction;}
	int8_t getDirection(){return volant;}
	bool getContact_moteur(){return moteur;}
	bool getMarche_avant_moteur(){return marche_avant_moteur;}
	uint8_t getRapport_de_BV(){return rapport_boite_de_vitesse;}

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
