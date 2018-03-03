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

//////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////
//                                 DEFINES                                              //
//////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////

// Type de mémoire prise en charge
// -------------------------------
// Cette information permet de traiter différement la mémoire 24LC02.
// Le test de débordement de page de la méthode WriteBytesArray est adapté en fonction de la valeur du define
#define     EEP_02                  8                   
#define     EEP_256                 64
#define     EEP_512                 128
#define     EEP_1025                128

// Mode disponible pour le contrôle
// --------------------------------
#define     EEP_WRITE               0
#define     EEP_READ                1

//////////////////////////////////////////////////////////////////////////////////////////
//                                  PROTOTYPES                                          //
//////////////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////////////
//                                  SELECTION DE L'EEPROM                               //
//////////////////////////////////////////////////////////////////////////////////////////
//----------------------------------------------------------------------------------------
// EepromAddress    : Adresse de la mémoire EEPROM sur 7bits décalé d'un rang sur la gauche
// EepromCapability : Capacité de la mémoire, la valeur est en fait le PageWriteLength
//----------------------------------------------------------------------------------------
extern void Eeprom_SelectChip(uint8_t EepromAddress, uint8_t EepromCapability);

//////////////////////////////////////////////////////////////////////////////////////////
//                                      LECTURE D'UN OCTET                              //
//////////////////////////////////////////////////////////////////////////////////////////
//----------------------------------------------------------------------------------------
// MemoryAddress    : Adresse de l'octet à lire 
// Retourne         : La valeur de l'octet lu
//----------------------------------------------------------------------------------------
extern uint8_t Eeprom_ReadByte(uint16_t MemoryAddress);

//////////////////////////////////////////////////////////////////////////////////////////
//                                       ECRIRE UN OCTET                                //
//////////////////////////////////////////////////////////////////////////////////////////
//----------------------------------------------------------------------------------------
// MemoryAddress    : Adresse de l'octet à écrire 
// Data             : Valeur de l'octet à écrire
// Retourne         : true ou false
//----------------------------------------------------------------------------------------
extern Boolean Eeprom_WriteByte(uint16_t MemoryAddress, uint8_t Data);

//////////////////////////////////////////////////////////////////////////////////////////
//                                       LIRE PLUSIEURS OCTETS                          //
//////////////////////////////////////////////////////////////////////////////////////////
//----------------------------------------------------------------------------------------
// MemoryAddress    : Adresse du premier octet à lire 
// Array            : Tableau qui va contenir les valeurs lues
// ArraySize        : Taille du tableau
// Retourne         : true ou false
//----------------------------------------------------------------------------------------
extern Boolean Eeprom_ReadBytesArray(uint16_t MemoryAddress, uint8_t *Array, uint8_t ArraySize);

//////////////////////////////////////////////////////////////////////////////////////////
//                                    ECRIRE PLUSIEURS OCTETS                           //
//////////////////////////////////////////////////////////////////////////////////////////
//----------------------------------------------------------------------------------------
// Attention à ne pas déborder de la page
//----------------------------------------------------------------------------------------
// MemoryAddress    : Adresse du premier octet à lire 
// Array            : Tableau qui contient les valeurs à écrire
// ArraySize        : Taille du tableau
// Retourne         : true ou false
//----------------------------------------------------------------------------------------
extern Boolean Eeprom_WriteBytesArray(uint16_t MemoryAddress, uint8_t *Array, uint8_t ArraySize);
