undefined8 main(void)
{
  char local_28 [32];
  
  printf("Input: ");
  fflush((FILE *)0x0);                  // Flushes all output streams
  gets(local_28);                       // Gets stdin into local_28
  printf("Hello %s\n",local_28);        // Correctly uses printf to print a string
  return 0;
}

                                        // address of flag from nm 4198802 (base 10)
void flag(void)
{
  size_t sVar1;                         // used for strlen
  undefined8 uStack_a0;                 // program counter for return instruction after function calls
  char local_98 [136];                  // buffer for contents of flag.txt
  FILE *local_10;                       // fd for flag.txt
  
  uStack_a0 = 0x4011b0;
  local_10 = fopen("flag.txt","r");     // local_10 is fd to flag.txt in read mode

  uStack_a0 = 0x4011d4;
  fread(local_98,0x80,1,local_10);      // reads 128 bytes from flag.txt into local_98

  uStack_a0 = 0x4011e3;
  sVar1 = strlen(local_98);             // strlen of flag.txt

  if (local_98[sVar1 - 1] == '\n') {    // if the final character of the string is \n
    uStack_a0 = 0x401202;
    sVar1 = strlen(local_98);           // use strlen again ???

    local_98[sVar1 - 1] = '\0';         // replace \n with \0
  }

  uStack_a0 = 0x40121d;
  puts(local_98);                       // writes null-terminated string + \n to stdout

  uStack_a0 = 0x40122c;
  fflush(stdout);                       // flushes all data written to stdout

  uStack_a0 = 0x401238;
  fclose(local_10);                     // closes flag.txt
  return;
}
