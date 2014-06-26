// udpsend.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <stdio.h>   
#include <Winsock2.h>   
#define SERVER_PORT  1500
#define SERVER_ADDR  "10.12.23.4 "




static int isReadable(SOCKET sd,int * error,int timeOut) { // milliseconds
    fd_set socketReadSet;
    FD_ZERO(&socketReadSet);
    FD_SET(sd,&socketReadSet);
    struct timeval tv;
    if (timeOut) {
        tv.tv_sec  = timeOut / 1000;
        tv.tv_usec = (timeOut % 1000) * 1000;
    } else {
        tv.tv_sec  = 0;
        tv.tv_usec = 0;
    } // if
    if (select(sd+1,&socketReadSet,0,0,&tv) == SOCKET_ERROR) {
        *error = 1;
        return 0;
    } // if
    *error = 0;
    return FD_ISSET(sd,&socketReadSet) != 0;
} /* isReadable */

int SendData(const char* srv_addr, unsigned int port)   
{   
    SOCKET socket1;   


    struct sockaddr_in server_addr;   
    size_t len = sizeof(server_addr);   
    server_addr.sin_family = AF_INET;   
    server_addr.sin_port = htons(port); 
    server_addr.sin_addr.s_addr = inet_addr(srv_addr); 

    socket1 = socket(AF_INET,SOCK_DGRAM,0);   

    char send_buffer[32];
    char recv_buffer[32];
    unsigned int send_seq_no = 0;

    //for statistics
    LARGE_INTEGER nFreq;
    LARGE_INTEGER nBeginTime;
    LARGE_INTEGER nEndTime;
    double time;
    QueryPerformanceFrequency(&nFreq);

    while (1)   
    {   
        memset((void*)send_buffer, 0, sizeof(send_buffer));
        sprintf_s(send_buffer, "#%4d", send_seq_no++);

        QueryPerformanceCounter(&nBeginTime); 
        int num_sent = sendto(socket1, send_buffer, sizeof(send_buffer),0,
            (struct sockaddr*)&server_addr, sizeof(server_addr));
        if (num_sent >= 0)   
        {   
           // printf("%d bytes has been sent\n", num_sent);
        }  

        int err_code;
        if(isReadable(socket1, &err_code, 3000)) //wait for 3000 ms
        {
            int recv_len = sizeof(server_addr);
            memset((void*)recv_buffer, 0, sizeof(recv_buffer));
            int num_read = recvfrom(socket1, recv_buffer, sizeof(recv_buffer), 0, 
                (struct sockaddr*)&server_addr, &recv_len);
            if( num_read > 0)
            {
                if(strcmp(send_buffer, recv_buffer) == 0)
                {
                    QueryPerformanceCounter(&nEndTime); 
                    time=(double)(nEndTime.QuadPart-nBeginTime.QuadPart)/(double)nFreq.QuadPart;
                    printf("%s echo time = %6.2f ms\n", recv_buffer,time*1000);
                }
            }
        }
        Sleep(1000); //wait for 1 second for each loop.
    }   
    closesocket(socket1);   
}   

void wcharTochar(const wchar_t *wchar, char *chr, int length)  
{  
    WideCharToMultiByte( CP_ACP, 0, wchar, -1, chr, length, NULL, NULL );  
}  

int _tmain(int argc, TCHAR* argv[])
{
    WSADATA wsaData;   
    int iErrorCode;   
    if (iErrorCode = WSAStartup(MAKEWORD(2,2),&wsaData)) // initialze windows dll
    {    
        printf("WSAStartup() failed,  error code:%d \n", iErrorCode);   
        WSACleanup();   
        exit(-1);   
    }   
    else
    {
        printf("This application uses socket version %d.%d\n", wsaData.wHighVersion,wsaData.wVersion); 
    }

    char  srv_addr[128];
    int port = SERVER_PORT;

    strcpy_s(srv_addr, SERVER_ADDR);
    if(argc > 1)
    {
        wcharTochar(argv[1], srv_addr, sizeof(srv_addr));
    }

    if(argc > 2)
    {
        char tmp[16] = {0};
        wcharTochar(argv[2], tmp, sizeof(tmp));
        port = atoi(tmp);
    }

    printf("Begin to send data to %s:%d\n", srv_addr, port);
    SendData(srv_addr, port);

    WSACleanup();  
    return 0;
}