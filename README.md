# zpic_CompParalela
This repository holds the files for the Computação Paralela Course (2025-2026)

Para rodar o código no Deucalion executar os seguintes comandos

srun -A <account> --time=0:15:00 --nodes=1 -n31 -p normal-arm --pty bash
ml OpenMPI
make clean
make run

31 foi o número de processos ideal para a fase
