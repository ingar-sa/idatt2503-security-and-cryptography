# Capture the user's current directory
ORIGINAL_DIR=$(pwd)

# Get the directory of the current script
DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"

cd "$DIR"

mpiexec -n 9 --oversubscribe ./build/mpi-ctf 8723469782356293 14753682

cd "$ORIGINAL_DIR"
