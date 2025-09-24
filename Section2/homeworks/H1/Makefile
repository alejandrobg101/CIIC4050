# Main Makefile for the Command Queue project

.PHONY: all clean worker manager test help

# Default target
all: worker manager

# Build worker program
worker:
	$(MAKE) -C worker all
	$(MAKE) -C worker install

# Build manager program
manager:
	$(MAKE) -C manager all
	$(MAKE) -C manager install

# Clean all build artifacts
clean:
	$(MAKE) -C worker clean
	$(MAKE) -C manager clean
	rm -f worker manager
	rm -f worker_log_*.txt

# Test the complete system
test: all
	@echo "Testing the Command Queue system..."
	@echo "Starting workers..."
	@./worker &
	@WORKER1_PID=$$!; \
	echo "Worker 1 started with PID: $$WORKER1_PID"; \
	./worker &
	@WORKER2_PID=$$!; \
	echo "Worker 2 started with PID: $$WORKER2_PID"; \
	sleep 1; \
	echo "Starting manager with PIDs $$WORKER1_PID and $$WORKER2_PID"; \
	./manager $$WORKER1_PID $$WORKER2_PID; \
	echo "Manager finished. Checking log files..."; \
	if [ -f "worker_log_$$WORKER1_PID.txt" ]; then \
		echo "Worker 1 log (PID $$WORKER1_PID):"; \
		cat "worker_log_$$WORKER1_PID.txt"; \
	fi; \
	if [ -f "worker_log_$$WORKER2_PID.txt" ]; then \
		echo "Worker 2 log (PID $$WORKER2_PID):"; \
		cat "worker_log_$$WORKER2_PID.txt"; \
	fi

# Help target
help:
	@echo "Available targets:"
	@echo "  all     - Build both worker and manager programs"
	@echo "  worker  - Build only the worker program"
	@echo "  manager - Build only the manager program"
	@echo "  clean   - Remove all build artifacts and log files"
	@echo "  test    - Build and run a complete test of the system"
	@echo "  help    - Show this help message"
