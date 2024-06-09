#ifndef BOT_HPP
#define BOT_HPP

#include <IrcMainHeader.hpp>

using namespace std;

class Bot{
    public:
        Bot();
        ~Bot();
        string Run(vector<string>);

    private:
        map<string, string(Bot ::*)(vector<string>)> botfns;

        string  RPS(vector<string>);
        string  Mimic(vector<string>);
};

#endif