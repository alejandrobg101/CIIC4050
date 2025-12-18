#define _DEFAULT_SOURCE
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

// Global variable to store the time of the *previous* SIGALRM event
struct timespec last_time;

// --- Helper Function: Calculate Time Difference ---

/**
 * @brief Calculates the time difference in seconds between two timespec
 * structures.
 * @param current The current time.
 * @param previous The previous time.
 * @return The difference in seconds (double).
 */
double timespec_diff(struct timespec current, struct timespec previous) {
  // Calculate difference in seconds
  double sec = (double)(current.tv_sec - previous.tv_sec);

  // Calculate difference in nanoseconds and convert to seconds
  double nsec = (double)(current.tv_nsec - previous.tv_nsec) / 1000000000.0;

  return sec + nsec;
}

// --- Signal Handler ---

/**
 * @brief Signal handler for SIGALRM. Measures elapsed time and resets the
 * timer.
 */
void alarm_handler(int signum) {
  // Use static flag to handle the first call (when last_time is unitialized)
  static int first_call = 1;

  // Get the current time using the monotonic clock
  struct timespec current_time;
  clock_gettime(CLOCK_MONOTONIC, &current_time);

  if (first_call) {
    // Initialize last_time on the very first execution
    printf("[INFO] First alarm received. Timer started.\n");
    first_call = 0;
  } else {
    // Calculate the difference between the current and last signal time
    double elapsed_seconds = timespec_diff(current_time, last_time);

    // Print the result to stderr to avoid mixing with any main process output
    fprintf(stderr, "[ALARM %d] Elapsed time since last signal: %.9f seconds\n",
            signum, elapsed_seconds);
  }

  // Crucial Step: Update last_time for the next signal calculation
  last_time = current_time;
}

// --- Main Program ---

int main() {
  // 1. Set up the signal handler using sigaction (more robust than signal())
  struct sigaction sa;
  memset(&sa, 0, sizeof(sa));
  sa.sa_handler = alarm_handler;
  sa.sa_flags = SA_RESTART;  // Allows system calls to restart if interrupted by
                             // the signal

  if (sigaction(SIGALRM, &sa, NULL) == -1) {
    perror("sigaction failed");
    return 1;
  }

  // 3. Keep the main thread alive indefinitely to receive signals
  while (1) {
    pause();  // Puts the process to sleep until a signal arrives
  }

  // Unreachable, but included for completeness
  return 0;
}