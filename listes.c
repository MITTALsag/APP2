#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <assert.h>
#include <string.h>
#ifdef NCURSES
#include <ncurses.h>
#endif
#include "listes.h"


/*
 *  Auteur(s) :
 *  Date :
 *  Suivi des Modifications :
 *
 */

bool silent_mode = true;
bool silent_avant_apres = true; //sert a montrer au debut et a la fin de l'execution


cellule_t* nouvelleCellule (void)
{
    cellule_t* nouvelle_cellule = (cellule_t*)(malloc (sizeof(cellule_t)));
    if (!nouvelle_cellule){
        fprintf(stderr, "Errueur : probleme allocation cellule dnas nouvelleCellule.\n");
        exit(EXIT_FAILURE);
    }
    nouvelle_cellule->type = 0;
    nouvelle_cellule->buffer.command = 'L'; //valeur impossible dans le jeu
    nouvelle_cellule->suivant = NULL;
    return nouvelle_cellule;
}

bool verif_type(int type, cellule_t* cel){
    return cel->type == type;
}

//detruit une cellule est renvoie NULL
cellule_t* detruireCellule (cellule_t* cel)
{
    free(cel);
    return NULL;
}


//fonction recursive qui detruit aussi les groupes de commandes
cellule_t* detruireSeq(cellule_t* cel){
    if (!cel)
        return NULL;
    
    if (cel->suivant)
        cel->suivant = detruireSeq(cel->suivant);
    
    if (cel->type == 2)
        cel->buffer.groupe_command = detruireSeq(cel->buffer.groupe_command);

    detruireCellule(cel);
    
    return NULL;
        
}

/*---------------------------------------------------------------------------------------------------------------------------------------*/


int mettre_dans_groupe_cmd(cellule_t* cel, char* txt, int len_txt, int indice, int* compteur, cellule_t** avant_queue){//sert dans conversion a mettre un groupe de commande dans une cellule

    if (*compteur >= 200000){
        *avant_queue = cel;
        return indice - 1;
    }

    //si le groupe est vide
    if (txt[indice] == '}'){
        cel->buffer.groupe_command = NULL;
        return indice;
    }

    bool first_iteration = true; //un bool qui sert a savoir si on est dans la prmiere iteration de la boucle

    cellule_t* queue_groupe; //pointe vers la queue du groupe sert a rajouter des element a la fin du groupe de commande

    while (indice < len_txt && txt[indice] != '}'){

        if (txt[indice] != '\n' && txt[indice] != '\0' && txt[indice] != ' '){
            cellule_t* groupe = nouvelleCellule();

            if (first_iteration) {
                queue_groupe = groupe;    //pour rajouter les elements dans le groupe a la fin
                cel->buffer.groupe_command = groupe;  //debut du groupe 
                first_iteration = false; // on viens de faire la premiere iteration
            }
            else {
                queue_groupe->suivant = groupe; //pour connecetr les cellule du meme groupe de commande
                queue_groupe = groupe; //pour continuer a pointer sur la queue du groupe
            }

            if (txt[indice] == '{'){ //appelle recursif 
                *compteur = *compteur + 1;
                groupe->type = 2;
                indice = mettre_dans_groupe_cmd(groupe, txt, len_txt, indice + 1, compteur, avant_queue);
                if (*compteur >= 200000){
                    return indice;
                }
            }
            else if ( '0' <= txt[indice] && txt[indice] <= '9'){//Alors on change le type de la cellule en 1 (chiffre et on met le chiffre dans la cellule)
                groupe->type = 1;
                groupe->buffer.chiffre =  txt[indice] - '0';
            }
            else { //ajout d'un elelemnt du type 0 a la fin de la queue
                groupe->buffer.command =  txt[indice];
            }
        }

        indice++;
    }


    return indice;
    
}

//converti un texte en sequence (liste chaine)
void conversion (char *texte, sequence_t *seq){

    //creation de la tete de la sequence
    seq->tete = NULL;
    
    int len = strlen(texte); 
    cellule_t* queue_seq;

    for (int i = 0 ; i < len ; i++){

        if (texte[i]!= '\n' && texte[i] != '\0' && texte[i] != ' '){ //on ne fais rien si texte[i] == \n ou \0
            cellule_t* nouv_cel = nouvelleCellule();
        

            if (!seq->tete){ //si seq->tete == NULL (sert que la premiere itération)
                seq->tete = nouv_cel;
                queue_seq = nouv_cel; //queue de la sequence sert a rajouter les prochaine cellule
            }
            else{ //pour toutes les itération saudf la premiere ( seq->tete != NULL)
                queue_seq->suivant = nouv_cel;
                queue_seq = nouv_cel;
            }

            if ( '0' <= texte[i] && texte[i] <= '9') {//Alors on change le type de la cellule en 1 (chiffre et on met le chiffre dans la cellule)
                nouv_cel->type = 1;
                nouv_cel->buffer.chiffre = texte[i] - '0';
            }

            else if (texte[i] == '{'){ //si on est dans un groupe de commande ---> Alors on change le type en 2 et on met le groupe dans la cellule
                cellule_t* queu_groupe = NULL;
                int cmp = 1;
                int* compteur = &cmp;
                nouv_cel->type = 2;
                i = mettre_dans_groupe_cmd(nouv_cel, texte, len, i + 1, compteur, &queu_groupe);
                while (queu_groupe && i < len){
                    *compteur = 0;
                    cellule_t* tmp = NULL;
                    i = mettre_dans_groupe_cmd(queu_groupe, texte, len, i+1, compteur, &tmp);
                    queu_groupe = tmp;
                }

            }

            else { //si c'est une commande executable
                
                nouv_cel->buffer.command = texte[i];
                
            }
        
        }
    }
}

/*---------------------------------------------------------------------------------------------------------------------------------------*/

void afficher_rec (sequence_t* seq)
{
    assert (seq); /* Le pointeur doit être valide */
    cellule_t* cel = seq->tete;

    if (!cel){
        return;
    }

    while (cel){

        if (cel->type == 1){ //si c'est un chiffre on affiche un %d
            printf("%d", cel->buffer.chiffre);
        }
        else if (cel->type == 0){   //si c'est un char on affiche un %c
            printf("%c", cel->buffer.command);
        }
        else { //appelle recursif pour les groupe
            printf("{");
            sequence_t tmp;
            tmp.tete = cel->buffer.groupe_command;
            afficher_rec(&tmp);
            printf("}");
        }

        cel = cel->suivant;
    }
}



void afficher (sequence_t* seq){ //Cette fonction sert juste pour fare le printf 
    afficher_rec(seq);
    printf("\n");
}


/*-----------------------------------------------------------------------------------------------------------------------------------*/

//supprime la tete de la sequence, la nouvelle tete est le deuxieme element de la sequance avant la fonction 
void supprime_tete(sequence_t* seq){
    if (seq->tete->type == 2){
        seq->tete->buffer.groupe_command = detruireSeq(seq->tete->buffer.groupe_command);
    }
    cellule_t* tmp = seq->tete;
    seq->tete = seq->tete->suivant;
    detruireCellule(tmp);
}


void ajouter_tete(sequence_t* seq, cellule_t* cel){

    cel->suivant = seq->tete;
    seq->tete = cel;

}


void ajout_pile(sequence_t* pile,sequence_t* seq){
    cellule_t* cel_courante = seq->tete;
    seq->tete = seq->tete->suivant;
    cel_courante->suivant = pile->tete;
    pile->tete = cel_courante;
}

/*---------------------------------------------------------------------------------------------------------------------------------------*/

cellule_t* trouver_queue_seq(sequence_t* seq){

    cellule_t* cel = seq->tete;

    if(!cel){ //si seq->tete == NULL
        return NULL;
    }

    while (cel->suivant){
        cel = cel->suivant;
    }
    return cel;
}


cellule_t* cell_pos_i(sequence_t* seq, int pos){
    cellule_t* cel = seq->tete;
    
    for(int i = 0 ; i < pos ; i++){
        cel = cel->suivant;
        if (!cel){
            printf("Erreur (fonction cell_pos_i): La sequence est trop petite pour acceder au %d eme elemnet.\nElle a une taille de %d", pos, i+1),
            exit(EXIT_FAILURE);
        }
    }
    return cel;
}

/*-----------------------------------------------------------------------------------------------------------------------------------*/

void exec_groupe(sequence_t* pile, sequence_t* seq){

    cellule_t* cel_tete_groupe = pile->tete->buffer.groupe_command; //pointe vers la premiere cellule du groupe de commande

    if (!cel_tete_groupe){ //c'est a dire que le groupe est vide donc pas besoin de le mettre dans seq
        supprime_tete(pile);
        return;
    }

    //Pour trouver l'adresse de la queue du groupe
    sequence_t seq_tmp;
    seq_tmp.tete = cel_tete_groupe;
    cellule_t* queue = trouver_queue_seq(&seq_tmp);

    queue->suivant = seq->tete;
    seq->tete = cel_tete_groupe;

    cellule_t* tmp = pile->tete;
    pile->tete = pile->tete->suivant;
    detruireCellule(tmp);

}

/*-----------------------------------------------------------------------------------------------------------------------------------*/

cellule_t* clone_chiffre(cellule_t* cel){

        if (!verif_type(1, cel)){
        printf("Erreur : fonction clone_chiffre ----> Le type de la cellule passé en argument est %d alors qu'il doit etre du type 1\n", cel->type);
        exit(EXIT_FAILURE);  
    }
    cellule_t* clone = nouvelleCellule();
    clone->type = 1;
    clone->buffer.chiffre = cel->buffer.chiffre;
    return clone;
}

cellule_t* clone_command(cellule_t* cel){

    if (!verif_type(0, cel)){
        printf("Erreur : fonction clone_command ----> Le type de la cellule passé en argument est %d alors qu'il doit etre du type 0\n", cel->type);
        exit(EXIT_FAILURE);  
    }
    cellule_t* clone = nouvelleCellule();
    clone->buffer.command = cel->buffer.command;
    return clone;
}

cellule_t* clone_groupe(cellule_t* groupe){

    if (!verif_type(2, groupe)){ //si pas de groupe donc erreur
        printf("Erreur : fonction clone_groupe ----> Le type de la cellule passé en argument est %d alors qu'il doit etre du type 2\n", groupe->type);
        exit(EXIT_FAILURE);
    }

    cellule_t* nouv_groupe = nouvelleCellule();
    nouv_groupe->type = 2;
    nouv_groupe->buffer.groupe_command = NULL;
    cellule_t* queu_groupe = NULL;
    cellule_t* pointeur_groupe = groupe->buffer.groupe_command;

    while (pointeur_groupe){
        cellule_t* nouv_cel;

        if(verif_type(2, pointeur_groupe)){
            nouv_cel = clone_groupe(pointeur_groupe);
        }
        else if (verif_type(1, pointeur_groupe)){
            nouv_cel = clone_chiffre(pointeur_groupe);
        }
        else{
            nouv_cel = clone_command(pointeur_groupe);
        }

        if (!nouv_groupe->buffer.groupe_command){ //sert a la premier iteration pour mettre la premier cellule du groupe de commande dans le champ groupe_command
            nouv_groupe->buffer.groupe_command = nouv_cel;
        }
        else{ //pour tout les autre iteration pour mettre nouv_cel a la suite du groupe
            queu_groupe->suivant = nouv_cel;
        }
        queu_groupe = nouv_cel;
        pointeur_groupe = pointeur_groupe->suivant;
    }
    return nouv_groupe;

}

cellule_t* clone_cellule(cellule_t* cel){
    int type = cel->type;
    
    switch(type){
        case 0 :
            return clone_command(cel);
            break;
        case 1 : 
            return clone_chiffre(cel);
            break;
        case 2 : 
            return clone_groupe(cel);
            break;
        default :
            printf("Type de cellule inconnu : %d\n", type);
            exit(EXIT_FAILURE);
            break;
    }

    return NULL; //impossible normalement d'arriver ici

}

/*-----------------------------------------------------------------------------------------------------------------------------------*/

void echanger_place(sequence_t* seq, cellule_t* premiere, cellule_t* deuxieme){
    
    if (premiere != seq->tete || deuxieme != seq->tete->suivant){
        printf("Erreur : fonction echanger_place ----> Les arguments ne sont pas bon (iol faux donner (sequance, tete de la sequence, tete->suivant de la sequence)\n");
        exit(EXIT_FAILURE);  
    }

    seq->tete = deuxieme;
    premiere->suivant = deuxieme->suivant;
    deuxieme->suivant = premiere;

}

cellule_t* inversee_seq(sequence_t* seq){
    cellule_t* cel_courante = seq->tete, *cel_pere_courante = NULL, *cel_fils_courante = NULL;

    while(cel_courante){
        //inverse pas a pas la sequence
        cel_fils_courante = cel_courante->suivant;
        cel_courante->suivant = cel_pere_courante;
        cel_pere_courante = cel_courante;
        cel_courante = cel_fils_courante;

    }
    return cel_pere_courante;
}