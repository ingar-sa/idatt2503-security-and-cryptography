#include <stdlib.h>
#include <stdio.h>

char *
ReplaceSymbols(char *String)
{
    int   StrLen      = 0;
    int   NewStrLen   = 0;
    char *StringAlias = String;

    while(*StringAlias != '\0') {
        switch(*StringAlias) {
            case '&':
                NewStrLen += 5;
                break;
            case '<':
                NewStrLen += 4;
                break;
            case '>':
                NewStrLen += 4;
                break;
            default:
                ++NewStrLen;
                break;
        }

        ++StrLen;
        ++StringAlias;
    }

    char *NewString    = malloc(NewStrLen + 1);
    int   NewStringIdx = 0;

    for(int i = 0; i <= StrLen; ++i) {
        switch(String[i]) {
            case '&':
                NewString[NewStringIdx++] = '&';
                NewString[NewStringIdx++] = 'a';
                NewString[NewStringIdx++] = 'm';
                NewString[NewStringIdx++] = 'p';
                NewString[NewStringIdx++] = ';';
                break;
            case '<':
                NewString[NewStringIdx++] = '&';
                NewString[NewStringIdx++] = 'l';
                NewString[NewStringIdx++] = 't';
                NewString[NewStringIdx++] = ';';
                break;
            case '>':
                NewString[NewStringIdx++] = '&';
                NewString[NewStringIdx++] = 'g';
                NewString[NewStringIdx++] = 't';
                NewString[NewStringIdx++] = ';';
                break;
            default:
                NewString[NewStringIdx++] = String[i];
                break;
        }
    }

    NewString[NewStrLen] = '\0';

    return NewString;
}

int
main(int ArgCount, char **ArgV)
{
    char *NewString = ReplaceSymbols(ArgV[1]);
    printf("%s\n", NewString);
}
