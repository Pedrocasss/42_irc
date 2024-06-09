#pragma once
#include "IrcMainHeader.hpp"

using namespace std;
class IrcMessage
{
    private:
        string                  _type;
        vector<string>     _params;

    public:
        string _list;

        IrcMessage(const string &msg);
        ~IrcMessage();

        string          getCommand();
        vector<string>  getParams();

};