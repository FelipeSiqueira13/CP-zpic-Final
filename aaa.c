// Init MPI once, but keep logic inside this function as requested
    int was_init = 0;
    MPI_Initialized(&was_init);
    if (!was_init) {
        MPI_Init(NULL, NULL);
    }

    int rank = 0, size = 1;
    MPI_Comm_rank( MPI_COMM_WORLD, &rank );
    MPI_Comm_size( MPI_COMM_WORLD, &size );

    // Datatype for particle
    MPI_Datatype mpi_part;
    int blocklen[5] = {1,1,1,1,1};
    MPI_Aint disp[5];
    disp[0] = offsetof(t_part, ix);
    disp[1] = offsetof(t_part, x);
    disp[2] = offsetof(t_part, ux);
    disp[3] = offsetof(t_part, uy);
    disp[4] = offsetof(t_part, uz);
    MPI_Datatype types[5] = {MPI_INT, MPI_FLOAT, MPI_FLOAT, MPI_FLOAT, MPI_FLOAT};
    MPI_Type_create_struct(5, blocklen, disp, types, &mpi_part);
    MPI_Type_commit(&mpi_part);

    // Broadcast scalar state needed by all ranks
    int np_root = spec->np;
    MPI_Bcast(&np_root, 1, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(&spec->iter, 1, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(&spec->n_move, 1, MPI_INT, 0, MPI_COMM_WORLD);

    // Ensure buffers are large enough in non-root ranks
    if (rank != 0) {
        spec_grow_buffer(spec, np_root);
        spec->np = np_root;
    }

    // Partition work
    int base = np_root / size;
    int rem = np_root % size;
    int local_n = base + ((rank < rem) ? 1 : 0);

    int *counts = malloc(size * sizeof(int));
    int *displs = malloc(size * sizeof(int));
    displs[0] = 0;
    for (int r = 0; r < size; r++) {
        counts[r] = base + ((r < rem) ? 1 : 0);
        if (r > 0) displs[r] = displs[r-1] + counts[r-1];
    }

    t_part *local_part = (local_n > 0) ? malloc(local_n * sizeof(t_part)) : NULL;

    // Scatter particle data (only root sends)
    MPI_Scatterv((rank == 0) ? spec->part : NULL, counts, displs, mpi_part,
                 local_part, local_n, mpi_part, 0, MPI_COMM_WORLD);

    // Broadcast fields buffers (they are contiguous)
    int emf_cells = emf->nx + emf->gc[0] + emf->gc[1];
    MPI_Bcast(emf->E_buf, emf_cells * 3, MPI_FLOAT, 0, MPI_COMM_WORLD);
    MPI_Bcast(emf->B_buf, emf_cells * 3, MPI_FLOAT, 0, MPI_COMM_WORLD);

    // Current buffer must start zeroed on all ranks (caller calls current_zero)

    double energy_local = 0.0;