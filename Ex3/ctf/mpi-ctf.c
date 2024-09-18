#define SDB_LOG_LEVEL 4
#include "sdb.h"

SDB_LOG_REGISTER(MpiCtf);

#include <mpi.h>
#include <stdlib.h>

/*
    ideas:

    one rank will have the flag, set randomly -> potential trick: find a way to set the rng seed,
    can then find sequence in own c program

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
*/

int
main(int ArgCount, char **ArgV)
{
    int CommSize;
    int MyRank;
    MPI_Init(&ArgCount, &ArgV);
    MPI_Comm_size(MPI_COMM_WORLD, &CommSize);
    MPI_Comm_rank(MPI_COMM_WORLD, &MyRank);

    srand(MyRank);

    if(0 == MyRank) {
        SdbLogInfo("There are %d ranks", CommSize);
    } else {
        SdbLogInfo("My rank is %d, and the address of my rank variabled is %p", MyRank, &MyRank);
        if(1 == MyRank || 2 == MyRank || 15 == MyRank) {
            for(int i = 0; i < 5; ++i) {
                SdbLogInfo("Rank %d rand nr. %d: %d", MyRank, i, ((int)rand() % 5));
            }
        }
    }

    MPI_Finalize();

    return 0;
}
