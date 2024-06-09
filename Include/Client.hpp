#ifndef CLIENT_HPP
#define CLIENT_HPP

#include <IrcMainHeader.hpp>
#include <IrcMessage.hpp>

class Server;
class Channel;
class Bot;
using namespace std;

extern string NOTACC;
extern string WRONGARGS;

class Client {
    private:
        Bot     *bot;
        string  _pass, _serverPass;
        string  _nick, _oldnick;
        string  _user;
        Server  *_server;
        bool    _accepted;

        void    Pass(IrcMessage &);
        void    Nick(IrcMessage &);
        void    User(IrcMessage &);
        void    checkLogin();

        void    Join(IrcMessage &);
        void    Priv(IrcMessage &);
        void    Who(IrcMessage &);
        void    Part(IrcMessage &);
        void    Kick(IrcMessage &);
        void    Topic(IrcMessage &);
        void    Invite(IrcMessage &);
        void    Quit(IrcMessage &);
        void    BotMsg(IrcMessage &);
        void    Modes(IrcMessage &);

        void    ModeO(vector<string>, string);
        void    ModeI(vector<string>, string);
        void    ModeT(vector<string>, string);
        void    ModeK(vector<string>, string);
        void    ModeL(vector<string>, string);
        string  makeMsg(string);

        void refresh(string);
        
        map<string, Channel *>                                  _channels;
        map<string, void(Client ::*)(IrcMessage &)>             fns;
        map<string, void(Client ::*) (vector<string>, string)>  modes;

    public:
        int toDisconnect;
        Client(int, Server *);
        ~Client();
        pollfd          socket;

        int readClient(string &);
        int writeClient(string &);

        const string&   getName() const;
        const string&   getPass() const;
        const string&   getNick() const;
        
        Channel*        addChannel(string);
        pollfd          *getPoll();

        void            removeChannel(const string& channelName);
        void            parseMessage(IrcMessage &);
  
        void            setPass(const string &);
        void            setAccept(bool);
        bool            getAccept();
};

#endif