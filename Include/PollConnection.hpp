#pragma once

#include <IrcMainHeader.hpp>
#include "Client.hpp"
#include <ostream>

using namespace std;
class Server;
class PollConnection
{

public:
    PollConnection();
    map<int, Client *>  connectionList;
    vector<pollfd>      connectionSockets;

    int numClients;
    sockaddr_in sI;

    void                    addClient(int, Server *);
    void                    delClient(Client *);
    void                    closeConnecions();
    int                     getSize() const;
    Client *                operator[](size_t );

    map<int, Client *>      getClients() const;
    vector<pollfd>          getPoll() const;
    pollfd                  GetSpecifPoll(int);
    Client *                GetSpecifClient(int);
};

ostream &operator<<(ostream &os, PollConnection const &other);