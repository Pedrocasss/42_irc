#include <Bot.hpp>

Bot :: Bot() {
    this->botfns["rps"] = &Bot :: RPS;
    this->botfns["mimic"] = &Bot :: Mimic;
}

Bot :: ~Bot() {}

string Bot :: Run(vector<string> args) {
    if (this->botfns.find(args[0]) != this->botfns.end())
        return (this->*(this->botfns[args[0]]))(args);
    return "Option not available yet\r\n";
}

string Bot :: RPS(vector<string> args) {
    int index = 0;
    string choices[] = {"rock", "paper", "scissors"};
    for (int i = 0; i < 3; i++) {
        if (choices[i] == args[1]){
            index = i;
            break;
        }
        if (i == 2)
            return ("BOT RPS: Invalid choice\r\n");
    }
    string ret = "=======================================\n";
    ret += "Bot chose: " + choices[(index + 1) % 3] + "\n";
    ret += "You chose: " + args[1] + "\n";
    ret += "You " + string((choices[(index + 1) % 3] == args[1]) ? "win" : "lose") + "\n";
    ret += "=======================================\r\n";
    return ret;
}

string Bot :: Mimic(vector<string> args) {
    string msg;

    for (unsigned long i = 1; i < args.size(); i++)
        msg += " " + args[i];
    string ret = "Bot says:" + msg + "\r\n";
    return ret;
}