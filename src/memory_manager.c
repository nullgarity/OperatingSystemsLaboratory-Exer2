#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "memory_manager.h"

void init_default_values(MemoryManager *mm) {
    mm->memory_size = 200;
    mm->compaction_interval = 3;
    mm->current_time = 0;
    mm->job_count = 10;
    mm->partition_count = 1;
    
    // DEFAULT job data
    int sizes[] = {50, 20, 30, 70, 80, 20, 10, 30, 20, 50};
    int arrivals[] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
    int processing[] = {5, 4, 3, 2, 6, 7, 1, 3, 4, 5};
    
    for (int i = 0; i < mm->job_count; i++) {
        mm->jobs[i].job_id = i + 1;
        mm->jobs[i].size = sizes[i];
        mm->jobs[i].arrival_time = arrivals[i];
        mm->jobs[i].processing_time = processing[i];
        mm->jobs[i].remaining_time = processing[i];
        mm->jobs[i].start_address = -1;
        mm->jobs[i].is_allocated = 0;
        mm->jobs[i].is_completed = 0;
    }
    
    // initial free partition covering entire memory
    mm->partitions[0].start_address = 0;
    mm->partitions[0].size = mm->memory_size;
    mm->partitions[0].job_id = -1;
    mm->partitions[0].is_free = 1;
    mm->partitions[0].remaining_time = 0;
}

void display_memory_state(MemoryManager *mm) {
    int widths[MAX_PARTITIONS];
    
    for (int i = 0; i < mm->partition_count; i++) {
        if (mm->partitions[i].is_free) {
            widths[i] = 9;
        } else {
            int job_digits = (mm->partitions[i].job_id >= 10) ? 2 : 1;
            int time_digits = (mm->partitions[i].remaining_time >= 10) ? 2 : 1;
            widths[i] = 3 + job_digits + 1 + time_digits + 1;
            if (widths[i] < 9) widths[i] = 9;
        }
    }
    
    for (int i = 0; i < mm->partition_count; i++) {
        printf("+");
        for (int j = 0; j < widths[i]; j++) {
            printf("-");
        }
    }
    printf("+\n");
    
    for (int i = 0; i < mm->partition_count; i++) {
        printf("|");
        if (mm->partitions[i].is_free) {
            for (int j = 0; j < widths[i]; j++) {
                printf(" ");
            }
        } else {
            char content[20];
            sprintf(content, " J%d(%d) ", mm->partitions[i].job_id, mm->partitions[i].remaining_time);
            int content_len = strlen(content);
            printf("%s", content);
            
            for (int j = content_len; j < widths[i]; j++) {
                printf(" ");
            }
        }
    }
    printf("|\n");
    for (int i = 0; i < mm->partition_count; i++) {
        printf("+");
        for (int j = 0; j < widths[i]; j++) {
            printf("-");
        }
    }
    printf("+\n");
    
    int addr = 0;
    for (int i = 0; i < mm->partition_count; i++) {
        printf("%-*d", widths[i] + 1, addr);
        addr += mm->partitions[i].size;
    }
    printf("%d\n", addr);
  getchar();
}

int find_free_partition(MemoryManager *mm, int size) {
    for (int i = 0; i < mm->partition_count; i++) {
        if (mm->partitions[i].is_free && mm->partitions[i].size >= size) {
            return i;
        }
    }
    return -1;
}

void allocate_job(MemoryManager *mm, int job_index, int partition_index) {
    Job *job = &mm->jobs[job_index];
    
    int original_start = mm->partitions[partition_index].start_address;
    int original_size = mm->partitions[partition_index].size;
    
    job->start_address = original_start;
    job->is_allocated = 1;
    
    if (original_size == job->size) {
        mm->partitions[partition_index].job_id = job->job_id;
        mm->partitions[partition_index].is_free = 0;
        mm->partitions[partition_index].remaining_time = job->remaining_time;
    } else {
        // SHIFT THE PARTITIONS to make room
        for (int i = mm->partition_count; i > partition_index + 1; i--) {
            mm->partitions[i] = mm->partitions[i - 1];
        }
        
        // allocate partition
        mm->partitions[partition_index].job_id = job->job_id;
        mm->partitions[partition_index].size = job->size;
        mm->partitions[partition_index].is_free = 0;
        mm->partitions[partition_index].remaining_time = job->remaining_time;
        
        // free partition HERE
        mm->partitions[partition_index + 1].start_address = original_start + job->size;
        mm->partitions[partition_index + 1].size = original_size - job->size;
        mm->partitions[partition_index + 1].job_id = -1;
        mm->partitions[partition_index + 1].is_free = 1;
        mm->partitions[partition_index + 1].remaining_time = 0;
        
        mm->partition_count++;
    }
    
    printf("Job %d allocated at address %d\n", job->job_id, job->start_address);
}

void deallocate_job(MemoryManager *mm, int job_id) {
    for (int i = 0; i < mm->partition_count; i++) {
        if (mm->partitions[i].job_id == job_id) {
            mm->partitions[i].is_free = 1;
            mm->partitions[i].job_id = -1;
            mm->partitions[i].remaining_time = 0;
            printf("Job %d completed and deallocated\n", job_id);
            break;
        }
    }
}

void merge_free_partitions(MemoryManager *mm) {
    int merged = 0;
    for (int i = 0; i < mm->partition_count - 1; i++) {
        if (mm->partitions[i].is_free && mm->partitions[i + 1].is_free) {
            mm->partitions[i].size += mm->partitions[i + 1].size;
            
            // MOVE THE PARTITIONS TO LEFT
            for (int j = i + 1; j < mm->partition_count - 1; j++) {
                mm->partitions[j] = mm->partitions[j + 1];
            }
            mm->partition_count--;
            merged = 1;
            i--;
        }
    }
    if (merged) {
    }
}

void compact_memory(MemoryManager *mm) {
    printf("\nCompaction Started\n");
    
    Partition temp_partitions[MAX_PARTITIONS];
    int temp_count = 0;
    int current_addr = 0;
    
    // should collect partitions
    for (int i = 0; i < mm->partition_count; i++) {
        if (!mm->partitions[i].is_free) {
            temp_partitions[temp_count] = mm->partitions[i];
            temp_partitions[temp_count].start_address = current_addr;
            current_addr += temp_partitions[temp_count].size;
            temp_count++;
        }
    }
    
    // updating job addresses
    for (int i = 0; i < mm->job_count; i++) {
        if (mm->jobs[i].is_allocated && !mm->jobs[i].is_completed) {
            for (int j = 0; j < temp_count; j++) {
                if (temp_partitions[j].job_id == mm->jobs[i].job_id) {
                    mm->jobs[i].start_address = temp_partitions[j].start_address;
                    break;
                }
            }
        }
    }
    
    // add free partition at the end (if NEED)
    if (current_addr < mm->memory_size) {
        temp_partitions[temp_count].start_address = current_addr;
        temp_partitions[temp_count].size = mm->memory_size - current_addr;
        temp_partitions[temp_count].job_id = -1;
        temp_partitions[temp_count].is_free = 1;
        temp_partitions[temp_count].remaining_time = 0;
        temp_count++;
    }
    
    // copy back
    for (int i = 0; i < temp_count; i++) {
        mm->partitions[i] = temp_partitions[i];
    }
    mm->partition_count = temp_count;
    
    printf("Compaction Completed\n");
}

void validate_job_totals(MemoryManager *mm) {
    // HAVE TO CHECK if stupid number of job sizes can theoretically fit
    int total_job_size = 0;
    for (int i = 0; i < mm->job_count; i++) {
        total_job_size += mm->jobs[i].size;
    }
    
    if (total_job_size > mm->memory_size) {
        printf("\nWarning: Total job sizes (%dK) exceed memory size (%dK)\n", 
               total_job_size, mm->memory_size);
        printf("Some jobs may never be allocated without compaction\n");
    }
}
