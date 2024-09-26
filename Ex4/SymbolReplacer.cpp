#include <iostream>

std::string *
ReplaceSymbols(std::string &String)
{
    std::string *NewString = new std::string;

    for(char c : String) {
        switch(c) {
            case '&':
                *NewString += "&amp;";
                break;
            case '<':
                *NewString += "&lt;";
                break;
            case '>':
                *NewString += "&gt;";
                break;
            default:
                *NewString += c;
                break;
        }
    }

    return NewString;
}

int
main(int ArgCount, char **ArgV)
{
    std::string InputString(ArgV[1]);
    std::cout << *ReplaceSymbols(InputString) << std::endl;
}
