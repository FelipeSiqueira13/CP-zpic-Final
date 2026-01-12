#!/bin/bash
#SBATCH --account=f202500010hpcvlabuminhoa
#SBATCH --partition=normal-arm
#SBATCH --ntasks=22
#SBATCH --time=00:4:00
#SBATCH --output=slurm-%j.out
#SBATCH --error=slurm-%j.err

# Run
srun ./zpic