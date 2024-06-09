#include <Server.hpp>
#include <Channel.hpp>

Server :: Server():  _port(), _pass(), _conn() {}
void Server :: stop() { this->run = false;}
void Server :: start() { this->run = true;}
string Server :: getPass() const { return this->_pass; }
map<int, Client *> Server :: getClients() { return _conn.getClients();}

Server :: ~Server() {
    this->_conn.closeConnecions();
    for (map<string, Channel *>::iterator it = _channels.begin(); it != _channels.end(); ++it)
    {
        cout << "Delete Channel: " << it->second->getName() << endl;
        delete it->second;
    }    
    _channels.clear();
}

Server :: Server(string const &port, string const &pass): _port(port), _pass(pass), run(true), _conn() {

    // check pass and port
    this->run = true;
    if (port.length() == 0 || pass.length() == 0)
        throw runtime_error("Paramters are not valid");
    // creating server socket
    int SocketServer = socket(AF_INET, SOCK_STREAM, 0);
    int yes = 1;
    if (SocketServer == -1)
        throw runtime_error("Socket initialization went wrong");
    // custom options
    setsockopt(SocketServer, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int));
    // prepare the process with ip:port
    this->_conn.sI.sin_family = AF_INET;
    this->_conn.sI.sin_addr.s_addr = INADDR_ANY;
    this->_conn.sI.sin_port = htons(atoi(port.c_str()));
    if (bind(SocketServer, (sockaddr *)&this->_conn.sI, sizeof(this->_conn.sI)) == -1)
        throw runtime_error("Bind System went wrong!");
    if (listen(SocketServer, FOPEN_MAX) == -1)
        throw runtime_error("Listen System went wrong!");
    _conn.addClient(SocketServer, this);
}

Channel *Server :: getChannel(string name) {
    if (this->_channels.find(name) == this->_channels.end())
        return NULL;
    return this->_channels[name];
}

bool Server :: serverLoop() {
    string buff;
    while (this->run) {
        int events = poll(this->_conn.connectionSockets.data(), this->_conn.numClients, -1);
        while (events > 0) {
            if (this->_conn.connectionSockets[0].revents & POLLIN) {
                socklen_t addrlen = sizeof(this->_conn.sI);
                int newClient = accept(this->_conn.connectionSockets[0].fd, (sockaddr *)&this->_conn.sI, &addrlen);
                if (newClient == -1)
                    return false;
                this->_conn.addClient(newClient, this);
            }
            for (map<int, Client *> :: iterator it = this->_conn.connectionList.begin(); it != this->_conn.connectionList.end(); it++) {
                if(it->second->toDisconnect == true)
                {
                    this->_conn.delClient(it->second);
                    break;
                }
                int readResult = ((it)->second)->readClient(buff);
                if (readResult == 1)
                {
                    this->_conn.delClient(it->second);
                    break;
                }
            }
            events--;
        }
    }
    return false;
}

void Server :: deleteClient(Client *client){
    map <int, Client *> clients = this->getClients();
    for (unsigned int i = 3; i < clients.size() + 3; i++){
        if (clients[i] == client){
            close(client[i].getPoll()->fd);
            delete clients[i]; // Delete the client object
            clients.erase(i);
            break;
        }
    }
}

Channel *Server :: addChannel(string name) {
    if (this->_channels.find(name) != this->_channels.end())
        return NULL;
    Channel *newChannel = new Channel(name);
    this->_channels[name] = newChannel;
    return newChannel;
}
