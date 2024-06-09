/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: psoares- <psoares-@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/29 20:07:46 by psoares-          #+#    #+#             */
/*   Updated: 2024/04/29 20:08:56 by psoares-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <Channel.hpp>

Channel :: Channel(const string &name) : _inviteStatus(false), _isLocked(false), _topicLocked(true), _userLimit(INFINITY), _name(name), _clients() {}

string Channel :: getName() { return _name; }
string Channel :: getPass() { return _pass; }

vector<Client *> &Channel::getClients() { return this->_clients; }
void Channel::addClient(Client *client) { this->_clients.push_back(client); }

string Channel :: getTopic(){ return this->_topic; }
void Channel :: setTopic(string topic){ this->_topic = topic; }

bool Channel :: getTopicStatus() { return this->_topicLocked; }
void Channel :: toggleTopic(){ this->_topicLocked = !this->_topicLocked; }

bool Channel :: getInviteStatus() { return this->_inviteStatus; }
void Channel :: toggleInvite(){ this->_inviteStatus = !this->_inviteStatus; }

bool Channel :: isProtected() { return this->_isLocked; }

bool Channel :: isFull(){ return (this->_clients.size() >= this->_userLimit); }
void Channel :: changeUserLimit(int limit) { limit == -1 ? this->_userLimit = INFINITY : this->_userLimit = limit; }

Channel::~Channel() {};

void Channel::removeClient(Client *client) {
    if (client == NULL)
        return ;
    vector<Client *>::iterator it = remove(_clients.begin(), _clients.end(), client);
    std::cout << "Removing The client from the channel: " << client->getNick() << endl;
    std::cout << endl;
    _clients.erase(it, _clients.end());
    
}

void Channel::broadcast(string &message, Client *sender) {
    vector<Client *> :: iterator it = this->_clients.begin();
    while (it != this->_clients.end()) {
        if (*it != sender)
            (*it)->writeClient(message);
        ++it;
    }
}

bool Channel::isOperator(Client *client) {
    vector<Client *> :: iterator it = find(_operator.begin(), _operator.end(), client);
    return (it != _operator.end());
}

void Channel::addOperator(Client *client) {
    vector<Client *> :: iterator it = find(_operator.begin(), _operator.end(), client);
    if (it == _operator.end())
        _operator.push_back(client);
}

void Channel::removeOperator(Client *client) {
    vector<Client *> :: iterator it = find(_operator.begin(), _operator.end(), client);
    if (it != _operator.end())
        _operator.erase(it);
}

bool Channel::isInvited(Client *client) {
    vector<Client *> :: iterator it = find(_invited.begin(), _invited.end(), client);

    return (it != _invited.end());
}

void Channel::addInvited(Client *client) {
    vector<Client *> :: iterator it =  find(_invited.begin(), _invited.end(), client);
    if (it == _invited.end())
        _invited.push_back(client);
}

void Channel::removeInvited(Client *client) {
    vector<Client *> :: iterator it = find(_invited.begin(), _invited.end(), client);

    if (it != _invited.end())
        _invited.erase(it);
}

void Channel :: toggleLock(string pass) {
    this->_isLocked = !this->_isLocked;

    if (this->_isLocked)
        this->_pass = pass;
}

ostream &operator<<(ostream &os, Channel &other) {
    os << "Channel: " << other.getName() << endl;
    os << "Clients: " << endl;
    for (vector<Client *>::const_iterator it = other.getClients().begin(); it != other.getClients().end(); ++it)
        os << "    " << (*it)->getNick() << endl;
    return os;
}
