//////////////////////////////////////////////////////////////////////////////////////////
//                             DÉCLARATIONS POUR MODULES I2C.C                          //
//////////////////////////////////////////////////////////////////////////////////////////
// Auteur               : Bigonoff                                                      //
// URL                  : www.bigonoff.org                                              //
//////////////////////////////////////////////////////////////////////////////////////////
// Historique                                                                           //
// ----------                                                                           //
// V1.00    18/05/2015  : Première version en ligne                                     //
// V1.01    04/01/2018  : Devient une librairie autonome (Koala)                        //
//////////////////////////////////////////////////////////////////////////////////////////
// Objet : Gestion des modules I2C                                                      //
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
//////////////////////////////////////////////////////////////////////////////////////////
//                                     DEFINES                                          //
//////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////////////
//                                      MODULES I2C DISPONIBLES                         //
//////////////////////////////////////////////////////////////////////////////////////////
#define I2C0                    0                       // I2C0 : SDA = P0.27, SCL = P0.28
#define I2C1                    1                       // I2C1 : SDA = P0.00, SCL = P0.01
#define I2C2                    2                       // I2C2 : SDA = P0.10, SCL = P0.11

//////////////////////////////////////////////////////////////////////////////////////////
//                                          MODE POUR LE CONTROLE                       //
//////////////////////////////////////////////////////////////////////////////////////////
#define I2C_WRITE               0                               
#define I2C_READ                1

//////////////////////////////////////////////////////////////////////////////////////////
//                       REGISTRE I2CONSET : LPC_I2Cn->I2CONSET                         //
//////////////////////////////////////////////////////////////////////////////////////////
//----------------------------------------------------------------------------------------
// Registre de forçage des bits de contrôle à 1 : Lecture/Écriture
// Les explications concernent le mode maître
// Le reset des bits s'effectue via le registre I2CONCLR
//----------------------------------------------------------------------------------------
#define I2CONSET_AA             0x04                     // Si 1 : émission d'un ACK sur réception d'un octet
#define I2CONSET_SI             0x08                     // Flag d'interruption I²C: modification d'état
#define I2CONSET_STO            0x10                     // Stop Flag : Émission d'un stop-condition (reset auto)
#define I2CONSET_STA            0x20                     // Start Flag : Émission d'un start-condition ou d'un Repeat-start
#define I2CONSET_I2EN           0x40                     // 1 = Mise en service du module

//////////////////////////////////////////////////////////////////////////////////////////
//                       REGISTRE I2CONSET : LPC_I2Cn->I2CONCLR                         //
//////////////////////////////////////////////////////////////////////////////////////////
//----------------------------------------------------------------------------------------
// Registre de reset de contrôle (mettre 1 = reset bit) : Écriture
// Le positionnement à 1 s'effectue via le registre I2CONSET
//----------------------------------------------------------------------------------------
#define I2CONCLR_AAC            0x04                    // Reset du bit AA
#define I2CONCLR_SIC            0x08                    // Reset du flag d'interruption SI
#define I2CONCLR_STAC           0x20                    // Reset du start-condition STA
#define I2CONCLR_I2ENC          0x40                    // Mise Hors-service du module

//////////////////////////////////////////////////////////////////////////////////////////
//              VALEURS POSSIBLES DU REGISTRE I2STAT : LPC_I2Cn->I2STAT                 //
//////////////////////////////////////////////////////////////////////////////////////////
//----------------------------------------------------------------------------------------
// Registre renseignant sur l'état actuel du module: Lecture
// Attention: Ce ne sont pas des bits, ce sont des valeurs concaténées: On lit le registre
// et la valeur retournée est une des valeurs définies ici
// Seules les valeurs concernant le mode Master sont reprises ici
//----------------------------------------------------------------------------------------
// Transmission
#define I2STAT_WCTRL_ACKED      0x18                    // l'octet de contrôle (adresse esclave + W) a été transmis et un ACK reçu
#define I2STAT_WCTRL_NACKED     0x20                    // l'octet de contrôle a été transmis et un NACK a été reçu
#define I2STAT_WDATA_ACKED      0x28                    // L'octet de data a été envoyé, un Ack a été reçu
#define I2STAT_WDATA_NACKED     0x30                    // L'octet de data a été envoyé, un NACK a été reçu
#define I2STAT_WLOSTED          0x38                    // perte d'arbitrage durant l'envoi d'un octet

// Réception
#define I2STAT_RLOSTED          0x38                    // perte d'arbitrage durant l'envoi du NACK
#define I2STAT_RCTRL_ACKED      0x40                    // l'octet de contrôle (adresse esclave + R) a été transmis et un ACK reçu
#define I2STAT_RCTRL_NACKED     0x48                    // l'octet de contrôle a été transmis et un NACK a été reçu
#define I2STAT_RDATA_ACKED      0x50                    // L'octet de data a été reçu, un Ack a été envoyé
#define I2STAT_RDATA_NACKED     0x58                    // L'octet de data a été reçu, un NACK a été envoyé

// Commun
#define I2STAT_ERROR            0x00                    // Bus error : lancer un stop
#define I2STAT_IDLE             0xF8                    // Ligne au repos
#define I2STAT_STARTED          0x08                    // Start-condition envoyé
#define I2STAT_RSTARTED         0x10                    // Repeated start-condition envoyé

//////////////////////////////////////////////////////////////////////////////////////////
//                                  PROTOTYPES                                          //
//////////////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////////////
//                        INITIALISE LE MODULE I2C UTILISÉ                              //
//////////////////////////////////////////////////////////////////////////////////////////
//----------------------------------------------------------------------------------------
// NumI2C : Numéro du module I2C à utiliser (I2C0, I2C1 ou I2C2)
//----------------------------------------------------------------------------------------
extern void I2C_Initialize(int NumI2C);

//////////////////////////////////////////////////////////////////////////////////////////
//                              ENVOIE LE START-CONDITION                               //
//////////////////////////////////////////////////////////////////////////////////////////
//----------------------------------------------------------------------------------------
// Envoie le start-condition ou le repeated-start-condition
// Ceci démarre le transfert
//----------------------------------------------------------------------------------------
extern void I2C_SendStart(void);

//////////////////////////////////////////////////////////////////////////////////////////
//                               ENVOIE LE STOP-CONDITION                               //
//////////////////////////////////////////////////////////////////////////////////////////
//----------------------------------------------------------------------------------------
// Envoie le stop-condition et attend la fin de l'opération
//----------------------------------------------------------------------------------------
extern void I2C_SendStop(void);

//////////////////////////////////////////////////////////////////////////////////////////
//                                  ÉCRIT UN OCTET DE DATA                              //
//////////////////////////////////////////////////////////////////////////////////////////
//----------------------------------------------------------------------------------------
// Écrit un octet de data et attend la fin de l'opération
//----------------------------------------------------------------------------------------
extern Boolean I2C_Write(unsigned char Data);

//////////////////////////////////////////////////////////////////////////////////////////
//                OBTIENT LE STATUT DE LA DERNIÈRE OPÉRATION RÉALISÉE                   //
//////////////////////////////////////////////////////////////////////////////////////////
//----------------------------------------------------------------------------------------
// Obtient le statut actuel du transfert
//----------------------------------------------------------------------------------------
extern unsigned char I2C_GetStat(void);

//////////////////////////////////////////////////////////////////////////////////////////
//                             ÉCRIT L'OCTET DE CONTRÔLE                                //
//////////////////////////////////////////////////////////////////////////////////////////
//----------------------------------------------------------------------------------------
// Address  : Adresse du périphérique à contacter sur 7bits décalés à gauche
// Mode     : I2C_READ ou I2C_WRITE
// Retourne : True si l'eeprom est libre dans un temps max de 15ms
//----------------------------------------------------------------------------------------
extern Boolean I2C_SendControl(unsigned char Address, int Mode);

//////////////////////////////////////////////////////////////////////////////////////////
//                                  LIT UN OCTET DE DATA                                //
//////////////////////////////////////////////////////////////////////////////////////////
//----------------------------------------------------------------------------------------
// Lit un octet de data dans le transfert en cours
// Ack  : si true on envoie le ACK, sinon le NACK
// On attend la fin de l'envoi avant de sortir
//----------------------------------------------------------------------------------------
extern unsigned char I2C_Read(Boolean Ack);
