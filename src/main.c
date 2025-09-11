#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define JOB_COUNT 10
#define MEMORY_SIZE 200   // total memory in KB
#define COMPACTION_INTERVAL 3   // compact every N steps

// job structure
typedef struct {
    int id;              // job number
    int size;            // memory size (KB)
    int arrival;         // arrival time
    int burst;           // processing time required
    int remaining;       // remaining processing time
    int allocated;       // memory allocated flag
    int finished;        // finished flag
    int start_address;   // for simulating memory partitions
} Job;

// FUNCTIONS

// MAIN
int main() {

}