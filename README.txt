Lib permettant le traitement des commandes RC via liaison sans fil Xbee

envoie type :
byte 0x3F 0x3F 0x00 0x01 0x02 0x03 0xXX
     | balise |      payload      | CRC

    payload :
      -0x00 >> 0b00000000
      -0x01 >> 0b00000000
      -0x02 >> int8_t
      -0x03 >> int8_t
[utilisation]

declaration:

#include <OpenMFU.h>
#define BOARD_ARDUINO_ESP32 (ou BOARD_ARDUINO_MEGA_DUE)

OpenMFU Nomdobjet(bool);
  -bool: true = éméteur, false = recepteur

initialisation:
void setup(){
Nomdojet.Etablir_liason();
}
