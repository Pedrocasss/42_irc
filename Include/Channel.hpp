#ifndef CHANNEL_HPP
#define CHANNEL_HPP

#include <IrcMainHeader.hpp>
#include <Server.hpp>
#include <string>

class Client;

using namespace std;

class Channel {
    private:
        bool                _inviteStatus;
        bool                _isLocked;
        bool                _topicLocked;
        float               _userLimit;
        string              _name;
        string              _pass;
        string              _topic;
        vector<Client *>    _clients;
        vector<Client *>    _invited;
        vector<Client *>    _operator;

    public:
        Channel(const string &name);
        ~Channel();

        vector<Client *>    &getClients();
        
        bool                getTopicStatus();
        string              getTopic();
        void                setTopic(string);
        void                toggleTopic();

        void                addClient(Client *);
        void                removeClient(Client *);

        bool                isOperator(Client *);
        void                addOperator(Client *);
        void                removeOperator(Client *);

        bool                getInviteStatus();
        bool                isInvited(Client *);
        void                addInvited(Client *);
        void                removeInvited(Client *);
        void                toggleInvite();

        bool                isProtected();
        void                toggleLock(string = "");

        bool                isFull();
        void                changeUserLimit(int limit);

        void                broadcast(string &, Client *);
        string              getName();
        string              getPass();
};

#endif // CHANNEL_HPP