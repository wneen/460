#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <unistd.h>
#include <curses.h>
#include "networking.h"

//connStatus, 1 = connected, 0 = disconnected
int connStatus;
int sock;

int connectToServer(char * ipAddress, int port)
{
    struct sockaddr_in sAddr;
    memset((void*) &sAddr, 0, sizeof(struct sockaddr_in));
    sAddr.sin_family = AF_INET;
    sAddr.sin_addr.s_addr = INADDR_ANY;
    sAddr.sin_port = 0;

    sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    bind(sock, (const struct sockaddr*) &sAddr, sizeof(sAddr));

    sAddr.sin_addr.s_addr = inet_addr(ipAddress);
    sAddr.sin_port = htons(port);

    if(connect(sock, (const struct sockaddr *) &sAddr, sizeof(sAddr)) != 0)
    {
        perror("Error connecting to server");
        return -1;
    }
    else
    {
        connStatus = 1;
    }

    return connStatus;
}

int isConnected()
{
    return connStatus;
}

int closeServer()
{
    return close(sock);

}

int getSock()
{
    return sock;
}

int sendConnectInit(ConnectInit * cI)
{
    return send(sock, cI, sizeof(ConnectInit), 0);
}

int getACK(ConnectACK * cACK)
{
    return recv(sock, cACK, sizeof(ConnectACK),0);
}

int sendChat(Chat * ch)
{
    char * sChat = (char*)malloc(1000);
    int size = serializeChat(sChat, ch);
    if(!size)
    {
        printw("Error serializing struct\n");
        free(sChat);
        return 0;
    }
    else
    {//send size of string and them data
        printw("MSG: %s\n",sChat);
        refresh();
        int recvSize = 0;
        send(sock,&size,sizeof(int),0);
        recv(sock, &recvSize, sizeof(int),0);
        recvSize = send(sock,sChat, size,0);
        //printw("Sending:%s\n",sChat);
        free(sChat);
        return recvSize;
    }
}

int receiveChat(Chat * ch)
{
    int recvSize = 0;
    recv(sock, &recvSize, sizeof(int),0);
    send(sock,&recvSize,sizeof(int),0);
    char * sChat = malloc(recvSize);
    if(!recv(sock, sChat,recvSize,0))
    {
        return 2; //something wrong
    }

    deserializeChat(sChat,ch);
    return 0; //no errors    
    free(sChat);


}

int serializeChat(char * msg, Chat * ch)
{
    //had to increment by 1, strpinf does not count the added \0
    return 1+sprintf(msg,"%i`%i`%i`%s",ch->id,ch->status,ch->messageLen,ch->message);
}

int deserializeChat(char * msg, Chat * ch)
{
    char * delim = strtok(msg,"`");
    ch->id = atoi(delim);
    delim = strtok(NULL,"`");
    ch->status = atoi(delim);
    delim = strtok(NULL,"`");
    ch->messageLen = atoi(delim);
    delim = strtok(NULL,"`");
    strcpy(ch->message,delim);
    return 0; //0 for now errors
}
