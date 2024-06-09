#include <IrcMainHeader.hpp>
#include <Server.hpp>

Server *irc = NULL;
int MIN_PORT = 1024;
int MAX_PORT = 65535;

void closeSignal(int sig) {
    if(!irc)
        cout << "There is no server to shutdown\n";
    else if (sig == SIGINT){
        irc->stop();
    }
}

int parseArgs(char **av) {
    string port = av[1];
    string pass = av[2];

    if (port.empty() || pass.empty())
        throw runtime_error("Port and password must be provided");
    if (port.find_first_not_of("0123456789") != string::npos)
        throw runtime_error("Port must be a number");
    if (atoi(av[1]) < MIN_PORT || atoi(av[1]) > MAX_PORT) {
        throw runtime_error("Port must be between 1024 and 65535");
    }
    return 1;
}

int main(int ac, char **av) {
    if (ac != 3) {
        cout << "Usage: ./ircserv <port> <password>\n";
        return 1;
    }

    try {
        parseArgs(av);
    } catch (const exception &e) {
        cerr << e.what() << endl;
        return 1;
    }
    irc = new Server(av[1], av[2]);
    signal(SIGINT, closeSignal);
    irc->serverLoop();
    cout << "Stop\n";
    delete irc;
}