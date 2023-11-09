#include <stdio.h>
#include <stdbool.h>
#include <assert.h>
#include <stdlib.h>
#ifdef NCURSES
#include <ncurses.h>
#endif
#include "commande_interprete.h"



/*ACTE II*/
/*------------------------------------------------------------------------------------------------------------------------------------------------------*/
void commande_operation(sequence_t* pile, char ope){

    if (!verif_type(1, pile->tete) || !verif_type(1, pile->tete->suivant)){
        printf("Erreur : fonction operation ----> Les opérateurs ne sont pas de chiffres.\n");
        exit(EXIT_FAILURE);
    }

    //recuperation des operande de gauche et de droite
    int ope_droite = pile->tete->buffer.chiffre;
    supprime_tete(pile);

    int ope_gauche = pile->tete->buffer.chiffre;
    supprime_tete(pile);

    int resultat;

    if(ope == '+')
        resultat = ope_gauche + ope_droite;
    else if (ope == '-'){
        resultat = ope_gauche - ope_droite;
    }
        /*
        if (ope_droite < ope_gauche)
            resultat = ope_gauche - ope_droite;
        else
            resultat = ope_droite - ope_gauche;
        }
        */
    else 
        resultat = ope_gauche * ope_droite;

    cellule_t* cel = nouvelleCellule();
    cel->type = 1;
    cel->buffer.chiffre = resultat;
    ajouter_tete(pile, cel);

}

/*ACTE III*/
/*------------------------------------------------------------------------------------------------------------------------------------------------------*/
void commande_P(sequence_t* pile){

    if(!verif_type(1, pile->tete)){
        printf("Erreur : fonction commande_P ----> La commande pour la pose n'est pas un chiffre.\n");
        exit(EXIT_FAILURE);
    }

    pose(pile->tete->buffer.chiffre);
    supprime_tete(pile);
}

void commande_M(sequence_t* pile){

    if(!verif_type(1, pile->tete)){
        printf("Erreur : fonction commande_M ----> La commande pour la mesure n'est pas un chiffre.\n");
        exit(EXIT_FAILURE);
    }

    int resultat = mesure(pile->tete->buffer.chiffre);

    pile->tete->buffer.chiffre = resultat; //on n'a plus besoin de l'ancienne valeur donc on peut la remplacer

}

/*ACTE IV*/
/*------------------------------------------------------------------------------------------------------------------------------------------------------*/

void commande_interrogation(sequence_t* pile, sequence_t* seq){


    if(!pile->tete->suivant->suivant || !verif_type(1, pile->tete->suivant->suivant)){
        printf("Erreur : fonction commande_interrogation ----> La pile na pas assez de cellule (il y a moins que 3 cellule)\nOr le nombre de cellule minimum pour cette commande est 3 !!\nOU :\nLa commande pour la mesure n'est pas un chiffre.\n");
        exit(EXIT_FAILURE);
    }

    int val = pile->tete->suivant->suivant->buffer.chiffre;

    if (val != 0 ){ //si nb != 0 ---> on supprime le premier groupe de commande puis on execute le groupe numeros 2
        supprime_tete(pile);
        exec_groupe(pile, seq);
    }
    else{ //sinon on execute le groupe de commande numeros 1 et on supprime le deuxième
        exec_groupe(pile, seq);
        supprime_tete(pile);
    }
    
    //supprime le nombre de la pile (plus besoin)
    supprime_tete(pile);
}

/*ACTE V*/
/*------------------------------------------------------------------------------------------------------------------------------------------------------*/

void commande_X(sequence_t* pile){
    echanger_place(pile, pile->tete, pile->tete->suivant);
}

void commande_exclamation(sequence_t* pile, sequence_t* seq){
    exec_groupe(pile, seq);
}

/*------------------------------------------------------------------------------------------------------------------------------------------------------*/

void commande_C(sequence_t* pile){
    
    cellule_t* a_copier = pile->tete;
    cellule_t* copie = clone_cellule(a_copier);

    ajouter_tete(pile, copie);
}


void commande_B(sequence_t* pile, sequence_t* seq){

    if(!verif_type(1, pile->tete)){
        printf("Erreur : fonction commande_B ----> La tete de la pile n'est pas du type 1 n'est pas un chiffre.\n");
        exit(EXIT_FAILURE);
    }

    int n = pile->tete->buffer.chiffre;

    if (n != 0){ 
        //on utilise des sous seq pour pouvoir utiliser la fonction exec_groupe
        sequence_t sous_seq;
        
        //donc sous_seq est la sequence ou il n'y a que le groupe a copier
        cellule_t* copie = clone_groupe(pile->tete->suivant);
        sous_seq.tete = copie;

        exec_groupe(&sous_seq, seq);
        
        pile->tete->buffer.chiffre = (n-1);
    }
    else{
        // on supprime la commande B, le chiffre (= 0) et le groupe de commande
        supprime_tete(seq);
        supprime_tete(pile);
        supprime_tete(pile);
    }
}


void commande_R(sequence_t* pile){
    
    if(!verif_type(1, pile->tete) || !verif_type(1, pile->tete->suivant)){
        printf("Erreur : fonction commande_R ----> Au moin une des deux premiere cellules n'est pas un chiffre\n");
        exit(EXIT_FAILURE);
    }

    int n = pile->tete->buffer.chiffre;
    supprime_tete(pile);

    int nb_cel = pile->tete->buffer.chiffre;
    supprime_tete(pile);

    while (n > 0){
        cellule_t* cel_dernier = cell_pos_i(pile, nb_cel-1);
        cellule_t* cel_avant_dernier = cell_pos_i(pile, nb_cel-2);

        //on met cel_dernier au debut de la pile
        cel_avant_dernier->suivant = cel_dernier->suivant;
        cel_dernier->suivant = pile->tete;
        pile->tete = cel_dernier;

        n--;
    }
}



void commande_I(sequence_t* pile){
    supprime_tete(pile);
}


void commande_Z(sequence_t* pile){
    pile->tete = inversee_seq(pile);
}