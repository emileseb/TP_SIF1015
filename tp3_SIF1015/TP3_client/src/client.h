/*
    SIF1015 - François Meunier A18
    Travail de session
    Création de JONATHAN BOISCLAIR SEPTEMBRE 2018

    Nom de l'étudiant:
    Code permanent de l'étudiant:
*/
#include "base.h"

#ifndef SERVER_H
#define SERVER_H

struct message{//Structure pour transfer d'information(interthread, interprocessus)
    char action;
    char filename[200];
    char param[200];
	int cid;
};

#endif