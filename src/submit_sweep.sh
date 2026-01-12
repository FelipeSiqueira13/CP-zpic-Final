# bash submit_sweep.sh


set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"
cd "$SCRIPT_DIR"

for ntasks in $(seq 1 32); do
    sbatch --ntasks="$ntasks" mpi.sh
    echo "Submitted run with $ntasks task(s)"
done
