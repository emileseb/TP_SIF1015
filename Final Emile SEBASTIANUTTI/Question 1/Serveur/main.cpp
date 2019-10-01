#include "base.h"

int main() {

    int serv_sockfd, client_sockfd;
    socklen_t serv_len, client_len;
    struct sockaddr_in serv_address;
    struct sockaddr_in client_address;

    serv_sockfd = socket(AF_INET, SOCK_STREAM, 0);

    serv_address.sin_family = AF_INET;
    serv_address.sin_addr.s_addr = inet_addr("192.168.90.20");
    serv_address.sin_port = 9734;
    serv_len = sizeof(serv_address);
    bind(serv_sockfd, (struct sockaddr *)&serv_address, serv_len);

    server* srv= creerServer();
    if(srv==null)
    {
        debug("ERROR CANNOT CREATE SERVER\r\n");return 0;
    }

    listen(serv_sockfd, 5);
    while(1) {
        printf("server waiting\n");

        client_len = sizeof(client_address);
        client_sockfd = accept(serv_sockfd, (struct sockaddr *)&client_address, &client_len);
        client* client = getClient(srv,client_sockfd);
    }
    return 0;
}
