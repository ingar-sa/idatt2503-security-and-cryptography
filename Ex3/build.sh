# Capture the user's current directory
ORIGINAL_DIR=$(pwd)

# Get the directory of the current script
DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"

cd "$DIR"

mkdir -p build

LIBS="-lpthread -lmpi"

if [ "$1" = "relwdb" ]; then
    CFLAGS="-O2 -std=c17 -g -DNDEBUG"
else
    CFLAGS="-g -gdwarf-4 -std=c17 -O0 -Wall -Wextra -pedantic -DDEBUG"
fi

mpicc $CFLAGS ctf/mpi-ctf.c -o build/mpi-ctf $LIBS

cd "$ORIGINAL_DIR"
