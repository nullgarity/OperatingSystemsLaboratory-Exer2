#ifndef SIMULATION_H
#define SIMULATION_H

#include "memory_manager.h"

void process_arriving_jobs(MemoryManager *mm);
void process_running_jobs(MemoryManager *mm);
void simulate_step(MemoryManager *mm);
int all_jobs_completed(MemoryManager *mm);
void run_complete_simulation(MemoryManager *mm);

#endif
