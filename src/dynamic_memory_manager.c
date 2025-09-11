#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <limits.h>

#define MAX_JOBS 10
#define MAX_PARTITIONS 20
#define MIN_MEMORY_SIZE 10
#define MAX_MEMORY_SIZE 10000
#define MIN_COMPACTION_INTERVAL 1
#define MAX_COMPACTION_INTERVAL 100
#define MAX_SIMULATION_TIME 50

typedef struct {
    int job_id;
    int size;
    int arrival_time;
    int processing_time;
    int remaining_time;
    int start_address;
    int is_allocated;
    int is_completed;
} Job;

typedef struct {
    int start_address;
    int size;
    int job_id;
    int is_free;
    int remaining_time;
} Partition;

typedef struct {
    Job jobs[MAX_JOBS];
    Partition partitions[MAX_PARTITIONS];
    int memory_size;
    int compaction_interval;
    int current_time;
    int partition_count;
    int job_count;
} MemoryManager;

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
    
    // compact first if it's time
    if (mm->current_time > 0 && mm->current_time % mm->compaction_interval == 0) {
        compact_memory(mm);
    }
    
    process_arriving_jobs(mm);
    process_running_jobs(mm);
    display_memory_state(mm);
    
    mm->current_time++;
}

char* get_input_line(char* buffer, int size) {
    if (fgets(buffer, size, stdin) != NULL) {
        buffer[strcspn(buffer, "\n")] = 0;
        return buffer;
    }
    return NULL;
}
// ERROR CHECKING !! 
int is_valid_integer(const char* str) {
    if (str == NULL || *str == '\0') return 0;
    
    while (isspace(*str)) str++;
    
    if (*str == '+' || *str == '-') str++;
    
    // must have at least one digit
    if (!isdigit(*str)) return 0;
    
    // check all remaining chars are digits
    while (*str) {
        if (!isdigit(*str) && !isspace(*str)) return 0;
        str++;
    }
    
    return 1;
}
// PROCESS INPUTS
int get_validated_integer(const char* prompt, int min, int max, int default_value) {
    char input[100];
    int value;
    
    while (1) {
        printf("%s", prompt);
        if (get_input_line(input, sizeof(input)) && strlen(input) > 0) {
            // trim whitespace
            char* trimmed = input;
            while (isspace(*trimmed)) trimmed++;
            
            if (!is_valid_integer(trimmed)) {
                printf("Error: Please enter a valid integer\n");
                continue;
            }
            
            value = atoi(trimmed);
            
            if (value < min || value > max) {
                printf("Error: Value must be between %d and %d\n", min, max);
                continue;
            }
            
            return value;
        } else {
            return default_value;
        }
    }
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
// EDIT PARAMS !
void edit_parameters(MemoryManager *mm) {
    char choice;
    printf("\nEdit parameters? (y/n): ");
    scanf(" %c", &choice);
    getchar();
    
    if (choice == 'y' || choice == 'Y') {
        char input[100];
        
        // CHECK . AGAIN
        mm->memory_size = get_validated_integer(
            "Enter memory size (current: 200, press Enter to keep): ",
            MIN_MEMORY_SIZE, MAX_MEMORY_SIZE, mm->memory_size
        );
        
        // VALIDATE
        mm->compaction_interval = get_validated_integer(
            "Enter compaction interval (current: 3, press Enter to keep): ",
            MIN_COMPACTION_INTERVAL, MAX_COMPACTION_INTERVAL, mm->compaction_interval
        );
        
        printf("Edit job details? (y/n): ");
        scanf(" %c", &choice);
        getchar(); // consume newline
        
        if (choice == 'y' || choice == 'Y') {
            for (int i = 0; i < mm->job_count; i++) {
                int valid_input = 0;
                
                while (!valid_input) {
                    printf("\nJob %d - Size:%d Arrival:%d Processing:%d\n", 
                           i+1, mm->jobs[i].size, mm->jobs[i].arrival_time, mm->jobs[i].processing_time);
                    printf("New values (size arrival processing, press Enter to keep): ");
                    
                    if (get_input_line(input, sizeof(input)) && strlen(input) > 0) {
                        // parse three INTEGERS EX. 50 0 5 NO COMMA
                        char* token;
                        char input_copy[100];
                        strcpy(input_copy, input);
                        
                        int values[3];
                        int count = 0;
                        
                        token = strtok(input_copy, " \t");
                        while (token != NULL && count < 3) {
                            if (!is_valid_integer(token)) {
                                printf("Error: All values must be valid integers\n");
                                count = -1;
                                break;
                            }
                            values[count++] = atoi(token);
                            token = strtok(NULL, " \t");
                        }
                        
                        if (count == -1) {
                            continue;
                        }
                        
                        if (count != 3) {
                            printf("Error: Please enter exactly 3 values (size arrival processing)\n");
                            continue;
                        }
                        
                        // validate ranges
                        if (values[0] <= 0 || values[0] > mm->memory_size) {
                            printf("Error: Job size must be between 1 and %d\n", mm->memory_size);
                            continue;
                        }
                        
                        if (values[1] < 0 || values[1] >= MAX_SIMULATION_TIME) {
                            printf("Error: Arrival time must be between 0 and %d\n", MAX_SIMULATION_TIME - 1);
                            continue;
                        }
                        
                        if (values[2] <= 0 || values[2] > MAX_SIMULATION_TIME) {
                            printf("Error: Processing time must be between 1 and %d\n", MAX_SIMULATION_TIME);
                            continue;
                        }
                        
                        mm->jobs[i].size = values[0];
                        mm->jobs[i].arrival_time = values[1];
                        mm->jobs[i].processing_time = values[2];
                        mm->jobs[i].remaining_time = values[2];
                        valid_input = 1;
                    } else {
                        valid_input = 1;
                    }
                }
            }
            
            validate_job_totals(mm);
        }
        
        mm->partition_count = 1;
        mm->partitions[0].start_address = 0;
        mm->partitions[0].size = mm->memory_size;
        mm->partitions[0].job_id = -1;
        mm->partitions[0].is_free = 1;
        mm->partitions[0].remaining_time = 0;
        
        for (int i = 0; i < mm->job_count; i++) {
            mm->jobs[i].start_address = -1;
            mm->jobs[i].is_allocated = 0;
            mm->jobs[i].is_completed = 0;
            mm->jobs[i].remaining_time = mm->jobs[i].processing_time;
        }
    }
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

int main() {
    MemoryManager mm;
    
    printf("Dynamic Memory Reallocation\n");
    
    init_default_values(&mm);
    edit_parameters(&mm);
    
    // run complete sim WHOLE THING NO PAUSE
    run_complete_simulation(&mm);
    
    return 0;
}