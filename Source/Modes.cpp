#include <Client.hpp>
#include <Channel.hpp>
#include <Server.hpp>

string processString(string input) {
    size_t pos = input.find_first_of("abcdefghijklmnopqrstuvwxyz");
    if (pos == string::npos || pos == 0) {
        return "+" + string(1, input[pos]);
    }
    if (input[pos - 1] == '-') {
        return "-" + string(1, input[pos]);
    } else if (input[pos - 1] != '#') {
        return "+" + string(1, input[pos]);
    }
    return string(1, input[pos]);
}

void Client :: ModeO(vector<string> params, string oper) {
    string ch = params[0];
    string nick = params[2];
    vector<Client *>::iterator it;
    string msg = "";
    for (it = this->_channels[ch]->getClients().begin(); it != this->_channels[ch]->getClients().end(); ++it) {
        if ((*it)->getNick() == nick) {
            if (!this->_channels[ch]->isOperator(*it) && oper == "+o") {
                msg = this->makeMsg("MODE " + ch + " " + oper + " " + nick);
                this->_channels[ch]->addOperator(*it);
            }
            else if (this->_channels[ch]->isOperator(*it) && oper == "-o") {
                msg = this->makeMsg("MODE " + ch + " " + oper + " " + nick);
                this->_channels[ch]->removeOperator(*it);
            }
            this->writeClient(msg);
            this->_channels[ch]->broadcast(msg, this);
        }
    }
}

void Client :: ModeI(vector<string> params, string oper) {
    string ch = params[0];

    string msg = this->makeMsg("MODE " + ch + " " + oper);

    if (!this->_channels[ch]->getInviteStatus() && oper == "+i")
        this->_channels[ch]->toggleInvite();
    else if (this->_channels[ch]->getInviteStatus() && oper == "-i")
        this->_channels[ch]->toggleInvite();
    this->writeClient(msg);
    this->_channels[ch]->broadcast(msg, this);
}

void Client :: ModeT(vector<string> params, string oper) {
    string ch = params[0];

    string msg = this->makeMsg("MODE " + ch + " " + oper);
    if (!this->_channels[ch]->getTopicStatus() && oper == "+t")
        this->_channels[ch]->toggleTopic();
    else if (this->_channels[ch]->getTopicStatus() && oper == "-t")
        this->_channels[ch]->toggleTopic();
    this->writeClient(msg);
    this->_channels[ch]->broadcast(msg, this);
}

void Client :: ModeL(vector<string> params, string oper) {
    string ch = params[0];
    string limit = (params.size() >= 3) ? params[2].c_str() : "";
    string msg = this->makeMsg("MODE " + ch + " " + oper + " " + limit);
    if ( oper == "+l" && params.size() >= 3)
        this->_channels[ch]->changeUserLimit(atoi(params[2].c_str()));
    else if (oper == "-l")
        this->_channels[ch]->changeUserLimit(-1);
    this->writeClient(msg);
    this->_channels[ch]->broadcast(msg, this);
}

void Client :: ModeK(vector<string> params, string oper) {
    string ch = params[0];
    string pass = params.size() >= 3 ? params[2] : "";

    string msg = this->makeMsg("MODE " + ch + " " + oper + " " + pass);
    if (!this->_channels[ch]->isProtected() && oper == "+k" && params.size() >= 3)
        this->_channels[ch]->toggleLock(pass);
    else if (this->_channels[ch]->isProtected() && oper == "-k")
        this->_channels[ch]->toggleLock();
    this->writeClient(msg);
    this->_channels[ch]->broadcast(msg, this);
}

void Client :: Modes(IrcMessage &msg) {
    if (!this->_accepted) {
        this->writeClient(NOTACC);
        return;
    }
    vector<string> params = msg.getParams();
    string ch = params[0];
    if (params.size() < 2) {
        return ;
    }

    string mode = params[1];
    
    if (this->_channels.find(ch) == this->_channels.end()) {
        string ret = ":ka100 403 " + ch + " :Channel not in Server\r\n";
        this->writeClient(ret); 
        return;
    }
    if (!this->_channels[ch]->isOperator(this)) {
        string err = ":ka100 482 " + this->_nick + " " + ch + " :You're not a channel operator!!\r\n";
        this->writeClient(err);
        return ;
    }
    if (this->_channels.find(ch) == this->_channels.end()) {
        string err = "Error :No such Channel" + ch + "\r\n";
        this->writeClient(err);
        return ;
    }

    string oper = processString(mode);
    if (modes.find(oper) == modes.end())
        return ;
    (this->*modes[oper])(params, oper);
    this->refresh(ch);
}