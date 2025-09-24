# H1: The Command Queue

This project implements a multi-process application that uses a manager-worker model with signals for inter-process communication. The manager process reads commands from a file and sends signals to worker processes.

## Project Structure

```
H1/
├── manager/
│   ├── src/
│   │   └── manager.c      # Manager program source
│   ├── build/             # Build directory
│   └── Makefile           # Manager build configuration
├── worker/
│   ├── src/
│   │   └── worker.c       # Worker program source
│   ├── build/             # Build directory
│   └── Makefile           # Worker build configuration
├── commands.txt           # Command file for manager
├── Makefile              # Main build configuration
└── README.md             # This file
```

## Building the Project

### Build All Programs
```bash
make all
```

### Build Individual Programs
```bash
make worker    # Build only worker
make manager   # Build only manager
```

### Clean Build Artifacts
```bash
make clean
```

## Running the System

### Manual Execution

1. **Start two worker processes:**
   ```bash
   # Terminal 1
   ./worker &
   [1] 12345
   
   # Terminal 2  
   ./worker &
   [2] 12346
   ```

2. **Run the manager with worker PIDs:**
   ```bash
   ./manager 12345 12346
   ```

### Automated Testing

Run the complete test suite:
```bash
make test
```

This will:
- Build both programs
- Start two worker processes automatically
- Run the manager with the worker PIDs
- Display the log files created by the workers

## Commands File Format

The `commands.txt` file contains commands in the format:
```
worker1 SIGUSR1
worker2 SIGUSR2
worker1 SIGUSR2
worker2 SIGUSR1
```

Each line specifies:
- Worker number (worker1 or worker2)
- Signal to send (SIGUSR1 or SIGUSR2)

## Output Files

The system creates log files for each worker process:
- `worker_log_<PID>.txt` - Contains the signals received by each worker

## Program Behavior

### Worker Program
- Registers signal handlers for SIGUSR1 and SIGUSR2 using `sigaction()`
- Writes received signals to a unique log file based on its PID
- Runs indefinitely until terminated with SIGTERM
- Prints startup information to stderr

### Manager Program
- Reads commands from `commands.txt`
- Waits 1 second between each command
- Sends the specified signal to the correct worker process
- Prints signal sending actions to stderr
- Terminates all workers when finished

## Signal Handling

- **SIGUSR1**: Logs "SIGUSR1 received" to worker log file
- **SIGUSR2**: Logs "SIGUSR2 received" to worker log file  
- **SIGTERM**: Gracefully terminates worker processes

## Error Handling

- Invalid command line arguments
- File I/O errors
- Signal sending failures
- Invalid command format in commands.txt
- Unknown worker names or signal names

## Example Output

After running the system, you should see log files like:

**worker_log_12345.txt:**
```
SIGUSR1 received
SIGUSR2 received
```

**worker_log_12346.txt:**
```
SIGUSR2 received
SIGUSR1 received
```
