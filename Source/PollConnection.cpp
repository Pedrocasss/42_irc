#include <PollConnection.hpp>
#include <Server.hpp>

PollConnection :: PollConnection() : connectionList(), connectionSockets(), numClients(0){};
vector<pollfd> PollConnection::getPoll() const { return this->connectionSockets; } 
map<int, Client *> PollConnection::getClients() const { return connectionList; }
int PollConnection::getSize() const { return connectionList.size(); }

void PollConnection :: addClient(int socket, Server *serv) {
    Client *s = new Client(socket, serv);
    this->connectionList[socket] = s;
    this->connectionSockets.push_back(*s->getPoll());
    this->numClients++;
}

void PollConnection :: delClient(Client *c) {
    this->numClients--;
    int fd = c->socket.fd;
    if(!c)
        return ;
    cout << "Delete Specific client" << c->getNick() << endl;
    delete this->connectionList.at(c->getPoll()->fd);
    this->connectionList.erase(fd);
    for(vector<pollfd> :: iterator it = this->connectionSockets.begin(); it != this->connectionSockets.end(); it++)
    {
        if((*it).fd == fd)
        {
            this->connectionSockets.erase(it);
            return;
        }
    }
}

pollfd PollConnection :: GetSpecifPoll(int index) {
    if (index > this->getSize())
        throw runtime_error("The index is bigger that the max");
    return this->connectionSockets[index];
}

Client *PollConnection :: GetSpecifClient(int index) {
    if (index > this->getSize())
        throw runtime_error("The index is bigger that the max");
    return this->operator[](index);
}


void PollConnection :: closeConnecions() {

    try {
            std::map<int, Client*>::iterator it = this->connectionList.begin();
            while (it != this->connectionList.end()) {
                if (it->second) {
                    std::cout << "Server Closing Deleting Client: " << it->second->getName() << std::endl;
                    delete it->second;
                    it->second = NULL; // Optional: to explicitly set the pointer to NULL
                }
                // Get the next iterator before erasing
                std::map<int, Client*>::iterator itToErase = it;
                ++it; // Increment the iterator before erasing
                this->connectionList.erase(itToErase); // Erase the element
            }
        } catch (std::exception &e) {
            // Handle exception, maybe log the error
            std::cerr << "Exception occurred: " << e.what() << std::endl;
            return;
        }
}

Client * PollConnection :: operator[](size_t index) {
    if (index >= this->connectionList.size())
        throw out_of_range("Index out of range\n");
    return this->connectionList[index];
}


ostream &operator<<(ostream &os, PollConnection const &other) {
    os << "Number of Clients: " << other.getClients().size() << endl;
    os << "Number of Polls: " << other.getPoll().size() << endl;
    return os;
}