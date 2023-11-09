#ifndef LISTES_H
#define LISTES_H

#include <stdbool.h>

/*
 * Pour réaliser des tests de performance, désactiver tous les 
 * affichages.
 * Pour cela, le plus simple est de redefinir les fonctions principales 
 * en decommentant les 3 lignes suivantes et en commentant l'ancienne 
 * definition de 'eprintf' juste dessous.
 */

#ifdef SILENT

#define printf(fmt, ...) (0)
#define eprintf(fmt, ...) (0)
#define putchar(c) (0)

#else

#define eprintf(...) fprintf (stderr, __VA_ARGS__)

#endif

extern bool silent_mode;




struct cellule {
    int type;   //pour savoir de quel type est dans le union par defaut 0 (0 pour commande, 1 pour chiffre et 2 pour groupe commande)

    union {
        char command;   //utiliser pour les commandes executables
        int chiffre;    //utilier uniquement pour les chiffre (pour les calcul et les commandes qui se servent de chiffre)
        struct cellule* groupe_command;  //utiliser uniquement dans la pile et uniquement pour les groupe de commandes 
    } buffer;
    
    struct cellule *suivant;
};
typedef struct cellule cellule_t;

struct sequence {
    cellule_t *tete;
};
typedef struct sequence sequence_t;


/*---------------------------------------------------------------------------------------------------------------------------------------*/

cellule_t* nouvelleCellule (void); //cree une nouvelle cellule de type 0

bool verif_type(int type, cellule_t* cel); //verifie le type de la cellule renvoie true si c'est le bon type false sinon

cellule_t* detruireCellule (cellule_t* cel); 

cellule_t* detruireSeq(cellule_t* cel); //detruit une sequence meme si il y un groupe de commande

/*---------------------------------------------------------------------------------------------------------------------------------------*/

int mettre_dans_groupe_cmd(cellule_t* cel, char* txt, int len_txt, int indice);//sert dans conversion a mettre un groupe de commande dans une cellule

void conversion (char *texte, sequence_t *seq);

/*---------------------------------------------------------------------------------------------------------------------------------------*/

void afficher_rec (sequence_t* seq); // sert dans afficher

void afficher (sequence_t* seq);

/*---------------------------------------------------------------------------------------------------------------------------------------*/

void supprime_tete(sequence_t* seq); //supprime la tete de seq (meme si il y a un groupe)

void ajouter_tete(sequence_t* seq, cellule_t* cel); //ajoute cel au debut de seq

void ajout_pile(sequence_t* pile,sequence_t* seq);//ajoute l'element au debut de seq au debut de la pile (evite de detruire puis recree)

/*---------------------------------------------------------------------------------------------------------------------------------------*/

cellule_t* trouver_queue_seq(sequence_t* seq); //renvoie l'adresse de la queue d'une sequance et NULL si la sequence est vide

cellule_t* cell_pos_i(sequence_t* seq, int pos); //renvoie l'adresse de la cellule a la position pos (la premiere cellule est a la position 0)

/*---------------------------------------------------------------------------------------------------------------------------------------*/

void exec_groupe(sequence_t* pile, sequence_t* seq); //prend le groupe en tete de la pile et le met sous forme "executable" au debut de la sequence 

/*---------------------------------------------------------------------------------------------------------------------------------------*/

cellule_t* clone_chiffre(cellule_t* cel); //revoie l'adresse d'une nouvelle cellule qui est du type 1 est qui contien le meme chiffre que cel (sert dans clone_cellule)

cellule_t* clone_command(cellule_t* cel); //revoie l'adresse d'une nouvelle cellule qui est du type 0 est qui contien la meme command que cel (sert dans clone_cellule)

cellule_t* clone_groupe(cellule_t* groupe); //revoie l'adresse d'une nouvelle cellule qui est du type 2 est qui contien le meme groupe que groupe (sert dans clone_cellule)

cellule_t* clone_cellule(cellule_t* cel); //revoie l'adresse d'une nouvelle cellule qui est du type 0, 1, 2 est qui contien la meme valeur que cel (se sert des 3 fonction precedente)

/*---------------------------------------------------------------------------------------------------------------------------------------*/

void echanger_place(sequence_t* seq, cellule_t* premiere, cellule_t* deuxieme); //echange les deux prmiere cellule d'une sequence de place

cellule_t* inversee_seq(sequence_t* seq); //inverse la sequence est revoie la cellule au debut de la nouvelle sequence

#endif
