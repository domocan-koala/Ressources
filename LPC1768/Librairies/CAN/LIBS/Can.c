//////////////////////////////////////////////////////////////////////////////////////////
//                       LIBRAIRIE DE GESTION DES MODULES CAN                           //
//////////////////////////////////////////////////////////////////////////////////////////
// Auteur               : Bigonoff                                                      //
// URL                  : www.bigonoff.org                                              //
// Projet               : Système domotique Domocan                                     //
// URL projet           : www.domocan.org                                               //
//////////////////////////////////////////////////////////////////////////////////////////
// Historique                                                                           //
// ----------                                                                           //
// V1.00    30/03/2015  : Première version en ligne                                     //
//////////////////////////////////////////////////////////////////////////////////////////
// Objet : Librairie de gestion des modules CAN d'un ARM1768 monté sur une carte        //
// de développement compatible LANDTIGER                                                //
// Librairie développée dans le cadre du projet Domocan, et donc sujette à modifications//
// en fonction des besoins futurs.                                                      //
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

#include "LPC17xx.h"
#include "LPC17xx_Bits.h"
#include "Can.h"

//////////////////////////////////////////////////////////////////////////////////////////
//                           CONFIGURATION DES MODULES CAN                              //
//////////////////////////////////////////////////////////////////////////////////////////
//----------------------------------------------------------------------------------------
// Affecte les pins concernées, configure et met le module précisé sous tension
// canModule : Numéro du contrôleur Can : CAN1 ou CAN2
// clockDivider : Diviseur d'horloge pour CAN : 1,2,4 ou 6
// prescaler :  de 1 à 1024
//              Diviseur servant à établir Tq (Time Quantum),
//              Tq = Tclk * clockDivider * prescaler
//              Tclk = 10ns (oscillateur sur 100Mhz)
// SJW : Synchro Jump Width: de 1 à 4
//       Mettre à 1Tq (min) pour processeurs à quartz
// prop : Temps de propagation sur la ligne : de 1 à 15
// PS1  : Phase segment 1 : 1 <= (prop + PS1) <= 16
// PS2  : Phase segment 2 : 1 <= PS2 <= 8 : PS2 >= PS1
// SAMP : 0 = un seul point d'échantillonnage, 1 = 3 points sur porte majoritaire
// 
// Nominal Bit Time = (Synchro (1) + prop + PS1 + PS2) * Tq
//----------------------------------------------------------------------------------------
void CAN_Initialize(int canModule, int clockDivider, int prescaler, int SJW, int prop, 
                    int PS1, int PS2, int SAMP)
{
    LPC_CAN_TypeDef *pCan;                                                     // Pointeur sur contrôleur CAN
    uint32_t mask1 = PCLKSEL0_ACF_6;                                           // 2 bits du diviseur du filtre
    uint32_t mask2;                                                            // Masque pour xor
    
    if (canModule == CAN1)
    {
        pCan = LPC_CAN1;                                                       // Pointer sur contrôleur CAN1
        mask1 |= PCLKSEL0_CAN1_6;                                              // 2 bits du diviseur CAN1
        LPC_PINCON->PINSEL0 |= 0x05;                                           // P0.0 = RD1, P0.1 = TD1
        LPC_SC->PCONP |= PCONP_PCCAN1;                                         // Mettre CAN1 sous tension
    }   
    else
    {
        pCan = LPC_CAN2;                                                       // Pointer sur contrôleur CAN2
        mask1 |= PCLKSEL0_CAN2_6;                                              // 2 bits du diviseur CAN2
        LPC_PINCON->PINSEL0 |= 0xA00;                                          // P0.4 = RD2, P0.5 = TD2
        LPC_SC->PCONP |= PCONP_PCCAN2;                                         // Mettre CAN2 sous tension
    }
    
    pCan->MOD = 1;                                                             // Module stoppé, mode reset, nécessaire pour modifier registres
    pCan->IER = 0;                                                             // Toutes les interruptions CAN H.S.
    pCan->GSR = 0;                                                             // Reset compteurs d'erreurs
    LPC_CANAF->AFMR = 2;                                                       // Aucun filtre (nécessaire pour modifier filtres)

    // Positionner diviseurs CAN + FILTRE   
    switch (clockDivider)                                                      // En fonction du diviseur
    {
        case 1:                                                                
            mask2 = PCLKSEL0_CAN1_1 | PCLKSEL0_CAN2_1 | PCLKSEL0_ACF_1;        // /1 = 01 (pour CAN1 et CAN2)
            break;

        case 2:
            mask2 = PCLKSEL0_CAN1_2 | PCLKSEL0_CAN2_2 | PCLKSEL0_ACF_2;        // /2 = 10
            break;

        case 6:
            mask2 = PCLKSEL0_CAN1_6 | PCLKSEL0_CAN2_6 | PCLKSEL0_ACF_6;        // /6 = 11
            break;

        default:                                                               // /4 = 00
            mask2 = 0;
    }
    LPC_SC->PCLKSEL0 = (LPC_SC->PCLKSEL0 & (~mask1) | (mask2 & mask1));        // Positionner bits

    // Paramétrer BitTime Can
    pCan->BTR = (prescaler - 1) | ((SJW-1)<<14) |((prop + PS1 - 1)<<16) | ((PS2 - 1)<<20)
                | (SAMP << 23);
}

//////////////////////////////////////////////////////////////////////////////////////////
//                                DÉMARRE LE MODULE CAN                                 //
//////////////////////////////////////////////////////////////////////////////////////////
//----------------------------------------------------------------------------------------
// Démarre le module Can précisé et précédemment configuré
// canModule : précise le contrôleur CAN : CAN1 ou CAN2
//----------------------------------------------------------------------------------------
void CAN_Start(int canModule)
{
    LPC_CAN_TypeDef *pCan = (canModule == CAN1)? LPC_CAN1 : LPC_CAN2;          // Pointer sur le bon contrôleur CAN
    pCan->MOD = 0;                                                             // Démarrer module CAN
}

//////////////////////////////////////////////////////////////////////////////////////////
//                                STOPPE LE MODULE CAN                                  //
//////////////////////////////////////////////////////////////////////////////////////////
//----------------------------------------------------------------------------------------
// Stoppe le module Can précisé et précédemment configuré (mode Reset).
// Nécessaire pour pouvoir modifier certains registres CAN
// canModule : précise le contrôleur CAN : CAN1 ou CAN2
//----------------------------------------------------------------------------------------
void CAN_Stop(int canModule)
{
    LPC_CAN_TypeDef *pCan = (canModule == CAN1)? LPC_CAN1 : LPC_CAN2;          // Pointer sur le bon contrôleur CAN
    pCan->MOD = 1;                                                             // Stopper module CAN
}

//////////////////////////////////////////////////////////////////////////////////////////
//                  VALIDE LES INTERRUPTIONS  SUR RÉCEPTION DE TRAME                    //
//////////////////////////////////////////////////////////////////////////////////////////
//----------------------------------------------------------------------------------------
// canModule : précise le contrôleur CAN : CAN1 ou CAN2
//----------------------------------------------------------------------------------------
void CAN_SetRxInterrupt(int canModule)
{
    LPC_CAN_TypeDef *pCan = (canModule == CAN1)? LPC_CAN1 : LPC_CAN2;          // Pointer sur le bon contrôleur CAN
    pCan->IER |= 0x01;                                                         // Interruption Rx en service
    NVIC_EnableIRQ(CAN_IRQn);                                                  // Lancer les interruptions CAN
}

//////////////////////////////////////////////////////////////////////////////////////////
//                  VALIDE LES INTERRUPTIONS  SUR ÉMISSION DE TRAME                     //
//////////////////////////////////////////////////////////////////////////////////////////
//----------------------------------------------------------------------------------------
// canModule : précise le contrôleur CAN : CAN1 ou CAN2
// nbTxBuffers: 1,2 ou 3 : Interruption sur buffer1, ou 1+2, ou 1+2+3
//----------------------------------------------------------------------------------------
void CAN_SetTxInterrupt (int canModule, int nbTxBuffers)
{
    int valIER = 0x01;                                                         // Par défaut, TX 1
    LPC_CAN_TypeDef *pCan = (canModule == CAN1)? LPC_CAN1 : LPC_CAN2;          // Pointer sur le bon contrôleur CAN
    if (nbTxBuffers > 1)                                                       // Si au moins 2 buffers
    {
        valIER |= 0x20;                                                        // Interruption sur buffer 2
        if (nbTxBuffers > 2)                                                   // Si 3 buffers
            valIER |= 0x040;                                                   // Activer interruption buffer 3
    }
    pCan->IER |= valIER;                                                       // Interruption Tx en service
    NVIC_EnableIRQ(CAN_IRQn);                                                  // Lancer les interruptions CAN
}

//////////////////////////////////////////////////////////////////////////////////////////
//                             STOPPER INTERRUPTIONS CAN                                //
//////////////////////////////////////////////////////////////////////////////////////////
//----------------------------------------------------------------------------------------
// canModule : précise le contrôleur CAN : CAN1 ou CAN2
//----------------------------------------------------------------------------------------
void CAN_StopInterrupt(int canModule)
{
    LPC_CAN_TypeDef *pCan = (canModule == CAN1)? LPC_CAN1 : LPC_CAN2;          // Pointer sur le bon contrôleur CAN
    NVIC_DisableIRQ(CAN_IRQn);                                                 // Désactiver interruptions
    pCan->IER = 0x00;                                                          // Désactiver interruptions CAN
}

//////////////////////////////////////////////////////////////////////////////////////////
//                                 LIRE UN MESSAGE CAN                                  //
//////////////////////////////////////////////////////////////////////////////////////////
//----------------------------------------------------------------------------------------
// Lire un message CAN puis libérer son buffer de réception
// canModule: contrôleur CAN: CAN1 ou CAN2
// msg: pointeur sur message à initialiser, évite de passer la structure sur la pile
//----------------------------------------------------------------------------------------
void CAN_ReadMsg(int canModule, CAN_Msg *msg)
{
    uint32_t rfs;                                                              // Copie du registre RFS
    LPC_CAN_TypeDef *pCan = (canModule == CAN1)? LPC_CAN1 : LPC_CAN2;          // Pointer sur le bon contrôleur CAN
    
    msg->Id = pCan->RID;                                                       // Lire ID (11 ou 29 bits)
    
    rfs = pCan->RFS;                                                           // lire RFS
    msg->TypeFrame = (unsigned char)(rfs >> 30);                               // b1 = extended, b0 = remote
    msg->LenDat = (unsigned char)((rfs >> 16) & 0x0F);                         // Nbre data

    *((uint32_t *)(&msg->Data[0])) = pCan->RDA;                                // Placer D0/D3
    *((uint32_t *)(&msg->Data[4])) = pCan->RDB;                                // Placer D4/D7
    
    LPC_CAN1->CMR = CANCMR_RRB;                                                // Libérer buffer réception -> reset flag d'interruption RX
}

//////////////////////////////////////////////////////////////////////////////////////////
//                                 ENVOYER UN MESSAGE CAN                               //
//////////////////////////////////////////////////////////////////////////////////////////
//----------------------------------------------------------------------------------------
// Envoie une trame sur le bus CAN
// canModule: numéro du module utilisé, CAN1 ou CAN2
// msg : message à envoyer sur le bus
// useOneTxBuffer : si true, on n'utilise que le buffer TX1 (garantit l'ordre d'émission)
//----------------------------------------------------------------------------------------
void CAN_SendMsg(int canModule, CAN_Msg *msg, Boolean useOneTxBuffer)
{
    uint32_t SR;
    uint32_t *ptr;
    uint32_t cmrValue;
    LPC_CAN_TypeDef *pCan = (canModule == CAN1)? LPC_CAN1 : LPC_CAN2;          // Pointer sur le bon contrôleur CAN


    // Attendre un buffer libre
    while (true)
    {
        SR = pCan->SR;                                                         // Récupérer statut des buffers
        if (SR & CANSR_TBS1)                                                   // Si TX1 libre
        {
            ptr = (uint32_t*)(&pCan->TFI1);                                    // Pointer sur premier des registres Tx concernés
            cmrValue = CANCMR_TR | CANCMR_STB1;                                // Pour démarrage envoi sur TX1
            break;                                                             // OK
        }
        
        if(!useOneTxBuffer)                                                    // On teste les autres buffers seulement si l'utilisateur l'a demandé
        {
            if (SR & CANSR_TBS2)
            {
                ptr = (uint32_t*)(&pCan->TFI2);
                cmrValue = CANCMR_TR | CANCMR_STB2;
                break;
            }
            if (SR & CANSR_TBS3)
            {
                ptr = (uint32_t*)(&pCan->TFI3);
                cmrValue = CANCMR_TR | CANCMR_STB3;
                break;
            }
        }
    }
    
    // Initialiser message à envoyer
    *ptr++ = (((uint32_t)msg->LenDat) << 16) | (msg->TypeFrame << 30);         // DLC, REMOTE, et EXTENDED -> TFI
    *ptr++ = (msg->Id & 0x1FFFFFFF);                                           // ID -> TID
    *ptr++ = *((uint32_t*)(&msg->Data[0]));                                    // D0/D3 -> TDA
    *ptr = *((uint32_t*)(&msg->Data[4]));                                      // D4/D7 -> TDB

    // Déclencher l'envoi
    pCan->CMR = cmrValue;
}
