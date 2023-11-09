#include <stdio.h>
#include <stdbool.h>
#include <assert.h>
#include <stdlib.h>
#ifdef NCURSES
#include <ncurses.h>
#endif
#include "listes.h"
#include "curiosity.h"
#include "commande_interprete.h"



/*
 *  Auteur(s) :
 *  Date :
 *  Suivi des Modifications :
 *
 */

void stop (void)
{
    char enter = '\0';
    printf ("Appuyer sur entrée pour continuer...\n");
    while (enter != '\r' && enter != '\n') { enter = getchar(); }
}



int interprete (sequence_t* seq, bool debug)
{
    // Version temporaire a remplacer par une lecture des commandes dans la
    // liste chainee et leur interpretation.

    char commande;

    if (!silent_avant_apres) {  //pour execution "pas a pas" ----> mettre silent_mode sur false pour affichage (modifier aussi le makefile) (silent_mode se trouve dans le fichier liste.c)
        bool debug = true;  
        printf ("Programme:");
        afficher(seq);
        printf ("\n");
        if (debug) stop();
    }


    cellule_t* cel_courante = seq->tete;

    int ret;         //utilisée pour les valeurs de retour

    //pile 
    sequence_t pile;
    pile.tete = NULL;

    while ( cel_courante ) { 

        int type = cel_courante->type;
        if (!silent_mode){printf("TYPE : %d\n", type);}

        switch (type) {

            case 0 : 

                commande = cel_courante->buffer.command;
                if (!silent_mode){printf("COMMAND : %c\n", commande);}

                switch (commande){
                    case 'A':
                        ret = avance();
                        supprime_tete(seq);
                        if (ret == VICTOIRE){ 

                            if (!silent_avant_apres){
                                bool debug = true;  
                                afficherCarte();
                                printf("PILE APRES EXEC : ");
                                afficher(&pile);
                                printf ("\n");
                                printf("SEQ APRES EXEC : ");
                                afficher(seq);
                                printf ("\n");
                                if (debug) stop();  
                            }

                            // ne jamais oublier de dettruire avnt la fin du programme
                            pile.tete = detruireSeq(pile.tete);
                            seq->tete = detruireSeq(seq->tete);
                            return VICTOIRE; /* on a atteint la cible */
                        }
                        if (ret == RATE){
                            pile.tete = detruireSeq(pile.tete);
                            seq->tete = detruireSeq(seq->tete);
                            afficherCarte();
                            return RATE;     /* tombé dans l'eau ou sur un rocher */
                        }
                        break; /* à ne jamais oublier !!! */
                    case 'D':
                        droite();
                        supprime_tete(seq);
                        break;
                    case 'G':
                        gauche();
                        supprime_tete(seq);
                        break;
                    case '+':   
                        commande_operation(&pile, '+');
                        supprime_tete(seq);                
                        break;
                    case '-':
                        commande_operation(&pile, '-');
                        supprime_tete(seq);
                        break;
                    case '*':
                        commande_operation(&pile, '*');
                        supprime_tete(seq);
                        break;
                    case 'P':
                        commande_P(&pile);
                        supprime_tete(seq);
                        break;
                    case 'M': 
                        commande_M(&pile);
                        supprime_tete(seq);
                        break;
                    case '?' :
                        supprime_tete(seq);
                        commande_interrogation(&pile, seq);
                        break;
                    case 'X' :
                        supprime_tete(seq);
                        commande_X(&pile);
                        break;
                    case '!' :
                        supprime_tete(seq);
                        commande_exclamation(&pile, seq);
                        break;
                    case 'C':
                        supprime_tete(seq);
                        commande_C(&pile);
                        break;
                    case 'B':
                        commande_B(&pile, seq);
                        break;
                    case 'R':
                        supprime_tete(seq);
                        commande_R(&pile);
                        break;
                    case 'I':
                        supprime_tete(seq);
                        commande_I(&pile);
                        break;
                    case 'Z':
                        supprime_tete(seq);
                        commande_Z(&pile);
                        break;
                    default:
                        if (!silent_mode) { eprintf("Caractère inconnu: '%c'\n", commande);}
                        supprime_tete(seq);
                        break;

                
                }
                break;
            case 1 :
                if (!silent_mode){printf("CHIFFRE : %d\n", cel_courante->buffer.chiffre);}
            case 2 : 
                ajout_pile(&pile, seq);
                break; 
            
            default:
                if (!silent_mode) { eprintf("Caractère inconnu: '%c'\n", commande);}
                supprime_tete(seq);
                break;
        }

        cel_courante = seq->tete;

        /* Affichage pour faciliter le debug*/
        if (!silent_mode) {
            bool debug = true;
            afficherCarte();
            printf("Commaande :%c\n", commande);
            printf ("Programme:");
            afficher(seq);
            printf ("\n");
            //printf("type : %d\n", cel_courante->type);
            printf("Pile:");
            afficher(&pile);
            printf("\n");
            if (debug) stop();
        }
    }

    //detruire pour eviter les fuite memoires
    pile.tete = detruireSeq(pile.tete);
    seq->tete = detruireSeq(seq->tete);
    /* Si on sort de la boucle sans arriver sur la cible,
     * c'est raté :-( */

    return CIBLERATEE;
}
