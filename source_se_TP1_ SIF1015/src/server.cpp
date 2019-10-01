/*
    SIF1015 - François Meunier A18
    Travail de session
    Dépanneur JONATHAN BOISCLAIR SEPTEMBRE 2018

    Nom des étudiants: Emile SEBASTIANUTTI, Elizabeth COURTIEU, Marianne PHILIPPON, Tom RAMIN
    Code permanent des étudiants:SEBE03079800, COUE05549100, PHIM12579800, RAMT22039800
*/
#include "base.h"

noeud* deplacerHorizontal(noeud * origin)
{
	sem_wait(&origin->sem);
	noeud * dest = origin->droit;
	if(dest!=NULL)
	{
		sem_wait(&dest->sem_nv);
		dest->nbVisiteur++;
		if(dest->aSupprimer && dest->nbVisiteur ==1)
		{
			sem_post(&dest->sem_nv);
			noeud * tmp = deplacerHorizontal(dest);
			origin->droit = tmp;
			if(tmp!=NULL)
				tmp->gauche = origin;
			
			supprimerChilds(dest);
			sem_destroy(&dest->sem_nv);
			sem_destroy(&dest->sem);
			free(dest);
			dest = tmp;
		}
		else
			sem_post(&dest->sem_nv);
	}
	sem_wait(&origin->sem_nv);
	origin->nbVisiteur--;
	sem_post(&origin->sem_nv);
	sem_post(&origin->sem);
	return dest;
}
	
noeud* deplacerVertical(noeud * origin)
{
	sem_wait(&origin->sem);
	noeud * dest = origin->enfant;
	if(dest!=NULL)
	{
		sem_wait(&dest->sem_nv);
		dest->nbVisiteur++;
		if(dest->aSupprimer && dest->nbVisiteur ==1)
		{
			sem_post(&dest->sem_nv);
			noeud * tmp = deplacerHorizontal(dest);
			origin->enfant = tmp;
			supprimerChilds(dest);
			sem_destroy(&dest->sem_nv);
			sem_destroy(&dest->sem);
			free(dest);
			dest = tmp;
		}
		else
			sem_post(&dest->sem_nv);
		
	}
	sem_post(&origin->sem);
	return dest;
}

noeud * findNode(noeud* n,char* path)
{
    //debug("C:findNode\r\n");
    entrerNoeud(n);
    noeud* retour=null;

    if(path[0]=='/')
    {
        while(n->parent!=null)	
        {
            entrerNoeud(n->parent);
            quitterNoeud(n);
            n = n->parent;
        }
        path++;
    }
    if(path[0]==0)
        return n;

    int lengthOfElement=0;
    char* ptr=path;
    while(*ptr != 0 && *ptr!= '/')
    {
        lengthOfElement++;
        ptr++;
    }

    noeud * child = deplacerVertical(n); 
	while(child != null)
    {
        if(!child->aSupprimer)
        {
            if(strncmp(child->name,path,lengthOfElement)==0 && child->name[lengthOfElement]==0)
            {
                if(*(path+lengthOfElement)==0)
                {
                    //debug("L:findNode(EXACT)\r\n");
                    return child;
                }
                else
                {
                    //debug("L:findNode(PARTIAL)\r\n");
                    retour=findNode(child,path+lengthOfElement+1);
                    if(retour!=null)
                    {
                        quitterNoeud(child);
                        return retour;
                    }
                }
            }
        }
        		
		child = deplacerHorizontal(child);
    }
    quitterNoeud(n);
    //debug("L:findNode(NULL)\r\n");
    return NULL;
}

void quitterArborescence(noeud* n,noeud* racine,bool relative)
{
    //debug("C:quitterArborescence\r\n");
    while(n!=racine && n!=NULL)
    {
        noeud * parent= n->parent;
        quitterNoeud(n);
        n = parent;
    }
    if(relative)
        quitterNoeud(racine);
    //debug("L:quitterArborescence\r\n");
}


void entrerNoeud(noeud* n)
{
    //debug("C:entrerNoeud\r\n");
    if(n!=null){
		sem_wait(&n->sem_nv);	//-------------------------------protection de n->nbVisiteur------------------------------------
        n->nbVisiteur++;
		sem_post(&n->sem_nv);	//-------------------------------protection de n->nbVisiteur------------------------------------
	}
    //debug("L:entrerNoeud\r\n");
}
void quitterNoeud(noeud* n)
{
    //debug("C:quitterNoeud\r\n");
    if(n!=null){
		sem_wait(&n->sem_nv);	//-------------------------------protection de n->nbVisiteur------------------------------------
        n->nbVisiteur--;
		sem_post(&n->sem_nv);	//-------------------------------protection de n->nbVisiteur------------------------------------
    }
    //debug("L:quitterNoeud\r\n");
}
void* thread_client(void* ptr)
{
	client* c = (client*)ptr;
	server* srv = c->srv;
	while(true)
	{
		sem_wait(&c->sem_element_ready);
		sem_wait(&c->sem_file);
		
		liste* tmp = c->debutListe;
		c->debutListe = c->finListe = NULL;
		sem_post(&c->sem_file);
		
		while(tmp!=NULL)
		{
			message m = tmp->info;
			liste* prec = tmp;
			tmp = tmp->suiv;
			free(prec);
		
		 char typeTransaction;
         char *nomFichier,*param;
		 typeTransaction = m.typeTransaction;
		 nomFichier = m.nomFichier;
		 param = m.param;
		 
		switch(typeTransaction)
            {
                case 'A':{
                    ajouterFichier(srv,nomFichier,c);
                }break;
                case 'R':{
                    renommer(srv,nomFichier,param,c);
                }break;
                case 'E':{
                    modifierFichier(srv,nomFichier,c);
                }break;
                case 'D':{
                    supprimerFichier(srv,nomFichier,c);
                }break;
                case 'p':{
                    printPath(c);
                }break;
                case 'C':{
                    noeud * pwd = c->pwd;
                    noeud * n = findNode(c->pwd,nomFichier);
                    if(n==null)
                    {
                        printf("Aucun fichier de ce nom\r\n");
                        break;
                    }
                    if(nomFichier[0]=='/')
                    {
                        quitterArborescence(pwd,NULL,false);
                    }
                    c->pwd = n;
                }break;
                case 'F':{
                    int flag;
                    sscanf(param,"%d",&flag);
                    modifierFlagFichier(srv,nomFichier,c,flag);
                }break;
                case 'l':{
                    noeud * n = findNode(c->pwd,nomFichier);
                    if(n==null)
                    {
                        printf("Aucun fichier de ce nom\r\n");
                        break;
                    }
                    enumererNoeud(n,0,false,false);
                    quitterArborescence(n,c->pwd, nomFichier[0]!='/');
                }break;
            }
		}
	}
	
	return NULL;
}



client* getClient(server* srv, int no, message m)
{
	message msg = m;
	liste * l = (liste*) malloc(sizeof(liste));
	l->info = msg;
	l->suiv = NULL;
    debug("C:getClient\r\n");
    std::vector<client*>* v = &srv->clients;
    for(int i=0;i<v->size();i++)
    {
        client* tmp = v->at(i);
        if(tmp->noClient == no)
        {
            debug("L:getClient\r\n");
			sem_wait(&tmp->sem_file);
			if(tmp->finListe==NULL)
				tmp ->finListe = tmp->debutListe = l;
			else
				tmp->finListe=tmp->finListe->suiv = l;
			sem_post(&tmp->sem_file);
            return tmp;
        }
    }
    client *  c = (client*) malloc(sizeof(client));
    c->noClient=no;
    c->pwd = srv->root;
	c->srv = srv;
	c->debutListe = l;
	c->finListe = l;
	sem_init(&c->sem_file, 0, 1);	//initialisation des sémaphores de la liste d'attente
	sem_init(&c->sem_element_ready, 0, 1);	//initialisation des sémaphores de l'élément prêt
	pthread_create(&c->tid,NULL,thread_client,c);	//Création d'un thread (thread_client(void *ptr)) qui fait la gestion des commandes d'un client
    entrerNoeud(c->pwd);
    ajouterClient(srv, c);
    //debug("L:getClient(NEW)\r\n");
    return c;
}
server* creerServer()
{
    debug("C:creerServer\r\n");
    server* s = (server*)malloc(sizeof(server));
    client* admin=(client*)malloc(sizeof(client));
    admin->noClient = 999;

    ajouterClient(s,admin);
    
    debug("D:server client count = %d\r\n",(int)s->clients.size());

    noeud* n = (noeud*)malloc(sizeof(noeud));
    n->noProprietaire = admin->noClient;
    
    n->edit = time(NULL);
    n->nbVisiteur=1;
    n->flag = 0000;
    n->parent = n->gauche = n->droit = n->enfant = NULL;
    strcpy(n->name,"");
    n->aSupprimer=false;
    s->root = n;
    admin->pwd = n;
	sem_init(&n->sem, 0, 1);
	sem_init(&n->sem_nv, 0, 1);
	sem_init(&admin->sem_file, 0, 1);
	sem_init(&admin->sem_element_ready, 0, 1);
	pthread_create(&admin->tid,NULL,thread_client,admin);
    debug("L:creerServer\r\n");
    return s;
}
void ajouterClient(server* srv,client* c){
     //debug("C:ajouterClient\r\n");
    srv->clients.push_back(c);
    //debug("L:ajouterClient\r\n");
}
void ajouterFichier(server* srv, char* path,client* c){

    //debug("C:ajouterFichier\r\n");
    noeud* t=findNode(c->pwd,path);
    if(t!=null)
    {
        printf("Impossible d'ajouter, un fichier possede deja ce nom\r\n");
        quitterArborescence(t,c->pwd, path[0]!='/');
        return;
    }

     
    char* lidx= strrchr(path,'/');
    char* name;
    noeud* parent;
    if(lidx==null)
    {
        name=path;
        parent = c->pwd;
        entrerNoeud(parent);
    }else{
        *lidx=0;
        name=lidx+1;
        parent = findNode(c->pwd,path);
        if(parent==null)
        {
            ajouterFichier(srv,path,c);
            parent = findNode(c->pwd,path);
        }
    }
    //Creation du noeud
    noeud* n = (noeud*)malloc(sizeof(noeud));
    n->noProprietaire = c->noClient;
    n->parent = parent;
    n->droit = parent->enfant;
    n->enfant = NULL;
    n->gauche = NULL;
    if(parent->enfant!=null)
        parent->enfant->gauche=n;
    parent->enfant=n;
	//initialisation des semaphores
	sem_init(&n->sem, 0, 1);
	sem_init(&n->sem_nv, 0, 1);

    n->edit = time(NULL);
    n->nbVisiteur=0;
    n->flag = 700;
    strcpy(n->name,name);
    n->aSupprimer=false;
    ///

    quitterArborescence(parent,c->pwd, path[0]!='/');

    if(lidx!=null)
        *lidx='/';
    //debug("L:ajouterFichier\r\n");
}

void supprimerChilds(noeud* n)
{
	noeud * child = n->enfant;
	noeud* tmp;
	while(child != null)
	{
		tmp=child->droit;
		supprimerChilds(child);
		sem_destroy(&child->sem);
		sem_destroy(&child->sem_nv);
		free(child);
		child=tmp;
	}
}


void supprimerFichier(server* srv, char* path,client* c){
    //debug("C:supprimerFichier\r\n");

    noeud* n = findNode(c->pwd,path);
    if(n!=null)
    {
        debug("D:found %s \r\n",n->name);
        n->aSupprimer=true;
        printf("D:%s a été mis en suppression\r\n",n->name);
        quitterArborescence(n,c->pwd, path[0]!='/');
    }
    else
    {
        printf("Aucun fichier de ce nom\r\n");
    }

    //debug("L:supprimerFichier\r\n");
}
void modifierFichier(server* srv, char* path,client* c)
{
     //debug("C:modifierFichier\r\n");
    noeud* n = findNode(c->pwd,path);
    if(n==null)
    {
        printf("Aucun fichier de ce nom\r\n");
        return;
    }
    n->edit = time(NULL);
    quitterArborescence(n,c->pwd, path[0]!='/');
    //debug("L:modifierFichier\r\n");
}
void modifierFlagFichier(server* srv,char* path, client* c, int flag)
{
     //debug("C:modifierFlagFichier\r\n");
    noeud* n = findNode(c->pwd,path);
    if(n==null)
    {
        printf("Aucun fichier de ce nom\r\n");
        return;
    }
    if(n->noProprietaire == c->noClient)
        n->flag = flag;
    quitterArborescence(n,c->pwd, path[0]!='/');
    //debug("L:modifierFlagFichier\r\n");
}
void renommer(server* srv, char* path, char* name,client* c){
     //debug("C:renommer\r\n");
    noeud* n = findNode(c->pwd,path);
    if(n==null)
    {
        printf("Aucun fichier de ce nom\r\n");
        return;
    }
    noeud* tmp = findNode(n->parent,name);
    if(tmp!=null)
    {
        quitterNoeud(tmp);
        printf("Impossible de renommer, un fichier possede deja ce nom\r\n");
    }
    else
    {
        strcpy(n->name, name);
    }

    
    quitterArborescence(n,c->pwd, path[0]!='/');
    //debug("L:renommer\r\n");
}

void afficherArborescence(server* srv,bool all){
    //debug("C:afficherArborescence\r\n");
    printf ("%*s%s\r\n", 0, "", srv->root->name);
    entrerNoeud(srv->root);
    enumererNoeud(srv->root, 1,true,all);
    quitterNoeud(srv->root);
    //debug("L:afficherArborescence\r\n");
}
void enumererNoeud(noeud* n, int depth,bool r,bool all)
{
    //debug("C:enumererNoeud\r\n");
    
    noeud * child = deplacerVertical(n);
    while(child != null)
    {
        if(!child->aSupprimer || all)
        {
            printf ("%*s%s\t [F:%d,Pro:%d,Vis:%d] \r\n", depth, "", child->name,child->flag, child->noProprietaire,child->nbVisiteur);//,asctime(gmtime(&child->edit)));
            if(r)
                enumererNoeud(child,depth+1,r,all);
        }
		child = deplacerHorizontal(child);
    }
    
    //debug("L:enumererNoeud\r\n");
}
bool isChildOf(noeud* n,noeud* of)
{
    while(n!=NULL)
    {
        if(n==of)
            return true;
        n= n->parent;
    }
    return false;
}
void printPath_r(noeud* n)
{
    if(n->parent!=null)
    {
        printPath_r(n->parent);
        printf("/%s",n->name);
    }
}
void printPath(client* c)
{
    printPath_r(c->pwd);
    printf("/\r\n");
}