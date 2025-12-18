#!/bin/bash

# --- Configuration ---
# Target execution time for all processes (in seconds)
RUNTIME=60

# The function that generates CPU load by running arithmetic operations
cpu_burner() {
    # Arguments: $1 = Intensity (number of loops to run)
    INTENSITY=$1
    
    # Simple loop to perform non-IO-bound, CPU-heavy operations
    # The 'bc' command inside the loop forces arithmetic interpretation,
    # and the outer loop controls duration.
    # We use a nested loop for a stable, configurable workload.
    for (( i=1; i<=$RUNTIME; i++ )); do
        for (( j=1; j<=$INTENSITY; j++ )); do
            # The actual workload: complex arithmetic involving pi, division, and exponentiation
            RESULT=$(echo "scale=10; a(1)*4/($i * $j) + 2^($i)" | bc -l)
        done
        sleep 1 # Sleep briefly to control the overall RUNTIME
    done
}

echo "Starting CPU Load Demonstration..."
echo "Target runtime for each process: $RUNTIME seconds."
echo "-----------------------------------------------------"

# --- Launch Processes with Different Intensities ---

# Process 1: Low CPU Consumption (Background/Light)
(
    echo "[Process 1: LOW Load] PID: $$" 
    cpu_burner 1000 # Lower loop count for light use
) &
PID_LOW=$!

# Process 2: Medium CPU Consumption (Normal/Moderate)
(
    echo "[Process 2: MEDIUM Load] PID: $$"
    cpu_burner 5000 # Medium loop count for moderate use
) &
PID_MEDIUM=$!

# Process 3: High CPU Consumption (Heavy/Max Load)
(
    echo "[Process 3: HIGH Load] PID: $$"
    cpu_burner 20000 # High loop count for heavy use
) &
PID_HIGH=$!

echo "Launched three processes with unique PIDs:"
echo "  - PID $PID_LOW (LOW)"
echo "  - PID $PID_MEDIUM (MEDIUM)"
echo "  - PID $PID_HIGH (HIGH)"
echo "-----------------------------------------------------"
echo "Monitoring processes for $RUNTIME seconds. Use 'top' or 'htop' to observe CPU usage."

# --- Wait for all processes to finish ---
wait $PID_LOW
wait $PID_MEDIUM
wait $PID_HIGH

echo "-----------------------------------------------------"
echo "All dummy processes have completed execution."