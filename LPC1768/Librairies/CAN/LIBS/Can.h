//////////////////////////////////////////////////////////////////////////////////////////
//                             DÉCLARATIONS POUR LIBRAIRIE CAN.C                        //
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
// Objet : Déclarations pour la librairie Can.c                                         //
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

//////////////////////////////////////////////////////////////////////////////////////////
//                                     DEFINES                                          //
//////////////////////////////////////////////////////////////////////////////////////////

// Numéro des contrôleurs physiques CAN
#define CAN1    1
#define CAN2    2

// Numéros des paramètres CAN en flash
#define F_CAN1  0
#define F_CAN2  1

//--------------------------------------------------------------------------------
//                             CMR : LPC_CANx->CMR                               -
//--------------------------------------------------------------------------------

// Can Command Register (un registre par port Can)
// -----------------------------------------------

#define CANCMR_TR   0x1                                                        // Transmission Request
#define CANCMR_AT   0x2                                                        // Abord Transmission
#define CANCMR_RRB  0x4                                                        // Release Receive Buffer
#define CANCMR_CDO  0x8                                                        // Clear Data Overrun
#define CANCMR_SSR  0x10                                                       // Self Reception Request
#define CANCMR_STB1 0x20                                                       // Select Transmit Buffer 1
#define CANCMR_STB2 0x40                                                       // Select Transmit Buffer 2
#define CANCMR_STB3 0x80                                                       // Select Transmit Buffer 3

//--------------------------------------------------------------------------------
//                            SR : LPC_CANx->SR                                  -
//--------------------------------------------------------------------------------

// Can Status Register (un registre par port Can)
// ----------------------------------------------

#define CANSR_RBS   0x01                                                       // Receive Buffer Status
#define CANSR_DOS1  0x02                                                       // Data Overrun Status
#define CANSR_TBS1  0x04                                                       // Transmit Buffer Status 1 (1 = buffer TX1 libre)
#define CANSR_TCS1  0x08                                                       // Transmission Complete Status 1 (1 = terminé)
#define CANSR_RS1   0x10                                                       // Receive Status
#define CANSR_TS1   0x20                                                       // Transmit Status 1 (1 = transmission en cours sur TX1) 
#define CANSR_ES1   0x40                                                       // Error Status
#define CANSR_BS1   0x80                                                       // Bus Status
#define CANSR_RBS1  0x100                                                      // Receive Buffer Status
#define CANSR_DOS2  0x200                                                      // Identique à CANSR_DOS1
#define CANSR_TBS2  0x400                                                      // Transmit Buffer Status 2 (1 = buffer TX2 libre)
#define CANSR_TCS2  0x0800                                                     // Transmission Complete Status 2 (1 = terminé)
#define CANSR_RS2   0x1000                                                     // Identique à RS1
#define CANSR_TS2   0x2000                                                     // Transmit Status 2 (1 = transmission en cours sur TX2)
#define CANSR_ES2   0x4000                                                     // Identique à ES1
#define CANSR_BS2   0x8000                                                     // Identique à BS1
#define CANSR_RBS2  0x010000                                                   // Identique à RBS1
#define CANSR_DOS3  0x020000                                                   // Identique à CANSR_DOS1
#define CANSR_TBS3  0x040000                                                   // Transmit Buffer Status 3 (1 = buffer TX3 libre)
#define CANSR_TCS3  0x080000                                                   // Transmission Complete Status 3 (1 = terminé)
#define CANSR_RS3   0x100000                                                   // Identique à RS1
#define CANSR_TS3   0x200000                                                   // Transmit Status 3 (1 = transmission en cours sur TX3)
#define CANSR_ES3   0x400000                                                   // Identique à ES1
#define CANSR_BS3   0x800000                                                   // Identique à BS1

//////////////////////////////////////////////////////////////////////////////////////////
//                                STRUCTURES                                            //
//////////////////////////////////////////////////////////////////////////////////////////
#pragma anon_unions

//--------------------------------------------------------------------------------
//                           MESSAGE CAN CLASSIQUE                               -
//--------------------------------------------------------------------------------
typedef struct
{
    union
    {
        uint32_t Id;                                                           // Identificateur complet 29 bits
        struct
        {
            unsigned char IdLSB;                                               // Identificateur, octet LSB
            unsigned char IdMSB;                                               // Id, octet MSB
            unsigned char IdUSB;                                               // Id, octet Upper SB
            unsigned char IdVSB;                                               // Very Upper SB
        };
    };
    unsigned char TypeFrame;                                                   // b1: 1 = Extended ID ; b0: 1 = trame Remote
    unsigned char LenDat;                                                      // Nombre d'octets de data
    unsigned char Data[8];                                                     // Octets de data
} CAN_Msg;

//--------------------------------------------------------------------------------
//                           MESSAGE TYPÉ DOMOCAN CE                             -
//--------------------------------------------------------------------------------
// Doit rester compatible avec CAN_MSG (cast possible)
//--------------------------------------------------------------------------------
typedef struct
{
    union
    {   
        uint32_t Id;
        struct
        {
            unsigned Param : 8;
            unsigned NumCard : 8;
            unsigned Cmd : 5;
            unsigned TypeCard : 8;
            unsigned : 3;
        };
    };
    unsigned char TypeFrame;                                                   // b1: 1 = Extended ID ; b0: 1 = trame Remote : toujours 1/0
    unsigned char LenDat;                                                      // Nombre d'octets de data
    unsigned char Data[8];                                                     // Octets de data
} CAN_Msg_CE;

//////////////////////////////////////////////////////////////////////////////////////////
//                                    PROTOTYPES                                        //
//////////////////////////////////////////////////////////////////////////////////////////
//----------------------------------------------------------------------------
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
//----------------------------------------------------------------------------
extern void CAN_Initialize(int canModule, int clockDivider, int prescaler, 
                    int SJW, int prop, int PS1, int PS2, int SAMP);

//----------------------------------------------------------------------------
// Démarre le module Can précisé et précédemment configuré
// canModule : Précise le contrôleur CAN : CAN1 ou CAN2
//----------------------------------------------------------------------------
extern void CAN_Start(int canModule);                                          // Démarre le module CAN

//----------------------------------------------------------------------------
// Stoppe le module Can précisé et précédemment configuré (mode Reset).
// Nécessaire pour pouvoir modifier certains registres CAN
// canModule : Précise le contrôleur CAN : CAN1 ou CAN2
//----------------------------------------------------------------------------
extern void CAN_Stop(int canModule);                                           // Stoppe le module CAN

//----------------------------------------------------------------------------
// Valide les interruptions sur réception de trame
// canModule : Précise le contrôleur CAN : CAN1 ou CAN2
//----------------------------------------------------------------------------
extern void CAN_SetRxInterrupt(int canModule);                                 // Lance les interruptions RX

//----------------------------------------------------------------------------
// Valide les interruptions sur émission de trame
// canModule : précise le contrôleur CAN : CAN1 ou CAN2
// nbTxBuffers: 1,2 ou 3 : Interruption sur buffer1, ou 1+2, ou 1+2+3
//----------------------------------------------------------------------------
extern void CAN_SetTxInterrupt (int canModule, int nbTxBuffers);               // Lance les interruptions TX

//----------------------------------------------------------------------------
// Stoppe les interruptions CAN
// canModule : Précise le contrôleur CAN : CAN1 ou CAN2
//----------------------------------------------------------------------------
extern void CAN_StopInterrupt(int canModule);                                  // Stoppe les interruptions CAN

//----------------------------------------------------------------------------
// Lire un message CAN de type quelconque, puis libérer son buffer de réception
// canModule: contrôleur CAN: CAN1 ou CAN2
// *msg: Pointeur sur message à initialiser, évite de passer la structure sur la pile
//----------------------------------------------------------------------------
extern void CAN_ReadMsg(int canModule, CAN_Msg *msg);

//----------------------------------------------------------------------------------------
// ENVOIE UNE TRAME SUR LE BUS CAN
// canModule: numéro du module utilisé, CAN1 ou CAN2
// msg            : Message à envoyer sur le bus
// useOneTxBuffer : Si true, on n'utilise que le buffer TX1 (garantit l'ordre d'émission)
//----------------------------------------------------------------------------------------
extern void CAN_SendMsg(int canModule, CAN_Msg *msg, Boolean useOneTxBuffer);

