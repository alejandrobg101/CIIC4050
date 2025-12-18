#!/bin/bash

# --- Configuration ---
BINARY_FILE="vruntime_log.bin"
C_PROGRAM="../src/main.c"
EXECUTABLE="./vruntime_logger"
TIMER_DELAY_MS=500
TOLERANCE=0.05 # Tolerance for floating-point comparison (5%)
TOTAL_SCORE=40
CURRENT_SCORE=0

# --- Point Allocation ---
POINTS_COMPILE=5
POINTS_TIMER_CONFIG=10
POINTS_HANDLER_EXEC=5
POINTS_BINARY_OUTPUT=10
POINTS_ACCURACY=10

# --- Helper Functions ---

# Function to update and display score
update_score() {
    local points=$1
    local message=$2
    CURRENT_SCORE=$((CURRENT_SCORE + points))
    echo "  [SCORE] +$points pts | $message"
}

# Function to verify the usage of timer functions
check_timer_usage() {
    local pid=$1
    local timer_used=0
    
    # Method 1: Check for timer_create syscall using strace
    # Run strace on the executable and look for timer_create
    if strace -e trace=timer_create "$EXECUTABLE" "$pid" 2>&1 | grep -q "timer_create"; then
        timer_used=1
    fi
    
    # Method 2: Alternative - check /proc/PID/fd for timerfd entries
    # (Less reliable but doesn't require strace)
    # if ls -la /proc/$pid/fd 2>/dev/null | grep -q timer; then
    #     timer_used=1
    # fi
    
    return $timer_used
}

check_timer_symbol() {
    local executable=$1
    
    if nm "$executable" 2>/dev/null | grep -q "sleep\|usleep"; then
        return 1  # sleep functions found
    fi
    
    # Check if the executable was linked with timer functions
    if nm "$executable" 2>/dev/null | grep -q "timer_create\|timer_settime"; then
        return 0  # Timer functions found
    fi

    if nm "$executable" 2>/dev/null | grep -q "setitimer"; then
        return 0  # Timer functions found
    fi
    
    # # Alternative: check if timer functions are called
    # strings "$executable" 2>/dev/null | grep -q "timer_create\|timer_settime" && return 0
    
    return 1  # Timer functions not found
}

# Function to create a CPU-intensive dummy process
dummy_process() {
    # Simple CPU loop to keep vruntime increasing
    COUNTER=0
    while [ $COUNTER -lt 100 ]; do
        # Perform non-IO bound, CPU-heavy operation (using 'bc' for complexity)
        RESULT=$(echo "scale=10; 4 * a(1) / 100000" | bc -l)
        COUNTER=$((COUNTER + 1))
    done
    sleep 20 # Keep the process alive for a while
}

# Function to safely extract vruntime from /proc/<PID>/sched (4th line)
get_vruntime_from_sched() {
    local pid=$1
    local line_content
    # Read /proc/<PID>/sched, specifically the 4th line
    # We grep for 'vruntime' to find the line accurately
    line_content=$(grep "vruntime" /proc/$pid/sched 2>/dev/null)

    if [ -z "$line_content" ]; then
        echo "0.0"
        return
    fi
    
    # Extract the floating point number (often the second field after ':')
    echo "$line_content" | awk -F': ' '{print $2}' | awk '{print $1}'
}


# --- Test Script Start ---

echo "--- VRuntime Logger Test Script (Total Points: $TOTAL_SCORE) ---"

# 1. Cleanup old files
rm -f "$BINARY_FILE" "$EXECUTABLE"

# --- CRITERION 1: Compilation Check (5 Points) ---
echo -e "\n---Compilation Check ---"
gcc "$C_PROGRAM" -o "$EXECUTABLE"
if [ $? -eq 0 ]; then
    update_score $POINTS_COMPILE "Compilation successful."
else
    echo "FAIL: Compilation failed. Cannot proceed with execution tests."
    exit 1
fi

echo -e "\n--- Timer Implementation Check ---"
if check_timer_symbol "$EXECUTABLE"; then
    echo "  [PASS] Timer functions (timer_create/timer_settime) detected."
    update_score $POINTS_TIMER_CONFIG "Correctly used timer instead of sleep()."
else
    echo "  [WARNING] Sleep functions found or Timer functions not detected - program may be using sleep()."
fi


# 2. Launch the dummy process
echo -e "\n--- Setup: Launching CPU load process ---"
dummy_process &
DUMMY_PID=$!
sleep 0.5 # Give dummy process time to accumulate vruntime

# 3. Launch the C logger program
echo "Launching logger program with PID $DUMMY_PID..."
# Run the logger in the background and capture its PID
"$EXECUTABLE" "$DUMMY_PID" &
LOGGER_PID=$!

# Wait briefly for the timer to be set
sleep 0.1


# 4. Wait for the logger to finish (it should exit after 500ms)
WAIT_START_TIME=$(date +%s.%N)

# timeout: 5 seconds for data_logger
timeout=5
elapsed=0
while kill -0 $LOGGER_PID 2>/dev/null && [ $elapsed -lt $timeout ]; do
    sleep 1
    elapsed=$((elapsed + 1))
done

if kill -0 $LOGGER_PID 2>/dev/null; then
    echo -e "${RED}Timeout reached. Killing data_logger.${NC}"
    logger_pid=$(pgrep -x "data_logger")
    if [ -n "$logger_pid" ]; then
        kill -9 $logger_pid
    fi
    C_EXIT_CODE=1  # Mark as failure
else
    wait $LOGGER_PID
    C_EXIT_CODE=0  # Mark as success
fi

WAIT_END_TIME=$(date +%s.%N)

# Calculate the actual time the logger was running (for debug/info)
ELAPSED_WAIT_TIME=$(echo "$WAIT_END_TIME - $WAIT_START_TIME" | bc)
echo "Logger elapsed time (approx): ${ELAPSED_WAIT_TIME}s"


# 5. --- CRITERION 3: Handler Execution and Exit Check (5 Points) ---
if [ $C_EXIT_CODE -eq 0 ]; then
    # Successful exit (SIGALRM handler executed and called exit(0))
    update_score $POINTS_HANDLER_EXEC "Program executed signal handler and exited gracefully (Exit 0)."
else
    echo "FAIL: Logger program exited with non-zero status or crash."
fi

# 6. --- CRITERION 4: Binary Output Check (10 Points) ---
if [ -f "$BINARY_FILE" ]; then
    update_score $POINTS_BINARY_OUTPUT "Binary output file $BINARY_FILE successfully created."

    # Read the double (8 bytes) from the binary file
    LOGGED_VRUNTIME=$(od -An -tf8 -w8 "$BINARY_FILE" | tr -d ' ' | head -n 1)
    
    # 7. --- CRITERION 5: Accuracy Check (10 Points) ---
    REAL_VRUNTIME=$(get_vruntime_from_sched "$DUMMY_PID")
    
    if [ -z "$REAL_VRUNTIME" ] || [ -z "$LOGGED_VRUNTIME" ]; then
        echo "FAIL: Could not extract valid numbers for final comparison."
    else
        echo -e "\n[INFO] Actual vruntime read from /proc/sched: $REAL_VRUNTIME"
        echo "[INFO] Logged vruntime read from binary file: $LOGGED_VRUNTIME"
        
        # Use awk for robust floating point comparison
        AWK_ACCURACY_RESULT=$(awk "BEGIN {
            diff = $REAL_VRUNTIME - $LOGGED_VRUNTIME;
            tolerance = $TOLERANCE * $REAL_VRUNTIME;
            # Check if the difference is within the specified tolerance.
            if (diff >= 0 && diff <= tolerance) {
                print \"PASS\";
            } else {
                print \"FAIL\";
            }
        }")

        if [ "$AWK_ACCURACY_RESULT" == "PASS" ]; then
            update_score $POINTS_ACCURACY "Accuracy check passed: Logged value within tolerance."
        else
            echo "FAIL: Accuracy check failed. Logged value ( $LOGGED_VRUNTIME ) out of tolerance ( $TOLERANCE ) from real value ( $REAL_VRUNTIME )."
        fi
    fi
else
    echo "FAIL: Binary file was not created, accuracy check skipped."
fi


# 8. Final Cleanup and Summary
echo -e "\n--- Final Summary ---"
echo "Total Possible Score: $TOTAL_SCORE"
echo "Obtained Score: $CURRENT_SCORE"

# Ensure the dummy process is terminated
kill "$DUMMY_PID" > /dev/null 2>&1
#rm -f "$BINARY_FILE" "$EXECUTABLE"

if [ $CURRENT_SCORE -eq $TOTAL_SCORE ]; then
    echo "Assessment: EXCELLENT - All criteria met."
    exit 0
elif [ $CURRENT_SCORE -gt $((TOTAL_SCORE / 2)) ]; then
    echo "Assessment: SATISFACTORY - Core functionality demonstrated."
    exit 0
else
    echo "Assessment: UNSATISFACTORY - Major errors present."
    exit 1
fi