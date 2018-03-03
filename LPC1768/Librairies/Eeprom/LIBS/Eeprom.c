//////////////////////////////////////////////////////////////////////////////////////////
//                     DÉCLARATIONS POUR LA LIBRAIRIE EEPROM.C                          //
//////////////////////////////////////////////////////////////////////////////////////////
// Auteur               : Bigonoff                                                      //
// URL                  : www.bigonoff.org                                              //
//////////////////////////////////////////////////////////////////////////////////////////
// Historique                                                                           //
// ----------                                                                           //
// V1.00    18/05/2015  : Première version en ligne                                     //
// V1.01    04/01/2018  : Prise en charge des 24LC256, 512 et 1025 (Koala)              //
//////////////////////////////////////////////////////////////////////////////////////////
// Objet : Déclarations pour la librairie Eeprom.c                                      //
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

#include <lpc17xx.h>
#include "LPC17xx_Bits.h"
#include "Eeprom.h"
#include "I2C.h"
#include "Timing.h"

//////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////
//                               VARIABLES GLOBALES                                     //
//////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////

uint8_t _selectedEepromAddress = 0;                                     // Adresse de la mémoire EEPROM actuellement utilisée
uint8_t _selectedEepromCapability = EEP_02;                             // Capacité de la mémoire EEPROM actuellement utilisée

//////////////////////////////////////////////////////////////////////////////////////////
//                                  PROTOTYPES                                          //
//////////////////////////////////////////////////////////////////////////////////////////

static __inline Boolean BeginTransmission(uint16_t MemoryAddress, Boolean Mode);
    
//////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////
//                               FONCTIONS PUBLIQUES                                    //
//////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////////////
//                                  SELECTION DE L'EEPROM                               //
//////////////////////////////////////////////////////////////////////////////////////////
//----------------------------------------------------------------------------------------
// EepromAddress    : Adresse de la mémoire EEPROM sur 7bits décalé d'un rang sur la gauche
// EepromCapability : Capacité de la mémoire, la valeur est en fait le PageWriteLength
//----------------------------------------------------------------------------------------
void Eeprom_SelectChip(uint8_t EepromAddress, uint8_t EepromCapability)
{
    _selectedEepromAddress = EepromAddress;                              // Mémorise l'information
    _selectedEepromCapability = EepromCapability;                        // Mémorise l'information
}

//////////////////////////////////////////////////////////////////////////////////////////
//                                      LECTURE D'UN OCTET                              //
//////////////////////////////////////////////////////////////////////////////////////////
//----------------------------------------------------------------------------------------
// MemoryAddress    : Adresse de l'octet à lire 
// Retourne         : La valeur de l'octet lu
//----------------------------------------------------------------------------------------
uint8_t Eeprom_ReadByte(uint16_t MemoryAddress)
{
    uint8_t value;                                                       // Octet lu
    
    if(!BeginTransmission(MemoryAddress, EEP_READ))                      // Si pas de contrôle en lecture
        return false;                                                    // Echec
    
    value = I2C_Read(false);                                             // Lecture de l'octet pointé par MemoryAddress
    I2C_SendStop();                                                      // Libère la ligne
    return value;                                                        // Retourne l'octet lu
}

//////////////////////////////////////////////////////////////////////////////////////////
//                                       ECRIRE UN OCTET                                //
//////////////////////////////////////////////////////////////////////////////////////////
//----------------------------------------------------------------------------------------
// MemoryAddress      : Adresse de l'octet à écrire 
// Data               : Valeur de l'octet à écrire
// Retourne           : true ou false
//----------------------------------------------------------------------------------------
Boolean Eeprom_WriteByte(uint16_t MemoryAddress, uint8_t Data)
{
    
    if(!BeginTransmission(MemoryAddress, EEP_WRITE))                      // Si pas de contrôle en écriture
        return false;                                                     // Echec
    
    I2C_Write(Data);                                                      // Ecrit l'octet à l'adresse pointée par MemoryAddress
    return (Eeprom_ReadByte(MemoryAddress) == Data);                      // Relecture pour contrôle
}

//////////////////////////////////////////////////////////////////////////////////////////
//                                       LIRE PLUSIEURS OCTETS                          //
//////////////////////////////////////////////////////////////////////////////////////////
//----------------------------------------------------------------------------------------
// MemoryAddress       : Adresse du premier octet à lire 
// Array               : Tableau qui va contenir les valeurs lues
// ArraySize           : Taille du tableau
// Retourne            : true ou false
//----------------------------------------------------------------------------------------
Boolean Eeprom_ReadBytesArray(uint16_t MemoryAddress, uint8_t *Array, uint8_t ArraySize)
{
    uint8_t i;                                                             // Variable pour la boucle for
    
    if(!BeginTransmission(MemoryAddress, EEP_READ))                        // Si pas de contrôle en lecture
        return false;                                                      // Echec
    
    for (i=0; i<ArraySize; i++)                                            // Pour le nombre d'octets demandés
        Array[i] = I2C_Read(i != (ArraySize - 1));                         // Lecture et place dans le tableau de retour
    
    I2C_SendStop();                                                        // Libère la ligne
    return true;                                                           // Ok
}

//////////////////////////////////////////////////////////////////////////////////////////
//                                    ECRIRE PLUSIEURS OCTETS                           //
//////////////////////////////////////////////////////////////////////////////////////////
//----------------------------------------------------------------------------------------
// Attention à ne pas déborder de la page
//----------------------------------------------------------------------------------------
// MemoryAddress        : Adresse du premier octet à écrire 
// Array                : Tableau qui contient les valeurs à écrire
// ArraySize            : Taille du tableau
// Retourne             : true ou false
//----------------------------------------------------------------------------------------
Boolean Eeprom_WriteBytesArray(uint16_t MemoryAddress, uint8_t *Array, uint8_t ArraySize)
{
    uint8_t i;                                                              // Variable pour la boucle for
    
    switch(_selectedEepromCapability)
    {
        case 8:
            if (((MemoryAddress & 0x07) + ArraySize) > 8)                   // Si débordement de page
                return false;                                               // On arrête là
            break;
        case 64:
            if (((MemoryAddress & 0x3F) + ArraySize) > 64)                  // Si débordement de page
                return false;                                               // On arrête là
            break;
        case 128:
            if (((MemoryAddress & 0x7F) + ArraySize) > 128)                 // Si débordement de page
                return false;                                               // On arrête là
                break;
            default:                                                        // Page write non gérée
                return false;                                               // On arrête là
    }
    
    if(!BeginTransmission(MemoryAddress, EEP_WRITE))                        // Si pas de contrôle en écriture
        return false;                                                       // Echec
    
    for (i=0; i<ArraySize; i++)                                             // Pour le nombre d'octets à écrire
        I2C_Write(Array[i]);                                                // Ecriture
    
    I2C_SendStop();                                                         // Libère la ligne
    
    // VERIFICATION
    // ------------
    if(!BeginTransmission(MemoryAddress, EEP_READ))                         // Si pas de contrôle en lecture
        return false;                                                       // Echec
    
    for (i=0; i<ArraySize; i++)                                             // Pour chaque octet à lire
    {
        if(I2C_Read(i < (ArraySize - 1)) != Array[i])                       // Lecture et comparaison
        {
            I2C_SendStop();                                                 // Erreur donc on libère la ligne
            return false;                                                                                           
        }
    }
    return true;                                                                                                    
}

//////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////
//                              FONCTIONS PRIVÉES                                       //
//////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////////////
//                                     GESTION DU CONTROLE                              //
//////////////////////////////////////////////////////////////////////////////////////////
//----------------------------------------------------------------------------------------
// MemoryAddress        : Adresse du premier octet à lire ou écrire
// Mode                 : Lecture (EEP_READ) ou écriture (EEP_WRITE)
// Retourne             : true ou false
//----------------------------------------------------------------------------------------
static __inline Boolean BeginTransmission(uint16_t MemoryAddress, Boolean Mode)
{
    uint8_t timeout;                                                         // Timeout pour le cycle d'écriture
    uint8_t result;                                                          // Retour à tester
    
    // Si l'EEPROM est dans un cycle d'écriture, elle ne va pas répondre au contrôle.
    // On lui laisse donc 16ms maximum pour terminer et répondre sinon on déclare un échec
    for(timeout=0; timeout<8; timeout++)
    {
        result = I2C_SendControl(_selectedEepromAddress, EEP_WRITE);         // Envoyer le contrôle en écriture
        if(result)                                                           // Bien reçu, l'EEPROM est libre
        {
            // Toutes les 24LCxx ont un adressage sur 16 bits sauf la 24LC02 que j'identifie par la taille de sa page 
            if(_selectedEepromCapability > 8)                                // Si autre qu'une 24LC02
                result = I2C_Write(MemoryAddress >> 8);                      // Envoi les 8bits de poid fort de l'octet à pointer
            
            if(result)                                                       // Ok
            {
                result = I2C_Write(MemoryAddress);                           // Envoi les 8bits de poid faible de l'octet à pointer
                if(result)                                                   // Ok
                    break;                                                   // On sort de la boucle
            }
        }
        Timing_WaitUs(2000);                                                 // L'EEPROM n'est pas prête, on attends 2ms (j'ai eu des problèmes avec 1ms)
    }
    
    if(!result)                                                              // Si il y a eu un erreur dans le traitement ci-dessus
    {
        I2C_SendStop();                                                      // On libère la ligne
        return false;                                                        // Echec
    }
    
    if(Mode == EEP_READ)                                                     // Si on demande à lire en mémoire
        return I2C_SendControl(_selectedEepromAddress, EEP_READ);            // Retourne le résultat de la demande de contrôle en lecture
        
    return true;                                                             // Sinon c'était Ok
}
