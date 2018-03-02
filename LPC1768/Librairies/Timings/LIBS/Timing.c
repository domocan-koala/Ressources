//////////////////////////////////////////////////////////////////////////////////////////
//                       LIBRAIRIE DE GESTION DE TEMPORISATIONS                         //
//////////////////////////////////////////////////////////////////////////////////////////
// Auteur               : Bigonoff                                                      //
// URL                  : www.bigonoff.org                                              //
//////////////////////////////////////////////////////////////////////////////////////////
// Historique                                                                           //
// ----------                                                                           //
// V1.00    30/03/2015  : Première version en ligne                                     //
//////////////////////////////////////////////////////////////////////////////////////////
// Objet : Permet de générer des boucles de temporisation en utilisant le même timer    //
// pour toutes les méthodes.                                                            //
// Si on n'initialise pas un timer particulier, le TIMER 0 sera utilisé par défaut.     //
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
//                             CONFIGURE LE TIMER UTILISÉ                               //
//////////////////////////////////////////////////////////////////////////////////////////
//----------------------------------------------------------------------------------------
// Sélectionne le timer réservé aux temporisations, et le configure
// Si cette fonction n'est pas appelée avant l'utilisation des temporisations,
// le TIMER0 sera configuré par défaut
// numTimer: numéro du timer, de TIMER0 à TIMER3
//----------------------------------------------------------------------------------------
void Timing_InitializeTimer(int numTimer)
{
    switch (numTimer)
    {
        case TIMER0:
            _timer = LPC_TIM0;                      // Pointer sur TIMER0
            LPC_SC->PCLKSEL0 |= PCLKSEL0_TIM0_4;    // Horloge timer0 = Clock/4 = 25Mhz
            LPC_SC->PCONP |= PCONP_PCTIM0;          // Timer0 sous tension (déjà ok par défaut)
            break;
            
        case TIMER1:
            _timer = LPC_TIM1;                      // Pointer sur TIMER1
            LPC_SC->PCLKSEL0 |= PCLKSEL0_TIM1_4;    // Horloge timer1 = 25Mhz
            LPC_SC->PCONP |= PCONP_PCTIM1;          // Timer1 sous tension (déjà ok par défaut)
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
    _timer->PR = 25;                                // Une incrémentation par µs
    _timer->MCR = TIMMCR_MR0I;                      // Flag d'interruption sur comparaison match0
}

//////////////////////////////////////////////////////////////////////////////////////////
//               OBTIENT LE NUMÉRO DU TIMER UTILISÉ POUR LES TEMPORISATIONS             //
//////////////////////////////////////////////////////////////////////////////////////////
//----------------------------------------------------------------------------------------
// Retourne le numéro du timer utilisé pour les temporisations, ou
// -1 si aucun timer configuré
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
//                       INTRODUIT UN DÉLAI EN MICROSECONDES                            //
//////////////////////////////////////////////////////////////////////////////////////////
//----------------------------------------------------------------------------------------
// Attend le nombre de µs précisé
// time : nombre minimum de µs d'attente, de 0 à 0xFFFFFFFF
//----------------------------------------------------------------------------------------
void Timing_WaitUs (uint32_t time)
{
    if (time == 0)                                    // Pour temps très courts
        return;
    
    if (_timer == NULL)                               // Si aucun timer configuré
        Timing_InitializeTimer(0);                    // Prendre timer 0 par défaut

    _timer->TCR = TIMTCR_RESET;                       // Reset timer
    _timer->MR0 = time;                               // Temps d'attente en µs
    _timer->IR |= TIMIR_MR0;                          // Reset flag Match0
    _timer->TCR = TIMTCR_ENABLE;                      // Lancer timer
    while (!(_timer->IR & TIMIR_MR0));                // Attendre positionnement du flag
}

//////////////////////////////////////////////////////////////////////////////////////////
//                       INTRODUIT UN DÉLAI EN MILLISECONDES                            //
//////////////////////////////////////////////////////////////////////////////////////////
//----------------------------------------------------------------------------------------
// Attend le nombre de ms précisé
// time : nombre minimum de ms d'attente, de 0 à 400.000.000
//----------------------------------------------------------------------------------------
void Timing_WaitMs (uint32_t time)
{
    Timing_WaitUs(time * 1000);
}

