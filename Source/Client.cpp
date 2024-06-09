#include <Client.hpp>
#include <Channel.hpp>
#include <Bot.hpp>
#include <Server.hpp>

string NOTACC = "ERROR :You have not registered\r\n";
string WRONGARGS = "ERROR :WRONG ARGUMENTS\r\n";

const string &Client ::getName() const { return this->_user; }
const string &Client ::getPass() const { return this->_pass; }
const string &Client ::getNick() const { return this->_nick; }
pollfd *Client ::getPoll() { return &this->socket; }
void Client ::setPass(const string &newPass) { this->_pass = newPass; }
void Client ::setAccept(bool value) { this->_accepted = value; }
bool Client ::getAccept() { return this->_accepted; }

void Client ::parseMessage(IrcMessage &msg)
{
    string command = msg.getCommand();
    vector<string> params = msg.getParams();

    if (params.empty() || command.empty())
        return;
    if (fns.find(command) == fns.end())
        return;
    (this->*fns[command])(msg);
}

void Client ::checkLogin()
{
    if (this->_nick == "\1" || this->_nick == "" || this->_user == "" || this->_pass != this->_serverPass)
        return;

    string ret = "Welcome " + this->_nick + "!" + this->_user + "@localhost\r\n";
    this->writeClient(ret);

    this->_accepted = true;
}

void Client ::Pass(IrcMessage &msg)
{
    vector<string> params = msg.getParams();
    this->_pass = params[0];

    if (params.size() < 1)
    {
        this->writeClient(WRONGARGS);
        return;
    }
    if (params.size() > 1)
        for (unsigned long i = 1; i < params.size(); i++)
            this->_pass += " " + params[i];
    if (this->_pass != this->_serverPass)
    {
        string retmsg = "ERROR :Invalid password\r\n";
        this->writeClient(retmsg);
        return;
    }
    checkLogin();
}

void Client ::Nick(IrcMessage &msg)
{
    vector<string> params = msg.getParams();
    string err = "";
    if (this->_oldnick.empty())
        this->_oldnick = params[0];

    string ret = ":" + this->_oldnick + "!" + this->_user + "@localhost " + "NICK :" + params[0] + "\r\n";
    this->writeClient(ret);

    if (params.size() < 1)
    {
        this->writeClient(WRONGARGS);
        return;
    }

    map<int, Client *> clients = this->_server->getClients();
    for (unsigned long i = 3; i < clients.size() + 3; i++)
    {
        if (clients[i]->getNick() == (params[0]))
        {
            err = ":ka100 433 " + params[0] + " :Nick already in use\r\n";
            this->writeClient(err);
            this->_oldnick = params[0];
            this->_nick = "\1";
            return;
        }
    }
    this->_nick = params[0];
    this->_oldnick = this->_nick;
    checkLogin();
}

void Client ::User(IrcMessage &msg)
{
    vector<string> params = msg.getParams();

    if (params.size() < 1)
    {
        this->writeClient(WRONGARGS);
        return;
    }
    this->_user = params[0];
    checkLogin();
}

void Client ::Join(IrcMessage &msg)
{

    if (!this->_accepted)
    {
        this->writeClient(NOTACC);
        return;
    }

    vector<string> params = msg.getParams();
    string ch = params[0];
    string ret;

    if (params.size() < 1)
    {
        this->writeClient(WRONGARGS);
        return;
    }
    if (ch[0] != '#')
    {
        string err = ch + ": No such channel\r\n";
        this->writeClient(err);
        return;
    }

    this->addChannel(ch);
    if (this->_channels[ch]->getInviteStatus() && !this->_channels[ch]->isInvited(this))
    {
        ret = ":ka100 473 " + this->_nick + " " + ch + " :Cannot join channel (Invite only)\r\n";
        this->writeClient(ret);
        return;
    }
    if (this->_channels[ch]->isFull())
    {
        ret = ":ka100 471 " + this->_nick + " " + ch + " :Cannot join channel (Channel is full)\r\n";
        this->writeClient(ret);
        return;
    }
    if (this->_channels[ch]->isProtected())
    {
        if (params.size() < 2 || (params[1]) != this->_channels[ch]->getPass())
        {
            ret = ":ka100 475 " + this->_nick + " " + ch + " :Cannot join channel (Incorrect channel key)\r\n";
            this->writeClient(ret);
            return;
        }
    }
    this->_channels[ch]->addClient(this);
    if (this->_channels[ch]->isInvited(this))
        this->_channels[ch]->removeInvited(this);
    if (this->_channels[ch]->getClients().size() == 1)
        this->_channels[ch]->addOperator(this);
    ret = this->makeMsg(msg._list);
    this->writeClient(ret);
    this->refresh(ch);
}

void Client ::Priv(IrcMessage &msg)
{
    if (!this->_accepted)
    {
        this->writeClient(NOTACC);
        return;
    }
    vector<string> params = msg.getParams();

    if (params.size() < 2)
    {
        this->writeClient(WRONGARGS);
        return;
    }
    if (params[0][0] == '#')
    {
        string ch = params[0];
        if (this->_channels.find(ch) == this->_channels.end())
        {
            string ret = ":ka100 403 " + ch + " :Channel not in Server\r\n";
            this->writeClient(ret);
            return;
        }
        string ret = this->makeMsg(msg._list);
        this->_channels[ch]->broadcast(ret, this);
        return;
    }
    map<int, Client *> clientela = this->_server->getClients();
    for (unsigned long i = 3; i < clientela.size() + 3; i++)
    {
        if (clientela[i]->getNick() == (params[0]))
        {
            string ret = this->makeMsg(msg._list);
            clientela[i]->writeClient(ret);
            break;
        }
        if (i == clientela.size() + 2)
        {
            string ret = ":ka100 401 " + this->_nick + " :Nick not in Server\r\n";
            this->writeClient(ret);
        }
    }
}

void Client ::Who(IrcMessage &msg)
{
    if (!this->_accepted)
    {
        this->writeClient(NOTACC);
        return;
    }
    vector<string> params = msg.getParams();

    if (this->_channels.find(params[0]) == this->_channels.end())
        return;
    this->refresh(params[0]);
    if (this->_channels[params[0]]->getTopic() != "")
    {
        string ch = params[0];
        string topic = ":ka100 332 " + this->_nick + " " + ch + " :" + this->_channels[params[0]]->getTopic() + "\r\n";
        this->writeClient(topic);
    }
}

void Client ::Part(IrcMessage &msg)
{
    if (!this->_accepted)
    {
        this->writeClient(NOTACC);
        return;
    }
    vector<string> params = msg.getParams();

    if (params.size() < 2)
    {
        this->writeClient(WRONGARGS);
        return;
    }
    string ch = params[0];
    if (this->_channels.find(ch) == this->_channels.end())
    {
        string ret = ":ka100 403 " + ch + " :Channel not in Server\r\n";
        this->writeClient(ret);
        return;
    }
    vector<Client *> &clients = this->_channels[ch]->getClients();
    for (vector<Client *>::iterator it = clients.begin(); it != clients.end();)
    {
        if (*it == this)
            it = clients.erase(it);
        else
            it++;
    }
    this->refresh(ch);
    string ret = this->makeMsg(msg._list);
    this->writeClient(ret);
    this->_channels.erase(ch);
}

void Client ::Quit(IrcMessage &msg)
{

    if (msg.getParams().size() < 1)
    {
        this->writeClient(WRONGARGS);
        return;
    }

    for (map<string, Channel *>::iterator it = this->_channels.begin(); it != this->_channels.end(); ++it)
    {
        // Remove this client from the channel's client list
        vector<Client *> &clients = it->second->getClients();
        for (vector<Client *>::iterator clientIt = clients.begin(); clientIt != clients.end();)
        {
            if (*clientIt == this)
                clientIt = clients.erase(clientIt);
            else
                clientIt++;
        }
        string ch = it->first;
        this->refresh(ch);
        it->second->removeClient(this);
    }
    this->_channels.clear();
    this->toDisconnect = true;
    close(this->socket.fd);
    // this->_server->deleteClient(this);

    // this->getPoll()->fd = -1;
}

void Client ::Kick(IrcMessage &msg)
{
    if (!this->_accepted)
    {
        this->writeClient(NOTACC);
        return;
    }

    vector<string> params = msg.getParams();
    string ch = params[0];
    string nick = params[1];
    string err;
    if (params.size() < 3)
    {
        this->writeClient(WRONGARGS);
        return;
    }
    if (this->_channels.find(ch) == this->_channels.end())
    {
        string ret = ":ka100 403 " + ch + " :Channel not in Server\r\n";
        this->writeClient(ret);
        return;
    }
    if (!this->_channels[ch]->isOperator(this))
    {
        err = ":ka100 482 " + this->_nick + " " + ch + " :You're not a channel operator!!\r\n";
        this->writeClient(err);
        return;
    }
    map<int, Client *> clientela = this->_server->getClients(); // all clients
    for (unsigned long i = 3; i < clientela.size() + 3; i++)
    {
        if (clientela[i]->getNick() == (nick))
        {
            if (clientela[i]->_channels.find(ch) == clientela[i]->_channels.end())
            {
                err = ":ka100 441 " + nick + " " + ch + " :They aren't on that channel\r\n";
                this->writeClient(err);
                return;
            }
            string ret = this->makeMsg(msg._list);
            clientela[i]->writeClient(ret);

            this->writeClient(ret);
            this->_channels[ch]->broadcast(ret, this);
            this->_channels[ch]->removeClient(clientela[i]);
            if (this->_channels[ch]->isOperator(clientela[i]) == true)
                this->_channels[ch]->removeOperator(clientela[i]);
            this->refresh(ch);
            clientela[i]->_channels.erase(ch);
            break;
        }
        if (i == clientela.size() + 2)
        {
            string ret = ":ka100 401 " + this->_nick + " :Nick not in Server\r\n";
            this->writeClient(ret);
        }
    }
}

void Client ::Topic(IrcMessage &msg)
{
    if (!this->_accepted)
    {
        this->writeClient(NOTACC);
        return;
    }

    vector<string> params = msg.getParams();
    string ch = params[0];
    if (this->_channels.find(ch) == this->_channels.end())
    {
        string ret = ":ka100 403 " + ch + " :Channel not in Server\r\n";
        this->writeClient(ret);
        return;
    }
    if (!this->_channels[ch]->isOperator(this) && this->_channels[ch]->getTopicStatus())
    {
        string err = ":ka100 482 " + this->_nick + " " + ch + " :You're not a channel operator!!\r\n";
        this->writeClient(err);
        return;
    }
    string topic = &params[1][1];
    if (params.size() > 2)
        for (unsigned long i = 2; i < params.size(); i++)
            topic += " " + params[i];

    this->_channels[ch]->setTopic(topic);
    string ret = this->makeMsg(msg._list);
    this->writeClient(ret);
    this->_channels[ch]->broadcast(ret, this);
}

void Client ::Invite(IrcMessage &msg)
{
    if (!this->_accepted)
    {
        this->writeClient(NOTACC);
        return;
    }

    vector<string> params = msg.getParams();

    if (params.size() <= 1)
        return;
    string inv = params[0];
    string ch = params[1];
    if (this->_channels.find(ch) == this->_channels.end())
    {
        string ret = ":ka100 403 " + ch + " :Channel not in Server\r\n";
        this->writeClient(ret);
        return;
    }
    if (!this->_channels[ch]->isOperator(this))
    {
        string err = ":ka100 482 " + this->_nick + " " + ch + " :You're not a channel operator!!\r\n";
        this->writeClient(err);
        return;
    }
    string INVITING = ":ka100 341 " + this->_nick + " " + inv + " " + ch + "\n";
    this->writeClient(INVITING);
    map<int, Client *> clientela = this->_server->getClients();
    for (unsigned long i = 3; i < clientela.size() + 3; i++)
    {
        if (clientela[i]->getNick() == inv)
        {
            this->_channels[ch]->addInvited(clientela[i]);
            string ret = this->makeMsg(msg._list);
            clientela[i]->writeClient(ret);
            break;
        }
        if (i == clientela.size() + 2)
        {
            string ret = ":ka100 401 " + this->_nick + " :Nick not in Server\r\n";
            this->writeClient(ret);
        }
    }
}

void Client ::refresh(string ch)
{
    string NAMREPLY = ":ka100 353 " + this->_nick + " = " + ch + " :";

    if (this->_channels.find(ch) == this->_channels.end())
    {
        string ret = ":ka100 403 " + ch + " :Channel not in Server\r\n";
        this->writeClient(ret);
        return;
    }

    for (vector<Client *>::iterator it = this->_channels[ch]->getClients().begin(); it != this->_channels[ch]->getClients().end(); ++it)
        NAMREPLY += ((*it)->_channels[ch]->isOperator(*it) ? "@" + (*it)->getNick() : (*it)->getNick()) + " ";

    NAMREPLY += "\r\n";
    string ENDOFNAMES = ":ka100 366 " + this->_nick + " " + ch + " :End of /WHO list.\r\n";
    string rep = NAMREPLY + ENDOFNAMES;

    this->writeClient(rep);
    this->_channels[ch]->broadcast(rep, this);
}

void Client ::BotMsg(IrcMessage &msg)
{
    vector<string> params = msg.getParams();
    if (params.size() < 2)
    {
        this->writeClient(WRONGARGS);
        return;
    }

    string ret = ":ka100 300 msg " + this->bot->Run(params);
    this->writeClient(ret);
}

string Client ::makeMsg(string str)
{
    string ret = ":" + this->_nick + "!" + this->_user + " ";
    ret.append(str + "\r\n");
    return ret;
}

Client ::Client(int socket, Server *host) : _pass(), _nick(), _user(), socket()
{
    this->_server = host;
    this->_serverPass = host->getPass();
    this->socket.fd = socket;
    this->socket.events = POLLIN;
    this->toDisconnect = false;
    this->_accepted = false;
    this->bot = new Bot();

    this->fns["JOIN"] = (&Client ::Join);
    this->fns["PASS"] = (&Client ::Pass);
    this->fns["NICK"] = (&Client ::Nick);
    this->fns["USER"] = (&Client ::User);
    this->fns["PRIVMSG"] = (&Client ::Priv);
    this->fns["WHO"] = (&Client ::Who);
    this->fns["PART"] = (&Client ::Part);
    this->fns["KICK"] = (&Client ::Kick);
    this->fns["TOPIC"] = (&Client ::Topic);
    this->fns["INVITE"] = (&Client ::Invite);
    this->fns["QUIT"] = (&Client::Quit);
    this->fns["MODE"] = (&Client::Modes);
    this->fns["BOT"] = (&Client::BotMsg);

    this->modes["+o"] = (&Client::ModeO);
    this->modes["-o"] = (&Client::ModeO);
    this->modes["+i"] = (&Client::ModeI);
    this->modes["-i"] = (&Client::ModeI);
    this->modes["+t"] = (&Client::ModeT);
    this->modes["-t"] = (&Client::ModeT);
    this->modes["+l"] = (&Client::ModeL);
    this->modes["-l"] = (&Client::ModeL);
    this->modes["+k"] = (&Client::ModeK);
    this->modes["-k"] = (&Client::ModeK);
}

int Client ::readClient(string &buff)
{
    char buf[2048];
    int wSize = 0;

    if (fcntl(this->socket.fd, F_SETFL, O_NONBLOCK) < 0)
    {
        return 0;
    }
    while (true)
    {
        buff.clear();
        memset(buf, 0, sizeof(buf)); // Clear the buffer after each read
        wSize = recv(this->socket.fd, buf, 2047, MSG_DONTWAIT);
        if (wSize <= 0)
            break;
        buff.append(buf, wSize);
        istringstream iss(buff);
        string line;
        while (getline(iss, line))
        {
            IrcMessage *msg = new IrcMessage(line);
            parseMessage(*msg);
            delete msg;
        }
    }
    if (wSize == 0)
        return 1; // Client disconnected
    else if (wSize == -1)
        return -1; // Error occurred while reading
    return 0;      // Read operation completed successfully
}

int Client ::writeClient(string &toSend)
{
    const char *buf = toSend.c_str(); // Get a pointer to the string's data
    int totalSent = 0;
    int remaining = toSend.length(); // Total bytes remaining to send

    if (fcntl(this->socket.fd, F_SETFL, O_NONBLOCK) > 0)
    {
        close(this->socket.fd);
        return -1;
    }
    while (remaining > 0)
    {
        int sent = send(this->socket.fd, buf + totalSent, remaining, 0);
        if (sent == -1)
            return -1;
        totalSent += sent;
        remaining -= sent;
    }
    return totalSent; // Return total bytes sent
}

Channel *Client ::addChannel(string channel)
{
    Channel *ch = this->_server->addChannel(channel);

    this->_channels[channel] = this->_server->getChannel(channel);
    return ch;
}

void Client ::removeChannel(const string &channelName)
{
    if (_channels.find(channelName) != _channels.end())
    {
        _channels[channelName]->removeClient(this);
        _channels.erase(channelName);
    }
}

Client ::~Client()
{
    cout << "This " << this->_nick << "is being Destroyed" << endl;
    map<string, Channel *>::iterator ms = this->_channels.begin();
    while (ms != this->_channels.end())
    {
        ms->second->removeClient(this);
        this->refresh(ms->first);
        ms++;
    }
    delete this->bot;
    close(this->socket.fd);
}