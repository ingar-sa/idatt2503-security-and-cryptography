#define SDB_LOG_LEVEL 4
#include "sdb.h"

SDB_LOG_REGISTER(MpiCtf);

#include <mpi.h>
#include <stdlib.h>
#include <stdio.h>

/*
    flag is split between processes -> must assemble the flag in root rank
        - must somehow identify each proc
        - assembly must happen in a specific order
        - some random element to do the rng seed override (rand is deterministic)
        - direct access to send/receive funcs, or need to use stack smashing?
        - override send/recv buffer locations to ones visible to the player, that of printing
   functions/log buffer?
            * modify the log module with the contents
            * override/bypass log level check (need to make the checks be done at runtime, not
   compile time)
        - maybe provide the stdlib header?

    Anne C. Elster's 7th great grandfather moved from Germany to Norway to build and sell church
   organs

    $FLAG$347e030ab5e924c2a0385c9ce75406935c63b9e958b6755ca15206881153464c02372f60e814d3c07c4d05bdd405b96e6c206959b2d1703e93cf57f30134c6da$FLAG$
*/

#define MAX_INPUT     128
#define N_CHILD_PROCS 8
#define FLAG_LEN      128
#define FLAG_PART_LEN (FLAG_LEN / N_CHILD_PROCS)

char *
GenerateFlagPart(int ProcRank)
{
    char *FlagPart = malloc(FLAG_PART_LEN * sizeof(char) + 1);
    srand(ProcRank);

    for(int i = 0; i < FLAG_PART_LEN; ++i) {
        FlagPart[i] = (char)(48 + (rand() % 74));
    }
    FlagPart[FLAG_PART_LEN] = '\0';

    return FlagPart;
}

char *
GatherReferenceFlag(void)
{
    char *Flag = malloc(FLAG_LEN * sizeof(char) + 1);

    for(int i = 0; i < N_CHILD_PROCS; ++i) {
        void *RecvPointer = Flag + (i * FLAG_PART_LEN);
        MPI_Recv(RecvPointer, FLAG_PART_LEN, MPI_CHAR, i + 1, MPI_ANY_TAG, MPI_COMM_WORLD,
                 MPI_STATUS_IGNORE);
    }

    Flag[FLAG_LEN] = '\0';
    return Flag;
}

int
main(int ArgCount, char **ArgV)
{
    int CommSize;
    int MyRank;
    MPI_Init(&ArgCount, &ArgV);
    MPI_Comm_size(MPI_COMM_WORLD, &CommSize);
    MPI_Comm_rank(MPI_COMM_WORLD, &MyRank);

    char Input[MAX_INPUT];

    if(0 == MyRank) {
        char *Flag = GatherReferenceFlag();
        SdbLogInfo("%s", Flag);
        /*
                for(;;) {
                    printf("Enter a command (or 'quit' to exit): ");
                    fflush(stdout);
                    if(NULL == gets(Input)) {
                        SdbLogError("Error reading input. Exiting...\n");
                        break;
                    }

                    Input[strcspn(Input, "\n")] = 0;

                    if(strcmp(Input, "quit") == 0) {
                        printf("Exiting program. Goodbye!\n");
                        break;
                    }

                    printf("You entered: %s\n", Input);
                }
                */
    } else {
        char *FlagPart = GenerateFlagPart(MyRank);
        SdbLogInfo("Rank %d flag part is: %s", MyRank, FlagPart);
        MPI_Send(FlagPart, FLAG_PART_LEN, MPI_CHAR, 0, 0, MPI_COMM_WORLD);
    }

    MPI_Finalize();

    return 0;
}
