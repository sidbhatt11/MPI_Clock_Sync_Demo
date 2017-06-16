# MPI Clock Synchronization Demo

This is an MPI program that implements the averaging algorithm for distributed clock synchronisation on any number of (even or odd) processors. By default MPI runtime syncs all the clocks for you.. so to simulate such a problem, this programs makes processes sleep for a specific interval of time. The code is pretty self-explanatory. 

Code Resides in :
```
Source/src/main.c
```

Dependencies :
```
gcc or clang
make 
A working MPI implementation (mpicc, mpirun)
```

Build :
```
cd Source
make
```

Run :
```
mpirun mpiClockSync -np < # no of processes here >
```

# Background

This was one of the tasks in the assignment for the subject 'Advanced Distributed and Parallel Systems' (FIT5139) during my Master of Information Technology course at Monash University, Melbourne. 

# Note

This code was written purely for demonstration and educational purposes. This code has no applicability in real world (production). The license allows you to do whatever you want to do with this code. This code comes with zero guarantee/warranty/Support/Hope.

 
