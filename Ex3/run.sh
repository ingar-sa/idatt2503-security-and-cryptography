# Capture the user's current directory
ORIGINAL_DIR=$(pwd)

# Get the directory of the current script
DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"

cd "$DIR"

mpiexec -n 9 --oversubscribe ./build/mpi-ctf

cd "$ORIGINAL_DIR"
