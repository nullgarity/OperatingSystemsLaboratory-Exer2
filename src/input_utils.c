#include "input_utils.h"


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
