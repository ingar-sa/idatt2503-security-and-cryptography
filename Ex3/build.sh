# Capture the user's current directory
ORIGINAL_DIR=$(pwd)

# Get the directory of the current script
DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"

cd "$DIR"

mkdir -p build

//LIBS="-lmpi"
CFLAGS="-g -gdwarf-4 -std=c99 -O0 -Wno-deprecated-declarations -fno-stack-protector -Wall -Wextra -pedantic -DDEBUG"

mpicc $CFLAGS ctf/mpi-ctf.c -o build/mpi-ctf $LIBS

cd "$ORIGINAL_DIR"
