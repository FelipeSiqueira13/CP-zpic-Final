#!/bin/bash
# Submit one SLURM job per task count from 1 to 32.
# Overrides the --ntasks directive in mpi.sh via sbatch CLI.

set -euo pipefail

# Run from script directory so mpi.sh is found
SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"
cd "$SCRIPT_DIR"

for ntasks in $(seq 1 32); do
    sbatch --ntasks="$ntasks" mpi.sh
    echo "Submitted run with $ntasks task(s)"
done
