Jake TerHark - jterha2
CS361 - Memory Paging Simulation

To compile the program run 'make' or 'make vmpager' in this directory.

The simulator takes 3 arguments where the first argument is always the file name of
    the file to be memory mapped

The next arguments are optional and may be either or both of the following
    --limit X
        where X is the maximum number of access to process. Ignore or enter 0 to process
        all accesses.
    --table X
        where X is the size of the frame table. Ignore to accept the default of 256 frames.

Ex.
    './vmpager diver.raw --limit 1000 --table 512'
        analyze the first 1000 memory access of diver.raw with a frame table of 512

