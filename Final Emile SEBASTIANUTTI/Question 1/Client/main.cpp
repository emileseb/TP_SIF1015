#include "base.h"
#include <ctype.h>

int sockfd;
bool continuer = true;
void * threadLecture(void* n)
{
    char retourserveur[400];
    while(continuer) {
        if (read(sockfd, retourserveur, 400) > 0) {
            printf("%s", retourserveur);
        }
    }
}
int main()
{
    int len;
    int result;
    struct sockaddr_in address;
    pthread_t tid;
    void* n;
    message m;
    m.cid = getpid();

    sockfd = socket(AF_INET, SOCK_STREAM, 0);

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = inet_addr("127.0.0.1");
    address.sin_port = 9734;
    len = sizeof(address);

    result = connect(sockfd, (struct sockaddr *)&address, len);

    if (result == -1) {
        printf("ERREUR : ça marche pas\n");
        exit(EXIT_FAILURE);
    }
    m.cid = getpid();
    char line[400];
    int rd=0;
    debug("debug n2\n");
    pthread_create(&tid, NULL, threadLecture, n);//-----------------------------------------------------------------

    while(continuer){
        rd=VALID(fgets(line, 400, stdin));
        if (line[0]=='Q')
        {
            continuer == false;
        }
        int ct=sscanf(line,"%c %s %s",&m.action, m.filename ,m.param);
        debug("Préparation écriture dans le socket\n");
        write(sockfd, &m, sizeof(message));
    }
    close(sockfd);
    exit(EXIT_SUCCESS);
}
