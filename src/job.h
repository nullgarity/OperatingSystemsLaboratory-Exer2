#ifndef JOB_H
#define JOB_H

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

#endif
