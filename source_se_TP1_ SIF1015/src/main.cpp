/*
    SIF1015 - François Meunier A18
    Travail de session
    Dépanneur JONATHAN BOISCLAIR SEPTEMBRE 2018

    Nom des étudiants: Emile SEBASTIANUTTI, Elizabeth COURTIEU, Marianne PHILIPPON, Tom RAMIN
    Code permanent des étudiants:SEBE03079800, COUE05549100, PHIM12579800, RAMT22039800
*/
#include "base.h"

int main() {
    server* srv= creerServer();

    if(srv==null)
    {
        //debug("ERROR CANNOT CREATE SERVER\r\n");return 0;
    }
    char line[400];
    FILE *f = fopen("t.txt", "rt");//stdin;//

    if(!VALID(f))
    {
        //debug("ERROR CANNOT OPEN FILE\r\n");return 0;
    }
    int rd=0;
    rd=VALID(fgets(line, 400, f));
    bool end=false;
	while(!end){
		//debug(line,"%s");		
        if(line[0]!='*')
        {
			//debug("------------------------------------------------\r\n");
            //debug("%c\r\n",line[0]);
            if(line[0]=='Q')//Commande serveur
            {
                //debug("Fermeture du serveur\r\n");
                afficherArborescence(srv,true);
                return 0;
            }
            else if(line[0]=='L')//Commande serveur
            {
                //debug("Commande serveur LIST\r\n");
                afficherArborescence(srv,false);
            }
            else if(line[0]=='A')//Commande serveur
            {
                //debug("Commande serveur LIST\r\n");
                afficherArborescence(srv,true);
            }
            else if(line[0]=='C')//Commande serveur
            {
                //debug("Commande serveur Clients\r\n");
                for(int i=0;i<srv->clients.size();i++)
                {
                    client* c = srv->clients[i];
                    printf("%d @",c->noClient);
                    printPath(c);
                }
            }
            else if(line[0]=='W')//Commande serveur
            {
                //debug("Commande serveur Wait\r\n");
                rd=VALID(scanf("%s",line));
            }
            else//Commande client
            {
                int noClient;
				message m;
                int ct=sscanf(line,"%d %c %s %s",&noClient,&m.typeTransaction, m.nomFichier ,m.param);
                if(ct>0)
                {
                    //debug("%s\r\n",line);
                    client* client = getClient(srv,noClient, m);
					sem_post(&client->sem_element_ready);
                }
            }
        }
        rd=VALID(fgets(line, 400, f));
        if(feof(f))
        {
            //end=true;
            f = stdin;
        }
    }

    return 0; 
}