#ifndef MEMORY_MANAGER_H
#define MEMORY_MANAGER_H

#include "job.h"
#include "partition.h"

#define MAX_PARTITIONS 20
#define MAX_JOBS 10
#define MIN_MEMORY_SIZE 10
#define MAX_MEMORY_SIZE 10000
#define MIN_COMPACTION_INTERVAL 1
#define MAX_COMPACTION_INTERVAL 100
#define MAX_SIMULATION_TIME 500

typedef struct {
    Job jobs[MAX_JOBS];
    Partition partitions[MAX_PARTITIONS];
    int memory_size;
    int compaction_interval;
    int current_time;
    int partition_count;
    int job_count;
} MemoryManager;

void init_default_values(MemoryManager *mm);
void display_memory_state(MemoryManager *mm);
int find_free_partition(MemoryManager *mm, int size);
void allocate_job(MemoryManager *mm, int job_index, int partition_index);
void deallocate_job(MemoryManager *mm, int job_id);
void merge_free_partitions(MemoryManager *mm);
void compact_memory(MemoryManager *mm);
void validate_job_totals(MemoryManager *mm);

#endif
