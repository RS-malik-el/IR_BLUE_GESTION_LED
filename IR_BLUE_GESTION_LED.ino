/**
 * e-mail : openprogramming23@gmail.com
 * gitHub : https://github.com/RS-malik-el
 * Donation : paypal.me/RachelSysteme
 *
 * @AUTEUR : Exaucé KIMBEMBE / @OpenProgramming
 * DATE : 11/06/2023
 * 
 * @Board : Arduino
 * Programme Arduino pour contrôler 4 lampes par Bluetooth ou télécommande infrarouge.
 * Ce code permet d'allumer ou d'éteindre les lampes individuellement en utilisant des signaux Bluetooth ou infrarouges.
 * 
 * @Outils
 * (1) HC-05
 * (1) Relais 4 canaux
 * (1) Recepteur infrarouge 32KHz
 * (1) Télécommande
 * (1) Carte Arduino
 * (4) Lampes
 * 
 * @Instructions d'utilisation :
 * 1. Assurez-vous d'avoir correctement connecté les modules Bluetooth et infrarouge à l'Arduino.
 * 2. Téléchargez et installez les bibliothèques nécessaires pour le module Bluetooth et infrarouge.
 * 3. Associez votre appareil Bluetooth avec l'Arduino, ou utilisez une télécommande infrarouge compatible.
 * 4. Envoyez les commandes appropriées (Bluetooth ou infrarouge) pour allumer ou éteindre les lampes
 * 
 * @Notes :
 * - Les signaux Bluetooth peuvent être envoyés à partir d'une application mobile ou d'un autre appareil compatible.
 * - Assurez-vous d'ajuster les broches utilisées pour les signaux infrarouges et les sorties des lampes en fonction 
 *   de votre configuration matérielle.
*/

#ifndef __AVR__
  #error "Selectionner une carte Arduino"
#endif

#define DECODE_NEC // Protocole utilisé
#include <IRremote.h>
#include <SoftwareSerial.h>

#define PIN_IR_REC 2 // Pin reception infrarouge
#define LED_1      3 // Pin led 1 
#define LED_2      4 // Pin led 2
#define LED_3      5 // Pin led 3
#define LED_4      6 // Pin led 4
// Pins HC-05
#define RX        10
#define TX        11
#define PIN_STATE 9

#define _REPEAT  0X1 // Valeur flag de répétition (appui maintenu du bouton)

// A remplacer
#define IR_CMD_1   0xF30CFF00 // Touche 1 de la télécommande
#define IR_CMD_2   0xE718FF00 // Touche 2 de la télécommande
#define IR_CMD_3   0xA15EFF00 // Touche 3 de la télécommande
#define IR_CMD_4   0xF708FF00 // Touche 4 de la télécommande

// Commandes bluetooth
#define BLUE_CMD_1  "L_1"
#define BLUE_CMD_2  "L_2"
#define BLUE_CMD_3  "L_3" 
#define BLUE_CMD_4  "L_4"

// Variables de contrôle de l'états des lampes
bool etat_1 = false; 
bool etat_2 = false;
bool etat_3 = false;
bool etat_4 = false;

uint8_t updateLed = 0; // Numéro de la led a mettre à jour l'état 

SoftwareSerial bluetooth(RX,TX);

// Prototype des fonctions
void updateLampe(void);

void setup(){
  Serial.begin(9600);
  bluetooth.begin(9600);

  IrReceiver.setReceivePin(PIN_IR_REC);
  IrReceiver.start();
  printActiveIRProtocols(&Serial);
  Serial.println(" ");

  pinMode(LED_1, OUTPUT);
  pinMode(LED_2, OUTPUT);
  pinMode(LED_3, OUTPUT);
  pinMode(LED_4, OUTPUT);

  pinMode(PIN_STATE, INPUT);
}


void loop(){
  // Si bluetooth connecté (Reception bluetooth)
  if(digitalRead(PIN_STATE)==true){
    if(bluetooth.available()){
      String reception = bluetooth.readStringUntil('\n');
      reception.trim();
      reception.toUpperCase();
        
      if(reception == BLUE_CMD_1){updateLed = 1;}
      else if(reception == BLUE_CMD_2){updateLed = 2;}
      else if(reception == BLUE_CMD_3){updateLed = 3;}
      else if(reception == BLUE_CMD_4){updateLed = 4;}

      updateLampe();
    }    
  }

  // Reception IR
  if(IrReceiver.decode() == true){
    // reception de potentiel bruit ou d'appuis maintenu du bouton
    if (IrReceiver.decodedIRData.protocol == UNKNOWN || IrReceiver.decodedIRData.flags == _REPEAT){
      IrReceiver.resume(); // Préparation pour une nouvelle réception
      return; // fin loop
    }

    uint32_t valCmd = 0 ; // Stocke la valeur du signal reçu
    valCmd = IrReceiver.decodedIRData.decodedRawData; // Stockage de la valeur du signal reçu

    // Affichage des résulats dans le moniteur série
    IrReceiver.printIRResultShort(&Serial);

    IrReceiver.resume(); 

    if (valCmd == IR_CMD_1){updateLed = 1;}
    else if (valCmd == IR_CMD_2){updateLed = 2;}
    else if (valCmd == IR_CMD_3){updateLed = 3;}
    else if (valCmd == IR_CMD_4){updateLed = 4;}

    updateLampe();
  }

}// Fin loop


void updateLampe(void){

  if(updateLed == 1){
    etat_1 = not etat_1; 
    digitalWrite(LED_1, etat_1);
  }

  else if(updateLed == 2) {
    etat_2 = not etat_2; 
    digitalWrite(LED_2, etat_2);       
  }

  else if(updateLed == 3) {
    etat_3 = not etat_3;
    digitalWrite(LED_3, etat_3);  
  }

  else if (updateLed == 4) {
    etat_4 = not etat_4;
    digitalWrite(LED_4, etat_4); 
  }

  updateLed = 0; // Re-initialisation de la variable
}