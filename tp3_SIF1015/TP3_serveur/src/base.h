#ifndef BASE_H
#define BASE_H
typedef unsigned char byte;
typedef unsigned short ushort;
typedef unsigned long ulong;
using namespace std;

#if !defined(NO_DEBUG)&& !defined(REDUCED_DEBUG)
#define debug(...) printf (__VA_ARGS__); fflush(stdin);
#else 
#define debug(...) // __VA_ARGS__
#endif

#include <iostream>
#include "stdio.h"
#include <time.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <cstdint>
#include <stdexcept>
#include <sys/mman.h>
#include <fcntl.h>
#include <string.h>
#include "pthread.h"
#include "semaphore.h"
#include <vector>
#include <queue>
#include <sys/types.h>
#include <sys/stat.h>

#define SERVER_FIFO_NAME "/tmp/serv_fifo" //quand on va créer le fifo serveur : mkfifo(SERVER_FIFO_NAME, 0777); cf cours6 diapo 21
#define CLIENT_FIFO_NAME "/tmp/cli_%d_fifo" //%d = cid.

#include "server.h"
#define null NULL

#define READ_BYTE_SYNC(FD,ADDR) while(read(FD,ADDR,1)<=0);

#define HZ_1000000  1L
#define HZ_100000   10L
#define HZ_10000    100L
#define HZ_1000     1000L
#define HZ_100      10000L
#define HZ_10       100000L
#define HZ_1        1000000L
#define HZ_01       10000000L
#define HZ_001      100000000L
#define HZ_0001     1000000000L

#define NEXT(WAIT)  (usleep(WAIT))

#define SPEED_0_001_HZ    while(NEXT(HZ_0001)==0)
#define SPEED_0_01_HZ    while(NEXT(HZ_001)==0)
#define SPEED_0_1_HZ    while(NEXT(HZ_01)==0)
#define SPEED_1_HZ      while(NEXT(HZ_1)==0)
#define SPEED_5_HZ      while(NEXT(HZ_10*2)==0)
#define SPEED_10_HZ     while(NEXT(HZ_10)==0)
#define SPEED_50_HZ     while(NEXT(HZ_100*2)==0)
#define SPEED_100_HZ    while(NEXT(HZ_100)==0)
#define SPEED_500_HZ    while(NEXT(HZ_1000*2)==0)
#define SPEED_1000_HZ   while(NEXT(HZ_1000)==0)
#define SPEED_5000_HZ   while(NEXT(HZ_10000*2)==0)
#define SPEED_10000_HZ  while(NEXT(HZ_10000)==0)
#define SPEED_50000_HZ  while(NEXT(HZ_100000*2)==0)
#define SPEED_100000_HZ while(NEXT(HZ_100000)==0)
#define SPEED_500000_HZ while(NEXT(HZ_1000000*2)==0)
#define SPEED_1000000_HZ while(NEXT(HZ_1000000)==0)

#define SPEED_1_KHZ     SPEED_1000_HZ
#define SPEED_5_KHZ     SPEED_5000_HZ
#define SPEED_10_KHZ    SPEED_10000_HZ
#define SPEED_50_KHZ    SPEED_50000_HZ
#define SPEED_100_KHZ   SPEED_100000_HZ
#define SPEED_500_KHZ   SPEED_500000_HZ
#define SPEED_1000_KHZ  SPEED_1000000_HZ

#define SPEED_1_MHZ SPEED_1000_KHZ

#define VALID(fd) (fd>0)

#endif