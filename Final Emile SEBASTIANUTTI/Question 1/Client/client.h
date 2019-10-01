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
