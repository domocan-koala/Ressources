//////////////////////////////////////////////////////////////////////////////////////////
//                     LIBRAIRIE DE GESTION DES MODULES I2C                             //
//////////////////////////////////////////////////////////////////////////////////////////
// Auteur               : Bigonoff                                                      //
// URL                  : www.bigonoff.org                                              //
//////////////////////////////////////////////////////////////////////////////////////////
// Historique                                                                           //
// ----------                                                                           //
// V1.00    18/05/2015  : Premi�re version en ligne                                     //
// V1.01    04/01/2018  : Devient une librairie autonome (Koala)                        //
//////////////////////////////////////////////////////////////////////////////////////////
// Objet : Gestion des modules I2C                                                      //
//////////////////////////////////////////////////////////////////////////////////////////
// Licence d'utilisation                                                                //
// ---------------------                                                                //
// Ce document est fourni tel quel, sans aucune garantie et sans recours d'aucune sorte //
// en cas de pr�judice direct ou indirect li� ou non � son utilisation.                 //
// Vous avez le droit d'utiliser et/ou de modifier ce document dans le cadre d'une      //
// utilisation priv�e, publique, ou commerciale.                                        //
// Vous ne pouvez distribuer ce document qu'accompagn� du pr�sent commentaire original  //
// int�gral incluant l'historique et les r�f�rences de l'auteur.                        //
// En cas de distribution d'une version modifi�e, vous devrez ajouter au pr�sent        //
// commentaire la liste des points modifi�s, la raison de la modification, sa date,     //
// ainsi que l'identification du modificateur.                                          //
// Il n'est pas autoris� de vendre ce document ou une version modifi�e de celui-ci.     //
// Les codes ex�cutables produits � partir de ce document sont soumis � cette licence   //
// d'utilsation.                                                                        //
// Il n'est pas autoris� de distribuer les codes ex�cutables sans distribuer les        //
// fichiers sources ayant servi � les g�n�rer.                                          //
// En cas de traduction, seule la version originale fran�aise obtenue � partir du site  //
// de l'auteur fait r�f�rence. Il en va de m�me concernant les modifications.           //
// L'auteur s'est efforc� de r�aliser cette librairie d'apr�s les documents techniques  //
// officiels. Toute ressemblance avec un document existant ne serait que pure           //
// co�ncidence.                                                                         //
//////////////////////////////////////////////////////////////////////////////////////////

#include <lpc17xx.h>
#include "LPC17xx_Bits.h"
#include "I2C.h"

//////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////
//                               VARIABLES GLOBALES                                     //
//////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////

LPC_I2C_TypeDef *_i2c = null;                                       // Pointeur sur le module I2C � utiliser

//////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////
//                                 PROTOTYPES                                           //
//////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////

static __inline void WaitSI(void);                                  // Attend la fin de l'op�ration en cours
static __inline void ClearSI(void);                                 // Efface le flag et relance les op�rations

//////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////
//                               FONCTIONS PUBLIQUES                                    //
//////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////////////
//                        INITIALISE LE MODULE I2C UTILIS�                              //
//////////////////////////////////////////////////////////////////////////////////////////
//----------------------------------------------------------------------------------------
// NumI2C : Num�ro du module I2C � utiliser (I2C0, I2C1 ou I2C2)
//----------------------------------------------------------------------------------------
void I2C_Initialize(int NumI2C)
{
    switch(NumI2C)
    {
        case I2C0:
        default:
            _i2c = LPC_I2C0;                                        // Pointeur sur I2C0
            LPC_PINCON->PINSEL1 &= ~0x03C00000;                     // Reset bits
            LPC_PINCON->PINSEL1 |= 0x01400000;                      // P0.27 = SDA0 - P0.28 = SCL0
            break;
        
        case I2C1:                                                                          
            _i2c = LPC_I2C1;                                        // Pointeur sur I2C1
            LPC_PINCON->PINSEL0 |= 0x0F;                            // P0.00 = SDA1 - P0.01 = SCL1
            LPC_PINCON->PINMODE0 &= ~0x05;                          // Reset bits
            LPC_PINCON->PINMODE0 |= 0x0A;                           // Pas de pullup/pulldown
            LPC_PINCON->PINMODE_OD0 |= 0x03;                        // Open drain
            break;
        
        case I2C2:
            _i2c = LPC_I2C2;                                        // Pointeur sur I2C2
            LPC_PINCON->PINSEL0 &= ~0x00500000;                     // Reset bits
            LPC_PINCON->PINSEL0 |= 0x00A00000;                      // P0.10 = SDA2 - P0.11 = SCL2
            LPC_PINCON->PINMODE0 &= ~0x00500000;                    // Reset bits
            LPC_PINCON->PINMODE0 |= 0x00A00000;                     // Pas de pullup/pulldown
            LPC_PINCON->PINMODE_OD0 |= 0x00000C00;                  // Open drain
            break;
    }
    
    // Reset des bits de contr�le par s�curit�
    _i2c->I2CONCLR = I2CONCLR_AAC | I2CONCLR_SIC | I2CONCLR_STAC | I2CONCLR_I2ENC;  

    // Mettre vitesse sur max 100Kbits/s (max pour eeprom aliment�e en 3V)
    // On laisse l'horloge du module I2C sur 100Mhz/4 (par d�faut)
    // (I2CSCLH + I2CSCLL) = PCLK_I2C / I2C_bit_frequency   = 25 / 0.1 = 250
    // Pour un signal d'horloge carr�, on r�partit moiti� moiti� sur niveau haut et bas
    // Soit une valeur pour chacun de 125 = 0x7D
    _i2c->I2SCLL = _i2c->I2SCLH = 0x7D; 
    
    _i2c->I2CONSET = I2CONSET_I2EN;                                 // D�marrer le module I2C
}

//////////////////////////////////////////////////////////////////////////////////////////
//                              ENVOIE LE START-CONDITION                               //
//////////////////////////////////////////////////////////////////////////////////////////
//----------------------------------------------------------------------------------------
// Envoie le start-condition ou le repeated-start-condition
// Ceci d�marre le transfert
//----------------------------------------------------------------------------------------
void I2C_SendStart(void)
{
    _i2c->I2CONCLR = I2CONCLR_AAC;                                  // Mode master
    ClearSI();                                                      // Lib�re les transferts
    _i2c->I2CONSET = I2CONSET_STA;                                  // Envoi le start-condition
    WaitSI();                                                       // Attendre fin de l'op�ration
    _i2c->I2CONCLR = I2CONCLR_STAC;                                 // Fin du start
}

//////////////////////////////////////////////////////////////////////////////////////////
//                               ENVOIE LE STOP-CONDITION                               //
//////////////////////////////////////////////////////////////////////////////////////////
//----------------------------------------------------------------------------------------
// Envoie le stop-condition et attend la fin de l'op�ration
//----------------------------------------------------------------------------------------
void I2C_SendStop(void)
{
    _i2c->I2CONSET = I2CONSET_STO;                                  // Envoyer stop-condition
    _i2c->I2CONCLR = I2CONCLR_SIC;                                  // Lib�rer le flag
    while (_i2c->I2CONSET & I2CONSET_STO);                          // Attendre fin de l'op�ration (auto-reset du bit)
}

//////////////////////////////////////////////////////////////////////////////////////////
//                                  �CRIT UN OCTET DE DATA                              //
//////////////////////////////////////////////////////////////////////////////////////////
//----------------------------------------------------------------------------------------
// �crit un octet de data et attend la fin de l'op�ration
//----------------------------------------------------------------------------------------
Boolean I2C_Write(unsigned char Data)
{
    _i2c->I2DAT = Data;                                             // Placer data
    ClearSI();                                                      // Relancer transferts
    WaitSI();                                                       // Attendre fin de l'op�ration
    return (I2C_GetStat() == I2STAT_WDATA_ACKED);
}

//////////////////////////////////////////////////////////////////////////////////////////
//                OBTIENT LE STATUT DE LA DERNI�RE OP�RATION R�ALIS�E                   //
//////////////////////////////////////////////////////////////////////////////////////////
//----------------------------------------------------------------------------------------
// Obtient le statut actuel du transfert
//----------------------------------------------------------------------------------------
unsigned char I2C_GetStat(void)
{
    return _i2c->I2STAT;                                            // Retourne le status
}

//////////////////////////////////////////////////////////////////////////////////////////
//                             �CRIT L'OCTET DE CONTR�LE                                //
//////////////////////////////////////////////////////////////////////////////////////////
//----------------------------------------------------------------------------------------
// Address  : Adresse du p�riph�rique � contacter sur 7bits d�cal�s � gauche
// Mode     : I2C_READ ou I2C_WRITE
// Retourne : True si l'eeprom est libre dans un temps max de 15ms
//----------------------------------------------------------------------------------------
Boolean I2C_SendControl(unsigned char Address, int Mode)
{
    if(Mode == I2C_READ)                                            // Si on demande le mode lecture                                
        Address |= 0x01;                                            // Placer le bit READ/WRITE (b0) � 1
    
        if (I2C_GetStat() != I2STAT_IDLE)                           // Si la ligne n'est pas au repos
            I2C_SendStop();                                         // Envoyer stop-condition
        
        I2C_SendStart();                                            // Envoyer start-condition
        I2C_Write(Address);                                         // Adresse du p�riph�rique + READ/WRITE
        
        if (I2C_GetStat() == I2STAT_WCTRL_ACKED || I2C_GetStat() == I2STAT_RCTRL_ACKED)
            return true;

        I2C_SendStop();
        return false;
}

//////////////////////////////////////////////////////////////////////////////////////////
//                                  LIT UN OCTET DE DATA                                //
//////////////////////////////////////////////////////////////////////////////////////////
//----------------------------------------------------------------------------------------
// Lit un octet de data dans le transfert en cours
// Ack  : si true on envoie le ACK, sinon le NACK
// On attend la fin de l'envoi avant de sortir
//----------------------------------------------------------------------------------------
unsigned char I2C_Read(Boolean Ack)
{
    if (Ack)                                                         // Si on demande ACK
        _i2c->I2CONSET = I2CONSET_AA;                                // Placer ACK
    else                                                             // Sinon
        _i2c->I2CONCLR = I2CONCLR_AAC;                               // Placer NACK
    
    ClearSI();                                                       // Relancer transferts
    WaitSI();                                                        // Attendre fin de r�ception 
    return _i2c->I2DAT;                                              // Retourne l'octet de data
}

//////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////
//                              FONCTIONS PRIV�ES                                       //
//////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////////////
//                         ATTEND LA FIN DE L'OP�RATION EN COURS                        //
//////////////////////////////////////////////////////////////////////////////////////////
//----------------------------------------------------------------------------------------
// Attend la fin de l'op�ration en cours
// S'il n'y a aucune op�ration r�ellement en cours, l'op�ration est d�finitivement bloquante
//----------------------------------------------------------------------------------------
static __inline void WaitSI(void)
{
    while ((_i2c->I2CONSET & I2CONSET_SI) == 0);                     // Attendre remont�e de SI
}

//////////////////////////////////////////////////////////////////////////////////////////
//                                 RELANCE LES TRANSFERTS                               //
//////////////////////////////////////////////////////////////////////////////////////////
//----------------------------------------------------------------------------------------
// Lib�re le flag d'interruption pour relancer les transferts
//----------------------------------------------------------------------------------------
static __inline void ClearSI(void)
{
    _i2c->I2CONCLR = I2CONCLR_SIC;                                   // Lib�re SI
}
