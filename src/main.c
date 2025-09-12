#include <stdio.h>
#include "memory_manager.h"
#include "simulation.h" 
#include "input_utils.h"

int main() {
    MemoryManager mm;
    
    printf("Dynamic Memory Reallocation\n");
    
    init_default_values(&mm);
    edit_parameters(&mm);
    
    // run complete sim WHOLE THING NO PAUSE
    run_complete_simulation(&mm);
    
    return 0;
}
