use std::collections::VecDeque;

#[derive(Clone, Debug)]
struct Task {
    size: u16,
    arrival_time: u16,
    processing_time: u16,
    pid: u16,
    partition: Option<u16>,
}

impl Task {
    fn new(size: u16, arrival_time: u16, processing_time: u16, pid: u16) -> Self {
        Task {
            size,
            arrival_time,
            processing_time,
            pid,
            partition: None,
        }
    }
}

#[derive(Clone, Debug)]
struct Memory {
    partitions: u16,
    max_size: u16,
    free: u16,
    used: u16,
    compaction_time: u16,
    queue: VecDeque<Task>,
    allocated_tasks: Vec<Task>, // Track currently allocated tasks
}

impl Memory {
    fn new(partitions: u16, max_size: u16, compaction_time: u16) -> Self {
        Memory {
            partitions,
            max_size,
            free: max_size,
            used: 0,
            compaction_time,
            queue: VecDeque::new(),
            allocated_tasks: Vec::new(),
        }
    }

    fn accept_task(&mut self, task: Task) {
        self.queue.push_back(task);
    }

    fn try_allocate_tasks(&mut self) {
        // Try to allocate as many tasks as possible from the queue
        while let Some(task) = self.queue.front() {
            if task.size <= self.free {
                if let Some(mut task) = self.queue.pop_front() {
                    // Find a partition (this is simplified)
                    task.partition = Some(self.allocated_tasks.len() as u16);
                    self.allocated_tasks.push(task.clone());
                    self.free -= task.size;
                    self.used += task.size;
                }
            } else {
                break; // Not enough memory for the next task
            }
        }
    }

    fn compact(&mut self) {
        // Simple compaction: defragment memory by moving all allocations to the beginning
        let mut new_used = 0;

        for (i, task) in self.allocated_tasks.iter_mut().enumerate() {
            task.partition = Some(i as u16);
            new_used += task.size;
        }

        self.free = self.max_size - new_used;
        self.used = new_used;

        // After compaction, try to allocate more tasks
        self.try_allocate_tasks();
    }

    fn process_tasks(&mut self) {
        // Process tasks and free completed ones
        let mut i = 0;
        while i < self.allocated_tasks.len() {
            // Simulate processing - decrement processing time
            if self.allocated_tasks[i].processing_time > 0 {
                self.allocated_tasks[i].processing_time -= 1;
                i += 1;
            } else {
                // Task completed, free its memory
                let completed_task = self.allocated_tasks.remove(i);
                self.free += completed_task.size;
                self.used -= completed_task.size;
            }
        }
    }

    fn status(&self) {
        println!("Memory Status:");
        println!(
            "Total: {}, Used: {}, Free: {}",
            self.max_size, self.used, self.free
        );
        println!("Queue length: {}", self.queue.len());
        println!("Allocated tasks: {}", self.allocated_tasks.len());
    }
}

fn main() {
    let mut memory = Memory::new(10, 1000, 5);

    // Add some tasks
    memory.accept_task(Task::new(200, 0, 5, 1));
    memory.accept_task(Task::new(300, 0, 3, 2));
    memory.accept_task(Task::new(150, 0, 4, 3));

    memory.try_allocate_tasks();
    memory.status();

    memory.process_tasks();
    memory.status();

    memory.compact();
    memory.status();
}
