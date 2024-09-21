#include "sdb.h"
#include <string.h>

SDB_LOG_REGISTER(MpiCtf);

#include <mpi.h>
#include <stdlib.h>
#include <stdio.h>

#define MAX_INPUT     32
#define N_CHILD_PROCS 8
#define FLAG_LEN      128
#define FLAG_PART_LEN (FLAG_LEN / N_CHILD_PROCS)

static u64 SDB_LOG_LEVEL;

char *
GenerateFlagPart(void)
{
    char *FlagPart = malloc(FLAG_PART_LEN * sizeof(char) + 1);

    for(int i = 0; i < FLAG_PART_LEN; ++i) {
        FlagPart[i] = (char)(48 + (rand() % 75));
    }

    FlagPart[FLAG_PART_LEN] = '\0';

    return FlagPart;
}

char *
AssembleFlag(i64 Seed)
{
    char *Flag = malloc(FLAG_LEN * sizeof(char) + 1);

    int RanksRecvd[N_CHILD_PROCS] = { 0 };
    int NRanksRecvd               = 0;
    u64 LogLevel                  = 4;
    SDB_LOG_LEVEL                 = LogLevel;

    srand(Seed);

    SdbLogDebug("Assembling flag.");

    for(; NRanksRecvd < N_CHILD_PROCS;) {
        int  Rank              = 1 + (rand() % N_CHILD_PROCS);
        bool RankFlagPartRecvd = false;

        for(int i = 0; i < NRanksRecvd; ++i) {
            RankFlagPartRecvd = (RanksRecvd[i] == Rank);
            if(RankFlagPartRecvd) {
                break;
            }
        }

        if(!RankFlagPartRecvd) {
            RanksRecvd[NRanksRecvd] = Rank;
            void *RecvPointer       = Flag + (NRanksRecvd * FLAG_PART_LEN);
            MPI_Recv(RecvPointer, FLAG_PART_LEN, MPI_CHAR, Rank, MPI_ANY_TAG, MPI_COMM_WORLD,
                     MPI_STATUS_IGNORE);

            NRanksRecvd += 1;
        }
    }

    Flag[FLAG_LEN] = '\0';

    SdbLogDebug("Flag assembled!");

    return Flag;
}

void
RootRank(i64 RankOrder)
{
    u64  LogLevel = 2;
    i64  Seed     = 1872940238;
    char Input[MAX_INPUT];

    printf("Hello! Do you want the flag (yes/no)?\n");

    if(NULL == gets(Input)) {
        SdbLogError("Error reading input. Exiting...");
        exit(EXIT_FAILURE);
    }

    Input[strcspn(Input, "\n")] = 0;
    Input[MAX_INPUT - 1]        = '\0';

    SDB_LOG_LEVEL = LogLevel;
    if(0 == strcmp(Input, "yes")) {
        SdbLogDebug("Note to self: the correct order is %ld!", RankOrder);
        char *Flag = AssembleFlag(Seed);
        printf("Here it is: %s\n", Flag);
    } else {
        printf("Okay, goodbye!\n");
    }
}

void
ChildRank(int MyRank, i64 Seed)
{
    srand((Seed >> MyRank));
    char *FlagPart = GenerateFlagPart();
    MPI_Send(FlagPart, FLAG_PART_LEN, MPI_CHAR, 0, 0, MPI_COMM_WORLD);
}

int
main(int ArgCount, char **ArgV)
{
    int CommSize;
    int MyRank;
    MPI_Init(&ArgCount, &ArgV);
    MPI_Comm_size(MPI_COMM_WORLD, &CommSize);
    MPI_Comm_rank(MPI_COMM_WORLD, &MyRank);

    i64 Seed      = strtoll(ArgV[1], NULL, 10);
    i64 RankOrder = strtoll(ArgV[2], NULL, 10);

    if(0 == MyRank) {
        RootRank(RankOrder);
    } else {
        ChildRank(MyRank, Seed);
    }

    MPI_Finalize();

    return 0;
}
