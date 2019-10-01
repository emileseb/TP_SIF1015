#include "base.h"

#ifndef SERVER_H
#define SERVER_H

struct message{//Structure pour transfer d'information(interthread, interprocessus)
    char action;
    char filename[200];
    char param[200];
	int cid;
};
struct m_node{//Structure pour une file sous forme de liste chainée de message
    message value;
    m_node* next;
};
struct noeud{//On fichier ou un dossier dans l'arbre
    noeud* parent;
    noeud* enfant;
    noeud* gauche;
    noeud* droit;
    time_t edit;//Dernière modification
    int nbVisiteur;//Nombre de visiteur actuel sur le noeud
    int flag;//Permission (777 = tout le monde peut tout faire)
    int noProprietaire;//Numéro du client propriétaire
    char name[200];//Nom du fichier
    bool aSupprimer;//Met le fichier en état de suppression(LAZY)
    sem_t sem;//Semaphore de sortie
    sem_t sem_nv;//Semaphore pour le nombre de visiteur
};
struct server;//Un prototype pour permette qu'un serveur aie accès aux clients et clients accès au serveur(cercle)
struct client{
    int noClient;
    noeud* pwd;
    m_node* first;
    m_node* last;
    int sockfd;
    server* serv;
    pthread_t thread;//Une référence au thread du client si on dois faire pthread_abort ou pthread_join
};
struct server{//Le serveur, racine et liste de client
    noeud* root;
    std::vector<client*> clients;
};

void entrerNoeud(noeud* n);
void quitterNoeud(noeud* n);

void ajouterClient(server* srv,client* c);
void ajouterFichier(server* srv, char* path,client* c);
void supprimerFichier(server* srv, char* path,client* c);
void modifierFichier(server* srv, char* path,client* c);
void modifierFlagFichier(server* srv,char* path, client* c, int flag);
void renommer(server* srv, char* path, char* name,client* c);

void afficherArborescence(server* srv,bool all, client* c);
void enumererNoeud(noeud* n, int depth,bool r,bool all, client* c);
client* getClient(server* srv, int no);
server* creerServer();
noeud * findNode(noeud* n,char* path);
void quitterArborescence(noeud* n,noeud* racine,bool relative);
bool isChildOf(noeud* n,noeud* of);
void printPath(client* c);
void supprimerChilds(noeud* n);
void printPath_r(noeud* n, client* c);

noeud* deplacerHorizontal(noeud * origin);
noeud* deplacerVertical(noeud * origin);

void* thread_client(void * client_v);
void enqueue(message, client* client);
#endif
