#ifndef PARTITION_H
#define PARTITION_H

typedef struct {
    int start_address;
    int size;
    int job_id;
    int is_free;
    int remaining_time;
} Partition;

#endif
