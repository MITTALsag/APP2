#ifndef COMMANDE_INTERPRETE_H
#define COMMANDE_INTERPRETE_H

#include "listes.h"
#include "curiosity.h"



void commande_operation(sequence_t* pile, char ope); // renvoie le resultat du calul de l'operation ope avec les deux prmiere cellule de la pile

/*---------------------------------------------------------------------------------------------------------------------------------------*/

void commande_P(sequence_t* pile); //fait la commande pose en prenant la tete de la pile et supprime ensuite la tete

void commande_M(sequence_t* pile); //fais la commande messure en prenant la tete de la pile et mat directement le resulatt dasn la tete de la pile

/*---------------------------------------------------------------------------------------------------------------------------------------*/

void commande_interrogation(sequence_t* pile, sequence_t* seq); //fais la commande '?' selon l'ennonce et "nettoie" tout les argument (les supprime)

/*---------------------------------------------------------------------------------------------------------------------------------------*/

void commande_X(sequence_t* pile); //fais la commande X en utilisant la fonction echanger_place

void commande_exclamation(sequence_t* pile, sequence_t* seq); 

/*---------------------------------------------------------------------------------------------------------------------------------------*/

void commande_C(sequence_t* pile);//copie la tete de la pile pour l'ajouter ensuite en tete de la pile

void commande_B(sequence_t* pile, sequence_t* seq); // fais la commande B comme dans l'ennoncé

void commande_R(sequence_t* pile);// fais la commande R comme dans l'ennoncé

void commande_I(sequence_t* pile); //supprime la tete de la pile

void commande_Z(sequence_t* pile); //inverse la pile 

#endif