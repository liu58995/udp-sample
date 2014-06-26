#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>

#define LOCAL_SERVER_PORT 1500
#define MAX_MSG_SIZE 1024

int main(int argc, char **argv)
{
    int sd, rc, n, flags;
    int iCounter;
    unsigned int cliLen;
    struct sockaddr_in cliAddr, servAddr;
    char msg[MAX_MSG_SIZE];
    
    //create socket
    sd = socket(AF_INET, SOCK_DGRAM, 0);
    if(sd < 0)
    {
        printf("%s: cannot open socket \n", argv[0]);
        exit(1);
    }
    
    //bind socket
    servAddr.sin_family = AF_INET;
    servAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servAddr.sin_port = htons(LOCAL_SERVER_PORT);
    rc = bind(sd, (struct sockaddr *) &servAddr, sizeof(servAddr));
    
    if(rc < 0)
    {
        printf("%s:cannot bind port number %d\n", argv[0], LOCAL_SERVER_PORT);
        exit(1);
    }
    
    printf("---  waiting for data on port UDP %u\n", LOCAL_SERVER_PORT);
    
    flags = 0;
    iCounter = 0;
    while(1) 
    {
        memset(msg, 0, MAX_MSG_SIZE);
        cliLen = sizeof(cliAddr);
        n = recvfrom(sd, msg, MAX_MSG_SIZE, flags, (struct sockaddr *) &cliAddr, &cliLen);
        if(n < 0)
        {
             printf("recvfram() return %d, cannot receive data \n", n );
             continue; 
        }
        else
        {
             printf("#%4d received %d bytes\n",++iCounter, n);
        }
        
        usleep(1000);
        sendto(sd, msg, 16, flags, (struct sockaddr *) &cliAddr, cliLen);
    }
    
    return 0;
    



}
