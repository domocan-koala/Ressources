//////////////////////////////////////////////////////////////////////////////////////////
//                       LIBRAIRIE DE GESTION DE TEMPORISATIONS                         //
//////////////////////////////////////////////////////////////////////////////////////////
// Auteur               : Bigonoff                                                      //
// URL                  : www.bigonoff.org                                              //
//////////////////////////////////////////////////////////////////////////////////////////
// Historique                                                                           //
// ----------                                                                           //
// V1.00    30/03/2015  : Premi�re version en ligne                                     //
//////////////////////////////////////////////////////////////////////////////////////////
// Objet : Permet de g�n�rer des boucles de temporisation en utilisant le m�me timer    //
// pour toutes les m�thodes.                                                            //
// Si on n'initialise pas un timer particulier, le TIMER 0 sera utilis� par d�faut.     //
// Librairie d�velopp�e dans le cadre du projet Domocan, et donc sujette � modifications//
// en fonction des besoins futurs.                                                      //
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
#include "Timing.h"

//////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////
//                               VARIABLES GLOBALES                                     //
//////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////
LPC_TIM_TypeDef *_timer = NULL;                     // Pointeur sur timer

//////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////
//                               FONCTIONS PUBLIQUES                                    //
//////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////////////
//                             CONFIGURE LE TIMER UTILIS�                               //
//////////////////////////////////////////////////////////////////////////////////////////
//----------------------------------------------------------------------------------------
// S�lectionne le timer r�serv� aux temporisations, et le configure
// Si cette fonction n'est pas appel�e avant l'utilisation des temporisations,
// le TIMER0 sera configur� par d�faut
// numTimer: num�ro du timer, de TIMER0 � TIMER3
//----------------------------------------------------------------------------------------
void Timing_InitializeTimer(int numTimer)
{
    switch (numTimer)
    {
        case TIMER0:
            _timer = LPC_TIM0;                      // Pointer sur TIMER0
            LPC_SC->PCLKSEL0 |= PCLKSEL0_TIM0_4;    // Horloge timer0 = Clock/4 = 25Mhz
            LPC_SC->PCONP |= PCONP_PCTIM0;          // Timer0 sous tension (d�j� ok par d�faut)
            break;
            
        case TIMER1:
            _timer = LPC_TIM1;                      // Pointer sur TIMER1
            LPC_SC->PCLKSEL0 |= PCLKSEL0_TIM1_4;    // Horloge timer1 = 25Mhz
            LPC_SC->PCONP |= PCONP_PCTIM1;          // Timer1 sous tension (d�j� ok par d�faut)
            break;
            
        case TIMER2:
            _timer = LPC_TIM2;                      // Pointer sur TIMER2
            LPC_SC->PCLKSEL1 |= PCLKSEL1_TIM2_4;    // Horloge timer2 = 25Mhz
            LPC_SC->PCONP |= PCONP_PCTIM2;          // Timer2 sous tension
            break;
            
            
        default:
            _timer = LPC_TIM3;                      // Pointer sur TIMER3
            LPC_SC->PCLKSEL1 |= PCLKSEL1_TIM3_4;    // Horloge timer 3 = 25Mhz
            LPC_SC->PCONP |= PCONP_PCTIM3;          // Timer3 sous tension
    }
    _timer->TCR = TIMCTCR_TIMERMODE;                // Timer en mode timer
    _timer->PR = 25;                                // Une incr�mentation par �s
    _timer->MCR = TIMMCR_MR0I;                      // Flag d'interruption sur comparaison match0
}

//////////////////////////////////////////////////////////////////////////////////////////
//               OBTIENT LE NUM�RO DU TIMER UTILIS� POUR LES TEMPORISATIONS             //
//////////////////////////////////////////////////////////////////////////////////////////
//----------------------------------------------------------------------------------------
// Retourne le num�ro du timer utilis� pour les temporisations, ou
// -1 si aucun timer configur�
//----------------------------------------------------------------------------------------
int GetNumTimer(void)
{
    if (_timer == NULL)
        return -1;
    
    // On ne sait pas faire un calcul, l'espacement des adresses n'est pas constant
    if (_timer == LPC_TIM0)     
        return TIMER0;
    
    if (_timer == LPC_TIM1)
        return TIMER1;
        
    if (_timer == LPC_TIM2)
        return TIMER2;
    
    return TIMER3;
}

//////////////////////////////////////////////////////////////////////////////////////////
//                       INTRODUIT UN D�LAI EN MICROSECONDES                            //
//////////////////////////////////////////////////////////////////////////////////////////
//----------------------------------------------------------------------------------------
// Attend le nombre de �s pr�cis�
// time : nombre minimum de �s d'attente, de 0 � 0xFFFFFFFF
//----------------------------------------------------------------------------------------
void Timing_WaitUs (uint32_t time)
{
    if (time == 0)                                    // Pour temps tr�s courts
        return;
    
    if (_timer == NULL)                               // Si aucun timer configur�
        Timing_InitializeTimer(0);                    // Prendre timer 0 par d�faut

    _timer->TCR = TIMTCR_RESET;                       // Reset timer
    _timer->MR0 = time;                               // Temps d'attente en �s
    _timer->IR |= TIMIR_MR0;                          // Reset flag Match0
    _timer->TCR = TIMTCR_ENABLE;                      // Lancer timer
    while (!(_timer->IR & TIMIR_MR0));                // Attendre positionnement du flag
}

//////////////////////////////////////////////////////////////////////////////////////////
//                       INTRODUIT UN D�LAI EN MILLISECONDES                            //
//////////////////////////////////////////////////////////////////////////////////////////
//----------------------------------------------------------------------------------------
// Attend le nombre de ms pr�cis�
// time : nombre minimum de ms d'attente, de 0 � 400.000.000
//----------------------------------------------------------------------------------------
void Timing_WaitMs (uint32_t time)
{
    Timing_WaitUs(time * 1000);
}

