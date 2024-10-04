#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

char *ReplaceSymbols(char *String) {
  int StrLen = 0;
  int NewStrLen = 0;
  char *StringAlias = String;

  while (*StringAlias != '\0') {
    switch (*StringAlias) {
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

  char *NewString = malloc(NewStrLen + 1);
  int NewStringIdx = 0;

  for (int i = 0; i <= StrLen; ++i) {
    switch (String[i]) {
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

int LLVMFuzzerTestOneInput(const uint8_t *data, size_t size) {
  (void)size;
  char *NewString = ReplaceSymbols((char *)data);
  return 0;
}
