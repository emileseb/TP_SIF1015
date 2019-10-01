#include "base.h"
#include "csapp.h"

//Sert à trouver un noeud à partir d'un chemin et d'un point de départ
noeud *findNode(noeud *n, char *path)
{
    //debug("C:findNode\r\n");
    entrerNoeud(n);
    noeud *retour = null;

    if (path[0] == '/') //Si on commence par un /, on remonte a la racine
    {
        while (n->parent != null)
        {
            entrerNoeud(n->parent);
            quitterNoeud(n);
            n = n->parent;
        }
        path++;
    }
    if (path[0] == 0)//Si la chaine est à la fin, le noeud que l'on désire est l'actuel
        return n;

    int lengthOfElement = 0;
    char *ptr = path;
    while (*ptr != 0 && *ptr != '/')//Trouver la sous chaine se terminant par / ou la fin de la chaine
    {
        lengthOfElement++;
        ptr++;
    }

    noeud *child = deplacerVertical(n);//On entre dans le noeud enfant
    while (child != null)
    {
        if (!child->aSupprimer)//On vérifie qu'il n'est pas en cours de suppression, dans lequel cas on prend comme s'il n'existait pas
        {
            if (strncmp(child->name, path, lengthOfElement) == 0 && child->name[lengthOfElement] == 0) //Trouver le nom exact
            {
                if (*(path + lengthOfElement) == 0) //Pas de sous element, on a notre noeud
                {
                    //debug("L:findNode(EXACT)\r\n");
                    return child;
                }
                else // le fichier se trouve dans ce dossier
                {
                    //debug("L:findNode(PARTIAL)\r\n");
                    retour = findNode(child, path + lengthOfElement + 1);
                    if (retour != null)
                    {
                        quitterNoeud(child);
                        return retour;
                    }
                }
            }
        }
        child = deplacerHorizontal(child);//On se déplace à un même niveau dans l'arbre
    }
    quitterNoeud(n);
    //debug("L:findNode(NULL)\r\n");
    return NULL;
}
//Permet de remettre à jour le nombre de visiteur quand on quitte une section de l'arbre(changer de PWD ou suite à une action temporaire)
void quitterArborescence(noeud *n, noeud *racine, bool relative)
{
    //debug("C:quitterArborescence\r\n");
    while (n != racine && n != NULL)
    {
        noeud *parent = n->parent;
        quitterNoeud(n);
        n = parent;
    }
    if (relative)
        quitterNoeud(racine);
    //debug("L:quitterArborescence\r\n");
}
noeud *deplacerHorizontal(noeud *origin)//Permet de se deplacer entre 2 noeuds du même niveau, met a jour les visiteurs et supprime les noeuds inutile.
{
    //printf("MOVING OUT OF ");
    //printPath_r(origin);
    //printf("\r\n");
    sem_wait(&origin->sem);
    noeud *dest = origin->droit;
    if (dest != NULL)
    {
        sem_wait(&dest->sem_nv);
        dest->nbVisiteur++;

        if (dest->aSupprimer && dest->nbVisiteur == 1)
        {
            sem_post(&dest->sem_nv);
            noeud *tmp = deplacerHorizontal(dest);
            origin->droit = tmp;
            if (tmp != NULL)
                tmp->gauche = origin;

            //debug("FREEING CHILDS\r\n");
            supprimerChilds(dest);
            sem_destroy(&dest->sem_nv);
            sem_destroy(&dest->sem);
            //debug("FREEING NODE\r\n");
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
noeud *deplacerVertical(noeud *origin)//Permet de se deplacer entre un parent et un enfant, met a jour les visiteurs et supprime les noeuds inutile.
{
    sem_wait(&origin->sem);
    noeud *dest = origin->enfant;
    if (dest != NULL)
    {
        sem_wait(&dest->sem_nv);
        dest->nbVisiteur++;

        if (dest->aSupprimer && dest->nbVisiteur == 1)
        {
            sem_post(&dest->sem_nv);
            noeud *tmp = deplacerHorizontal(dest);
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
//Pour les noeuds ou l'on restera un certain temps, cette méthode assure le nombre de visiteur ++
void entrerNoeud(
        noeud *n)
{
    //debug("C:entrerNoeud\r\n");
    if (n != null)
    {
        sem_wait(&n->sem_nv);
        n->nbVisiteur++;
        sem_post(&n->sem_nv);
    }
    //debug("L:entrerNoeud\r\n");
}
//Pour les noeuds ou l'on restera un certain temps, cette méthode assure le nombre de visiteur --
void quitterNoeud(noeud *n)
{
    //debug("C:quitterNoeud\r\n");
    if (n != null)
    {
        sem_wait(&n->sem_nv);
        n->nbVisiteur--;
        sem_post(&n->sem_nv);
    }
    //debug("L:quitterNoeud\r\n");
}
//Obtention d'un client sur un serveur SRV
client *getClient(server *srv, int no)
{
    //debug("C:getClient\r\n");
    debug("Getting client #%d\r\n", no);
    //Parcours de la liste des clients existants
    std::vector<client *> *v = &srv->clients;
    for (int i = 0; i < v->size(); i++)
    {
        client *tmp = v->at(i);
        if (tmp->noClient == no)
        {
            //debug("L:getClient\r\n");
            //Le client existe déja
            return tmp;
        }
    }
    debug("ADDING CLIENT %d\r\n", no);
    //Le client n'était pas présent dans la liste on le crée
    client *c = (client *)malloc(sizeof(client));
    c->noClient = no;
    c->pwd = srv->root;
    c->serv = srv;
    c->first = c->last = NULL;
    c->sockfd = no;
    debug("CLIENT READY %d\r\n", no);
    //On commence le thread pour le client
    pthread_create(&c->thread, NULL, thread_client, c);
    debug(".");
    entrerNoeud(c->pwd);
    debug(".");
    ajouterClient(srv, c);
    debug(".\r\n");
    //debug("L:getClient(NEW)\r\n");
    return c;
}
server *creerServer()
{
    //Création du serveur
    //debug("C:creerServer\r\n");
    server *s = (server *)malloc(sizeof(server));
    noeud *n = (noeud *)malloc(sizeof(noeud));

    //On initialise la racine à NULL car getClient appele entrerNoeud(racine), entrerNoeud(NULL) ne fait rien
    s->root = NULL;
    s->clients = vector<client *>();
    client *admin = getClient(s, 999);

    debug("D:server client count = %d\r\n", (int)s->clients.size());

    n->noProprietaire = admin->noClient;

    n->edit = time(NULL);
    n->nbVisiteur = 1;
    n->flag = 0000;
    n->parent = n->gauche = n->droit = n->enfant = NULL;
    strcpy(n->name, "");
    n->aSupprimer = false;
    sem_init(&n->sem, 0, 1);
    sem_init(&n->sem_nv, 0, 1);

    s->root = n;
    admin->pwd = n;

    //debug("L:creerServer\r\n");
    return s;
}
//Ajoute un client dans la liste du serveur, pourrais être réécrit pour être pûr C au lieu de C++
void ajouterClient(server *srv, client *c)
{
    //debug("C:ajouterClient\r\n");
    srv->clients.push_back(c);
    //debug("L:ajouterClient\r\n");
}
//Permet d'ajouter un fichier dans l'arbre
void ajouterFichier(server *srv, char *path, client *c)
{

    debug("C:ajouterFichier\r\n");
    noeud *t = findNode(c->pwd, path); //Evite les doublons
    if (t != null)
    {
        char tmp[400];
        sprintf(tmp,"Impossible d'ajouter, un fichier possede deja ce nom\r\n");
        write(c->sockfd,tmp,400);
        quitterArborescence(t, c->pwd, path[0] != '/');
        return;
    }

    debug("1:ajouterFichier\r\n");
    char *lidx = strrchr(path, '/');//On trouve le dernier /, le nom de fichier est après ce dernier
    char *name;
    noeud *parent;
    if (lidx == null)//Si on a pas de slash on l'ajoute dans le dossier actuel
    {
        name = path;
        parent = c->pwd;
        entrerNoeud(parent);
    }
    else//Si on a un slash, on met le pointer name sur celui ci, on met la fin de chaine sur le slash
    {
        *lidx = 0;
        name = lidx + 1;
        parent = findNode(c->pwd, path);//On obtient le parent, ayant un 0 a à place du /, la chaîne est terminé et on a une sous chaine
        if (parent == null)//Si le parent n'existe pas, on appel l'ajout pour le créer
        {
            ajouterFichier(srv, path, c);
            parent = findNode(c->pwd, path);
        }
    }
    debug("2:ajouterFichier\r\n");
    //Creation du noeud
    noeud *n = (noeud *)malloc(sizeof(noeud));
    n->noProprietaire = c->noClient;
    //Semaphore de sortie
    sem_init(&n->sem, 0, 1);
    //Semaphore nombre visiteur
    sem_init(&n->sem_nv, 0, 1);

    n->edit = time(NULL);
    n->nbVisiteur = 0;
    n->flag = 700;
    strcpy(n->name, name);
    n->aSupprimer = false;

    //On bloque la sortie du parent le temps de faire les action sur l'arbre
    sem_wait(&parent->sem);
    n->parent = parent;
    n->droit = parent->enfant;
    n->enfant = NULL;
    n->gauche = NULL;
    if (parent->enfant != null)
        parent->enfant->gauche = n;
    parent->enfant = n;
    sem_post(&parent->sem);
    ///

    quitterArborescence(parent, c->pwd, path[0] != '/');

    if (lidx != null)
        *lidx = '/';
    debug("L:ajouterFichier\r\n");
}
void supprimerChilds(noeud *n)//On supprime tout les enfants d'un noeud lors de la suppression de ce noeud(si N a 0 visiteur ces enfants ont 0 visiteur donc aucune validation nécessaire)
{
    //debug("C:supprimerChilds\r\n");
    noeud *child = n->enfant;
    noeud *tmp;

    while (child != NULL)
    {
        tmp = child->droit;
        supprimerChilds(child);

        /*debug("DELETING ");
        printPath_r(child);
        debug("\r\n");*/

        sem_destroy(&child->sem);
        sem_destroy(&child->sem_nv);

        free(child);

        child = tmp;
    }
    //debug("L:supprimerChilds\r\n");
}
void supprimerFichier(server *srv, char *path, client *c)//On marque un fichier comme devant être supprimé
{
    //debug("C:supprimerFichier\r\n");

    noeud *n = findNode(c->pwd, path);
    if (n != null)
    {
        char tmp[400];
        debug("D:found %s \r\n", n->name);
        n->aSupprimer = true;
        sprintf(tmp,"D:%s a été mis en suppression\r\n", n->name);
        write(c->sockfd,tmp,400);
        quitterArborescence(n, c->pwd, path[0] != '/');
    }
    else
    {
        char tmp[400];
        sprintf(tmp,"Aucun fichier de ce nom\r\n");
        write(c->sockfd,tmp,400);
    }

    //debug("L:supprimerFichier\r\n");
}
void modifierFichier(server *srv, char *path, client *c)//On modifie la date de dernière modification d'un fichier
{
    //debug("C:modifierFichier\r\n");
    noeud *n = findNode(c->pwd, path);
    if (n == null)
    {
        char tmp[400];
        sprintf(tmp,"Aucun fichier de ce nom\r\n");
        write(c->sockfd,tmp,400);
        return;
    }
    n->edit = time(NULL);
    quitterArborescence(n, c->pwd, path[0] != '/');
    //debug("L:modifierFichier\r\n");
}
void modifierFlagFichier(server *srv, char *path, client *c, int flag)//On modifie le flag d'un fichier
{
    //debug("C:modifierFlagFichier\r\n");
    noeud *n = findNode(c->pwd, path);
    if (n == null)
    {
        char tmp[400];
        sprintf(tmp,"Aucun fichier de ce nom\r\n");
        write(c->sockfd,tmp,400);
        return;
    }
    if (n->noProprietaire == c->noClient)
        n->flag = flag;
    quitterArborescence(n, c->pwd, path[0] != '/');
    //debug("L:modifierFlagFichier\r\n");
}
void renommer(server *srv, char *path, char *name, client *c)//On renomme un fichier
{
    //debug("C:renommer\r\n");
    noeud *n = findNode(c->pwd, path);
    if (n == null)
    {
        char tmp[400];
        sprintf(tmp, "Aucun fichier de ce nom\r\n");
        write(c->sockfd,tmp,400);
        return;
    }
    noeud *tmp = findNode(n->parent, name);
    if (tmp != null)
    {
        char tmpfifo[400];
        quitterNoeud(tmp);
        sprintf(tmpfifo, "Impossible de renommer, un fichier possede deja ce nom\r\n");
        write(c->sockfd,tmpfifo,400);
    }
    else
    {
        strcpy(n->name, name);
    }
    quitterArborescence(n, c->pwd, path[0] != '/');
    //debug("L:renommer\r\n");
}

void afficherArborescence(server *srv, bool all, client* c)//On affiche le contenu du serveur
{
    debug("C:afficherArborescence\r\n");
    char tmpfifo[400];
    sprintf(tmpfifo, "%*s%s\r\n", 0, "", srv->root->name);
    write(c->sockfd,tmpfifo,400);
    entrerNoeud(srv->root);
    enumererNoeud(srv->root, 1, true, all, c);
    quitterNoeud(srv->root);
    debug("L:afficherArborescence\r\n");
}
void enumererNoeud(noeud *n, int depth, bool r, bool all, client* c)//On énumère les noeuds , r permet de dire que l'on veut voir ses enfants, et all montre aussi ceux en cours de suppression
{
    //debug("C:enumererNoeud\r\n");

    noeud *child = deplacerVertical(n);
    while (child != null)
    {
        if (!child->aSupprimer || all)
        {
            char tmp[400];
            sprintf(tmp,"%*s%s\t [F:%d,Pro:%d,Vis:%d] \r\n", depth, "", child->name, child->flag, child->noProprietaire, child->nbVisiteur);
            write(c->sockfd,tmp,400);
            //,asctime(gmtime(&child->edit)));
            if (r)
                enumererNoeud(child, depth + 1, r, all, c);
        }
        child = deplacerHorizontal(child);
    }

    //debug("L:enumererNoeud\r\n");
}
bool isChildOf(noeud *n, noeud *of)
{
    while (n != NULL)
    {
        if (n == of)
            return true;
        n = n->parent;
    }
    return false;
}
void printPath_r(noeud *n, client* c)//Affiche le chemin pour un fichier à partir de la racine
{
    if (n->parent != null)
    {
        printPath_r(n->parent, c);
        char tmpfifo[400];
        sprintf(tmpfifo, "/%s", n->name);
        write(c->sockfd,tmpfifo,400);
    }
}
void printPath(client *c)
{
    printPath_r(c->pwd, c);
    char tmp[400];
    sprintf(tmp, "/\r\n");
    write(c->sockfd,tmp,400);
}

void *thread_client(void *client_v)//On dois passer le client en void* dû a pthread
{
    client *client = (struct client *)client_v;
    debug("STARTING THREAD FOR CLIENT %d\r\n", client->noClient);
    bool end = false;

    //On obtient le serveur(les commandes on besoin de savoir dans quel serveur, il pourrais il y en avoir plusieurs sur le même process)
    server *srv = client->serv;
    int noClient;
    char typeTransaction;
    char *nomFichier, *param;
    message m;
    m_node *node, *node_previous;
    while (!end)
        //TP2: ajouter une commande qui permet à un client de se déconnecter(mettre ses FD à NULL ou 0)
    {
        if(read(client->sockfd,&m,sizeof(message))>0)
        {
            debug("Information lue\n");
            typeTransaction = m.action;
            nomFichier = m.filename;
            param = m.param;

            //printf("\r\n\r\n\r\n\r\n&&&&&%d %c %s %s&&&&&\r\n\r\n\r\n\r\n", noClient, typeTransaction, nomFichier, param);

            switch (typeTransaction)//Effectuer nos actions sur le serveur
            {
            case 'A':
            {
                ajouterFichier(srv, nomFichier, client);
            }
                break;
            case 'R':
            {
                renommer(srv, nomFichier, param, client);
            }
                break;
            case 'E':
            {
                modifierFichier(srv, nomFichier, client);
            }
                break;
            case 'D':
            {
                supprimerFichier(srv, nomFichier, client);
            }
                break;
            case 'p':
            {
                printPath(client);
            }
                break;
            case 'C'://Changement de PWD
            {
                noeud *pwd = client->pwd;
                noeud *n = findNode(client->pwd, nomFichier);
                if (n == null)
                {
                    char tmp[400];
                    sprintf(tmp, "Aucun fichier de ce nom\r\n");
                    write(client->sockfd,tmp,400);
                    break;
                }
                if (nomFichier[0] == '/')
                {
                    quitterArborescence(pwd, NULL, false);
                }
                client->pwd = n;
            }
                break;
            case 'F':
            {
                int flag;
                sscanf(param, "%d", &flag);
                modifierFlagFichier(srv, nomFichier, client, flag);
            }
                break;
            case 'l':
            {
                noeud *n = findNode(client->pwd, nomFichier);
                if (n == null)
                {
                    char tmp[400];
                    sprintf(tmp,"Aucun fichier de ce nom\r\n");
                    write(client->sockfd,tmp,400);
                    break;
                }
                enumererNoeud(n, 0, false, false, client);
                quitterArborescence(n, client->pwd, nomFichier[0] != '/');
            }
                break;
            case 'S' :
            {
                struct stat stat;
                char *type, *inode, *taille, *blocs, *permission;

                Stat(nomFichier, &stat);
                if (S_ISREG(stat.st_mode)) /* file type*/
                    type = "regular";
                else if (S_ISDIR(stat.st_mode))
                    type = "directory";
                else
                    type = "other";
                inode = "I-node number:            %ld\n", (long) stat.st_ino;     /* numero inode */
                taille = "File size:                %lld bytes\n",(long long) sb.st_size;      /*taille du fichier */
                blocs = "Blocks allocated:         %lld\n",(long long) sb.st_blocks;     /* nombre de blocs alloués */
                permission = "Mode:                     %lo (octal)\n",(unsigned long) sb.st_mode);


                sprintf(tmp, "type: %s, %s, %s, %s, %s\n", type, inode, taille, blocs, permission);
                char tmp[400];
                write(client->sockfd,tmp,400);
            }
                break;
            case 'Q':
            {
                end = !end;
                close(client->sockfd);
            }
                break;
            }
        }
    }
    return NULL;
}
