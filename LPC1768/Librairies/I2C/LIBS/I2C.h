//////////////////////////////////////////////////////////////////////////////////////////
//                             D�CLARATIONS POUR MODULES I2C.C                          //
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
// Registre de for�age des bits de contr�le � 1 : Lecture/�criture
// Les explications concernent le mode ma�tre
// Le reset des bits s'effectue via le registre I2CONCLR
//----------------------------------------------------------------------------------------
#define I2CONSET_AA             0x04                     // Si 1 : �mission d'un ACK sur r�ception d'un octet
#define I2CONSET_SI             0x08                     // Flag d'interruption I�C: modification d'�tat
#define I2CONSET_STO            0x10                     // Stop Flag : �mission d'un stop-condition (reset auto)
#define I2CONSET_STA            0x20                     // Start Flag : �mission d'un start-condition ou d'un Repeat-start
#define I2CONSET_I2EN           0x40                     // 1 = Mise en service du module

//////////////////////////////////////////////////////////////////////////////////////////
//                       REGISTRE I2CONSET : LPC_I2Cn->I2CONCLR                         //
//////////////////////////////////////////////////////////////////////////////////////////
//----------------------------------------------------------------------------------------
// Registre de reset de contr�le (mettre 1 = reset bit) : �criture
// Le positionnement � 1 s'effectue via le registre I2CONSET
//----------------------------------------------------------------------------------------
#define I2CONCLR_AAC            0x04                    // Reset du bit AA
#define I2CONCLR_SIC            0x08                    // Reset du flag d'interruption SI
#define I2CONCLR_STAC           0x20                    // Reset du start-condition STA
#define I2CONCLR_I2ENC          0x40                    // Mise Hors-service du module

//////////////////////////////////////////////////////////////////////////////////////////
//              VALEURS POSSIBLES DU REGISTRE I2STAT : LPC_I2Cn->I2STAT                 //
//////////////////////////////////////////////////////////////////////////////////////////
//----------------------------------------------------------------------------------------
// Registre renseignant sur l'�tat actuel du module: Lecture
// Attention: Ce ne sont pas des bits, ce sont des valeurs concat�n�es: On lit le registre
// et la valeur retourn�e est une des valeurs d�finies ici
// Seules les valeurs concernant le mode Master sont reprises ici
//----------------------------------------------------------------------------------------
// Transmission
#define I2STAT_WCTRL_ACKED      0x18                    // l'octet de contr�le (adresse esclave + W) a �t� transmis et un ACK re�u
#define I2STAT_WCTRL_NACKED     0x20                    // l'octet de contr�le a �t� transmis et un NACK a �t� re�u
#define I2STAT_WDATA_ACKED      0x28                    // L'octet de data a �t� envoy�, un Ack a �t� re�u
#define I2STAT_WDATA_NACKED     0x30                    // L'octet de data a �t� envoy�, un NACK a �t� re�u
#define I2STAT_WLOSTED          0x38                    // perte d'arbitrage durant l'envoi d'un octet

// R�ception
#define I2STAT_RLOSTED          0x38                    // perte d'arbitrage durant l'envoi du NACK
#define I2STAT_RCTRL_ACKED      0x40                    // l'octet de contr�le (adresse esclave + R) a �t� transmis et un ACK re�u
#define I2STAT_RCTRL_NACKED     0x48                    // l'octet de contr�le a �t� transmis et un NACK a �t� re�u
#define I2STAT_RDATA_ACKED      0x50                    // L'octet de data a �t� re�u, un Ack a �t� envoy�
#define I2STAT_RDATA_NACKED     0x58                    // L'octet de data a �t� re�u, un NACK a �t� envoy�

// Commun
#define I2STAT_ERROR            0x00                    // Bus error : lancer un stop
#define I2STAT_IDLE             0xF8                    // Ligne au repos
#define I2STAT_STARTED          0x08                    // Start-condition envoy�
#define I2STAT_RSTARTED         0x10                    // Repeated start-condition envoy�

//////////////////////////////////////////////////////////////////////////////////////////
//                                  PROTOTYPES                                          //
//////////////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////////////
//                        INITIALISE LE MODULE I2C UTILIS�                              //
//////////////////////////////////////////////////////////////////////////////////////////
//----------------------------------------------------------------------------------------
// NumI2C : Num�ro du module I2C � utiliser (I2C0, I2C1 ou I2C2)
//----------------------------------------------------------------------------------------
extern void I2C_Initialize(int NumI2C);

//////////////////////////////////////////////////////////////////////////////////////////
//                              ENVOIE LE START-CONDITION                               //
//////////////////////////////////////////////////////////////////////////////////////////
//----------------------------------------------------------------------------------------
// Envoie le start-condition ou le repeated-start-condition
// Ceci d�marre le transfert
//----------------------------------------------------------------------------------------
extern void I2C_SendStart(void);

//////////////////////////////////////////////////////////////////////////////////////////
//                               ENVOIE LE STOP-CONDITION                               //
//////////////////////////////////////////////////////////////////////////////////////////
//----------------------------------------------------------------------------------------
// Envoie le stop-condition et attend la fin de l'op�ration
//----------------------------------------------------------------------------------------
extern void I2C_SendStop(void);

//////////////////////////////////////////////////////////////////////////////////////////
//                                  �CRIT UN OCTET DE DATA                              //
//////////////////////////////////////////////////////////////////////////////////////////
//----------------------------------------------------------------------------------------
// �crit un octet de data et attend la fin de l'op�ration
//----------------------------------------------------------------------------------------
extern Boolean I2C_Write(unsigned char Data);

//////////////////////////////////////////////////////////////////////////////////////////
//                OBTIENT LE STATUT DE LA DERNI�RE OP�RATION R�ALIS�E                   //
//////////////////////////////////////////////////////////////////////////////////////////
//----------------------------------------------------------------------------------------
// Obtient le statut actuel du transfert
//----------------------------------------------------------------------------------------
extern unsigned char I2C_GetStat(void);

//////////////////////////////////////////////////////////////////////////////////////////
//                             �CRIT L'OCTET DE CONTR�LE                                //
//////////////////////////////////////////////////////////////////////////////////////////
//----------------------------------------------------------------------------------------
// Address  : Adresse du p�riph�rique � contacter sur 7bits d�cal�s � gauche
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
