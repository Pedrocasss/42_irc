#include "IrcMessage.hpp"

#include <sstream>

std :: string IrcMessage :: getCommand() { return this->_type; }
std :: vector<std :: string> IrcMessage :: getParams() { return this->_params; }
IrcMessage::~IrcMessage(){}

vector<string> splitString(const string &str) {
    vector<string> result;
    istringstream iss(str);
    string token;

    while (getline(iss, token, ' ') || getline(iss, token, '\n'))
        result.push_back(token);
    return result;
}

IrcMessage :: IrcMessage(const string &msg) : _list(msg){
    string _cmd;
    string _par;

    if (_list.empty() || _list == ".\r\n") 
        return ;

    this->_list.erase(remove(this->_list.begin(), this->_list.end(), '\r'), this->_list.end());
    vector<string> tokens = splitString(_list);
    this->_type = tokens[0];
    this->_params.assign(tokens.begin() + 1, tokens.end()); // Assign the tokens vector from the second position to the end to _params
}
