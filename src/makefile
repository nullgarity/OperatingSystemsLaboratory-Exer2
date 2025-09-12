# Compiler and flags
CC = gcc
CFLAGS = -Wall -Wextra -std=c99
OBJS = main.o memory_manager.o simulation.o input_utils.o
TARGET = memory_sim

# Default target
all: $(TARGET)

# Create the executable
$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJS)

# Compile individual source files
main.o: main.c memory_manager.h simulation.h input_utils.h
	$(CC) $(CFLAGS) -c main.c

memory_manager.o: memory_manager.c memory_manager.h job.h partition.h
	$(CC) $(CFLAGS) -c memory_manager.c

simulation.o: simulation.c simulation.h memory_manager.h
	$(CC) $(CFLAGS) -c simulation.c

input_utils.o: input_utils.c input_utils.h memory_manager.h
	$(CC) $(CFLAGS) -c input_utils.c

# Clean up compiled files
clean:
	rm -f $(TARGET) *.o

# Phony targets (not actual files)
.PHONY: all clean
