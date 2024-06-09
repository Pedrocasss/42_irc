#pragma once
#include <PollConnection.hpp>
#include <IrcMainHeader.hpp>

using namespace std;


class Channel;

class Server
{
private: 
    string _port, _pass;
    map <string, Channel *> _channels;
    bool run;
public:
    PollConnection _conn;
    Server();
    Server(string const &, string const&);

    ~Server();

    void stop();
    void start();

    map<int, Client *>  getClients();

    Channel *addChannel(string);
    Channel *getChannel(string);
    string  getPass() const;
    bool    serverLoop();
    bool    ircMensageContent(string);
    void    deleteClient(Client *);
    // void    removeClientFromChannel(string &);    
};
