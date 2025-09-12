#include <stdio.h>
#include "simulation.h"
#include "memory_manager.h"

void process_arriving_jobs(MemoryManager *mm) {
    for (int i = 0; i < mm->job_count; i++) {
        Job *job = &mm->jobs[i];
        
        // new jobs
        if (job->arrival_time == mm->current_time && !job->is_allocated && !job->is_completed) {
            int partition_idx = find_free_partition(mm, job->size);
            if (partition_idx != -1) {
                allocate_job(mm, i, partition_idx);
            } else {
                printf("Job %d arrived but cannot be allocated (insufficient memory)\n", job->job_id);
            }
        }
        
        // RETRY MISSED JOBS so we wont leave a job undone
        if (job->arrival_time < mm->current_time && !job->is_allocated && !job->is_completed) {
            int partition_idx = find_free_partition(mm, job->size);
            if (partition_idx != -1) {
                printf("Job %d now allocated (memory became available)\n", job->job_id);
                allocate_job(mm, i, partition_idx);
            }
        }
    }
}

void process_running_jobs(MemoryManager *mm) {
    for (int i = 0; i < mm->job_count; i++) {
        Job *job = &mm->jobs[i];
        
        if (job->is_allocated && !job->is_completed && job->remaining_time > 0) {
            job->remaining_time--;
            
            // partition REMAINING TIME
            for (int j = 0; j < mm->partition_count; j++) {
                if (mm->partitions[j].job_id == job->job_id) {
                    mm->partitions[j].remaining_time = job->remaining_time;
                    break;
                }
            }
            
            if (job->remaining_time == 0) {
                job->is_completed = 1;
                deallocate_job(mm, job->job_id);
                merge_free_partitions(mm);
            }
        }
    }
}

void simulate_step(MemoryManager *mm) {
    printf("\nTime %d\n", mm->current_time);
    
    process_arriving_jobs(mm);
    process_running_jobs(mm);
    display_memory_state(mm);

    // compaction happens AFTER a step finished; compaction has its own event
    if (mm->current_time > 0 && mm->current_time % mm->compaction_interval == 0) {
        printf(">>> Compaction step <<<\n");
        compact_memory(mm);
        display_memory_state(mm);
    }

    mm->current_time++;
}

int all_jobs_completed(MemoryManager *mm) {
    for (int i = 0; i < mm->job_count; i++) {
        if (!mm->jobs[i].is_completed) {
            return 0;
        }
    }
    return 1;
}

void run_complete_simulation(MemoryManager *mm) {
    printf("\nStarting simulation..\n");
    printf("=================================================\n");
    
    while (!all_jobs_completed(mm) && mm->current_time < MAX_SIMULATION_TIME) {
        simulate_step(mm);
        printf("-------------------------------------------------\n");
    }
    
    printf("\nSimulation completed!\n");
    
    if (all_jobs_completed(mm)) {
        printf("All jobs processed in %d time units.\n", mm->current_time - 1);
    } else {
        printf("Simulation timeout reached at %d time units.\n", MAX_SIMULATION_TIME);
        printf("Uncompleted jobs:\n");
        for (int i = 0; i < mm->job_count; i++) {
            if (!mm->jobs[i].is_completed) {
                printf("  Job %d - ", mm->jobs[i].job_id);
                if (!mm->jobs[i].is_allocated) {
                    printf("Never allocated (size: %d)\n", mm->jobs[i].size);
                } else {
                    printf("Remaining time: %d\n", mm->jobs[i].remaining_time);
                }
            }
        }
    }
}

