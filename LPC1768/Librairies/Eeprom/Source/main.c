//////////////////////////////////////////////////////////////////////////////////////////
//                             EXEMPLE D'UTILISATION                                    //
//////////////////////////////////////////////////////////////////////////////////////////
// Auteur               : Koala                                                         //
// URL                  : https://www.abcelectronique.com/bigonoff/forum/index.php      //
//////////////////////////////////////////////////////////////////////////////////////////
// Licence d'utilisation                                                                //
// ---------------------                                                                //
// Ce document est fourni tel quel, sans aucune garantie et sans recours d'aucune sorte //
// en cas de préjudice direct ou indirect lié ou non à son utilisation.                 //
// Vous avez le droit d'utiliser et/ou de modifier ce document dans le cadre d'une      //
// utilisation privée, publique, ou commerciale.                                        //
// Vous ne pouvez distribuer ce document qu'accompagné du présent commentaire original  //
// intégral incluant l'historique et les références de l'auteur.                        //
// En cas de distribution d'une version modifiée, vous devrez ajouter au présent        //
// commentaire la liste des points modifiés, la raison de la modification, sa date,     //
// ainsi que l'identification du modificateur.                                          //
// Il n'est pas autorisé de vendre ce document ou une version modifiée de celui-ci.     //
// Les codes exécutables produits à partir de ce document sont soumis à cette licence   //
// d'utilsation.                                                                        //
// Il n'est pas autorisé de distribuer les codes exécutables sans distribuer les        //
// fichiers sources ayant servi à les générer.                                          //
// En cas de traduction, seule la version originale française obtenue à partir du site  //
// de l'auteur fait référence. Il en va de même concernant les modifications.           //
// L'auteur s'est efforcé de réaliser cette librairie d'après les documents techniques  //
// officiels. Toute ressemblance avec un document existant ne serait que pure           //
// coïncidence.                                                                         //
//////////////////////////////////////////////////////////////////////////////////////////

#include "LPC17xx.h"                                                         // Toujours présent
#include "LPC17xx_Bits.h"                                                    // Définitions supplémentaires
#include "I2C.h"                                                             // Librairie I2C
#include "Timing.h"                                                          // Librairie pour les timings
#include "Eeprom.h"                                                          // Librairie Eeprom

//////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////
//                                 DEFINES                                              //
//////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////

// Mémoire Eeprom 24LC512 avec A2=1, A1=0 et A0=0
// ----------------------------------------------
#define MEMORY_1            0xA8, EEP_512                                    // Adresse et type de mémoire

//////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////
//                                      MAIN                                            //
//////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////

int main(void)
{   
    uint8_t buffer[128] = {0};                                               // Tableau pour lire et écrire les données dans la mémoire
    Boolean result = false;                                                  // Résultat des opérations
    
    SystemInit();                                                            // Initialise horloge : INDISPENSABLE
    I2C_Initialize(I2C0);                                                    // Démarre le module I2C0
    Timing_InitializeTimer(TIMER0);                                          // Utilisation du timer0 pour les tempos
    
    Eeprom_SelectChip(MEMORY_1);                                             // Sélectionne la mémoire Eeprom à utiliser
    result = Eeprom_WriteBytesArray(0x00, buffer, sizeof(buffer));           // Place les 128 premiers octets à 0
    if(result){                                                              // Si OK
        result = Eeprom_WriteByte(0x05, 0xFF);                               // Place la valeur 255 à l'adresse 0x05. 
        if(result){                                                          // Si OK
            result = Eeprom_ReadBytesArray(0x00, buffer, sizeof(buffer));    // Lecture des 128 premiers octets
            uint8_t value = Eeprom_ReadByte(0x05);                           // On peut aussi lire un octet précis
        }
    }
    
    while(1)
    {
        
    }
}
