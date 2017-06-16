#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include "mpi.h"

// No counter interval will ever be less than 1
const int MINIMUM_ALLOWED_INTERVAL = 1;

// Only one process will touch this. Debug purpose only.
int totalIterations = 0;

int main(int argc, char ** argv) {
	
    // Basic building blocks..
    int rank, size;
    
    // Begin MPI Scope
    MPI_Init(&argc, &argv);
    
    // Get self-identity
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    
    // Get the size of comm
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    
    // Needed while receiving ..
    MPI_Status status;
    
    // Declare an array to hold everybody's counters
    long double clocksOfAllProcesses[size];
    memset(clocksOfAllProcesses, 0, size * sizeof(long double)); // all counters set to 0
    
    // Seed random number generator. or it will always generate the same number for every process
    srand(time(NULL) + rank);
    
    // Generate random number
    long double interval = (rand() % size) + (long double)MINIMUM_ALLOWED_INTERVAL;
    
    // Log the random number
    printf("\nProcess %d of %d generated %Lf\n", rank, size-1, interval);
    fflush(stdout); // In case it doesn't print stuff straighaway..
    
    // Let's all wait for everybody to finish printing ..
    MPI_Barrier(MPI_COMM_WORLD);
    
    // Declare flag
    int clocksAreSynced = 0; // 0 is false BTW
    
    // clocksAreSynced = 1 if clocksAreTheSameCounter = 2;
    int clocksAreTheSameCounter = 0;

    // Var to hold the last recorded time
    long double secondsSinceEpoch;
    
    while (!clocksAreSynced) {
        
        // Printing for debug purpose
        if (rank == 0) {
            ++totalIterations;
            printf("\n\n=======================================================\n");
            printf("\nStarting iteration # : %d\n", totalIterations);
            printf("\n=======================================================\n\n");
            fflush(stdout);
        }
        
        // Sleep
        sleep(interval);
        
        // Get time
        secondsSinceEpoch = time(NULL);
        
        // Announce time
        printf("\nProcess %d of %d slept for %Lf and shows time %Lf seconds since since January 1, 1970\n", rank, size-1, interval, secondsSinceEpoch);
        fflush(stdout);
        
        // Broadcast
        for (int process = 0; process <= size - 1; process++) {
            if (process == rank) continue; // Don't send it to self..because we already have it ! hehe ..
            MPI_Send(&secondsSinceEpoch, 1, MPI_LONG_DOUBLE, process, 0, MPI_COMM_WORLD);
        }
        
        printf("\nProcess %d done sending\n", rank);
        fflush(stdout);
        
        // Set all time to 0. Just to be sure.
        memset(clocksOfAllProcesses, 0, size * sizeof(long double)); // all counters set to 0
        
        // put time in the array
        clocksOfAllProcesses[rank] = secondsSinceEpoch;
        
        // Receive
        for (int process = 0; process <= size - 1; process++) {
            if (process == rank) continue; // Don't try to receive our own message.. coz we didn't send it to us in the first place ! LOL
            MPI_Recv(&clocksOfAllProcesses[process], 1, MPI_LONG_DOUBLE, process, 0, MPI_COMM_WORLD, &status);
        }
        
        printf("\nProcess %d done receiving\n", rank);
        fflush(stdout);
        
        // Check if all clocks are the same
        int clocksAreTheSame = 1; // start with the positive
        for (int process = 0; process <= size - 1; process++) {
            if (clocksAreTheSame) continue;
            if (clocksOfAllProcesses[process] != secondsSinceEpoch) {
                clocksAreTheSame = 0;
            }
        }
        
        if (clocksAreTheSame) {
            clocksAreTheSameCounter += 1;
        } else {
            clocksAreTheSameCounter = 0;
        }
        
        if (clocksAreTheSameCounter == 2) {
            clocksAreSynced = 1;
            
            printf("\nProcess %d of %d has time %Lf\n", rank, size-1, secondsSinceEpoch);
            fflush(stdout);
            
        } else {
           
            // Average
            int numberOfNonZeroTime = 0;
            long double sumOfAllTime = 0;
            for (int process = 0; process <= size - 1; process++) {
                if (clocksOfAllProcesses[process] != 0) {
                    numberOfNonZeroTime++;
                    sumOfAllTime += clocksOfAllProcesses[process];
                }
            }
            
            printf("\nProcess %d of %d received time from %d processes\n", rank, size-1, numberOfNonZeroTime);
            fflush(stdout);
            
            long double averageSecondsSinceEpoch = sumOfAllTime/(long double)numberOfNonZeroTime;
            long double offset = averageSecondsSinceEpoch - secondsSinceEpoch;
            
            // Announce findings
            printf("\nProcess %d of %d found average %Lf and is at %Lf and is off by %Lf\n", rank, size-1, averageSecondsSinceEpoch, secondsSinceEpoch, offset);
            fflush(stdout);

            // Apply the interval
            interval += offset;
            
            // Let's all wait for everybody to finish processing ..
            MPI_Barrier(MPI_COMM_WORLD);
            
            printf("\nNew interval for Process %d of %d is %Lf\n", rank, size-1, interval);
            fflush(stdout);
        }
        
    } // end loop
    
    // Let's all wait for everybody to finish processing ..
    MPI_Barrier(MPI_COMM_WORLD);
    
    // Print Debug messages
    if (rank == 0) {
        printf("\n=======================================================\n");
        printf("\nLogs:\n");
        printf("\n=======================================================\n");
        printf("\nIt took %d iterations to fully sync the clocks among %d processes\n", totalIterations, size);
        printf("\nThe final time when sync was achieved was : %Lf\n", secondsSinceEpoch);
        printf("\nThe final interval when sync was achieved was : %Lf\n", interval);
        int totalMessagesSent = (size-1) * totalIterations;
        printf("\nTotal messages sent : %d\n", totalMessagesSent);
        printf("\nTotal messages received : %d (same as sent basically..)\n\n", totalMessagesSent);
        
        fflush(stdout);
    }
    
	// End MPI Scope
    MPI_Finalize();

	// Done
	return 0;
} 
