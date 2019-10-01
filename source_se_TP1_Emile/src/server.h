/*
    SIF1015 - François Meunier A18
    Travail de session
    Dépanneur JONATHAN BOISCLAIR SEPTEMBRE 2018

    Nom des étudiants: Emile SEBASTIANUTTI, Elizabeth COURTIEU, Marianne PHILIPPON, Tom RAMIN
    Code permanent des étudiants:SEBE03079800, COUE05549100, PHIM12579800, RAMT22039800
*/
#include "base.h"

#ifndef SERVER_H
#define SERVER_H

struct noeud{	
    noeud* parent;
    noeud* enfant;
    noeud* gauche; //param inutile
    noeud* droit;
    time_t edit;
    int nbVisiteur;	
    int flag;
    int noProprietaire;
    char name[200];
    bool aSupprimer;
	sem_t sem;
	sem_t sem_nv;
};
struct server;
struct message{	
	char typeTransaction;
    char nomFichier[200],param[200];
};
struct liste{
	message info;
	liste* suiv;
};
struct client{	
    int noClient;	
    noeud* pwd;	
	server* srv;
	pthread_t tid;
	liste* debutListe;
	liste* finListe;
    sem_t sem_file;
    sem_t sem_element_ready;
};
struct server{	
    noeud* root;
    std::vector<client*> clients;
};


void entrerNoeud(noeud* n);		
void quitterNoeud(noeud* n);	
	
noeud* deplacerHorizontal(noeud * origin);

void ajouterClient(server* srv,client* c);	
void ajouterFichier(server* srv, char* path,client* c);
void supprimerFichier(server* srv, char* path,client* c);
void modifierFichier(server* srv, char* path,client* c);
void modifierFlagFichier(server* srv,char* path, client* c, int flag);
void renommer(server* srv, char* path, char* name,client* c);

void afficherArborescence(server* srv,bool all);
void enumererNoeud(noeud* n, int depth,bool r,bool all);
client* getClient(server* srv, int no, message m);
server* creerServer();
noeud * findNode(noeud* n,char* path);
void quitterArborescence(noeud* n,noeud* racine,bool relative);
bool isChildOf(noeud* n,noeud* of);
void printPath(client* c);
void supprimerChilds(noeud* n);
#endif