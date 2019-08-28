
#pragma once

#include <deque>
#include <vector>
using namespace std;;

#include <WinSock2.h>
#include <ws2tcpip.h>

struct connections {
        SOCKET serv;           //SOCKET on server side after accept()
        sockaddr_in clntaddr;  //client address connected to server
};

class Socket
{
        int res;
        char mess[256];
        SOCKET sock;
		bool onConnected;

        sockaddr_in saddr;                 //address to which get connected
        char hostname[NI_MAXHOST];         //host name
        int family, socktype, protocol, port;

        char *buff;                        //buff for 1 single read
        DWORD lastread;                    //len of last read
        char *err;                         //err pointer

        vector<BYTE> array;                             //whole packet recieved
        deque<connections *> active_connections;     //connections
        SOCKET lastaccepted;


public:
        Socket(void);
        ~Socket(void);

        char *Create(int af, int type, int proto);
        char *Select(HWND handle, UINT msg, long events);
        char *Connect(const char *addr, WORD port);
        char *Listen(WORD);
        char *Accept();
        char *Disconnect(SOCKET s=0);
        bool Canceled(SOCKET);
        char *Read(SOCKET s = 0);                                    //return 0-terminated char string
        char *Send(char *, SOCKET s = 0);
        char *GetError();


        /////////CLIENT////////////////////////////////////////////////////////////////
        char *gethostname() {
                return &hostname[0];        //client connected host name
        }
        char *getaddr() {
                return inet_ntoa(saddr.sin_addr);        //client socket
        }
        int getport() {
                return port;        //client socket
        }
        int getlastread() {
                return lastread;
        }
        vector<BYTE> *getarray() {
                return &array;
        }


        /////////SERVER/////////////////////////////////////////////////////////////////
        char *getaddr(SOCKET);                                       //server active connections
        char *getaddr(int);                                          //server active connections
        SOCKET getclientsocket(char *);                              //server active connections
        int getport(SOCKET);                                         //server active connections
        SOCKET getlastaccepted() {
                return lastaccepted;        //lastaccepted socket
        }
        int getclientsnum() {
                return (int)active_connections.size();
        }
		bool isCreated();
		bool isConnected();
};

