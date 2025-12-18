#define _XOPEN_SOURCE 700

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>
#include <sys/mman.h>
#include <unistd.h>

// --- Configuration ---
#define BLOCK_SIZE_KB 64  // Size of each block in Kilobytes
#define BLOCK_SIZE (BLOCK_SIZE_KB * 1024)
#define PROC_MAPS_PATH "/proc/self/maps"

// Structure to hold the details of each memory block
typedef struct {
  char name[20];
  int* address;
  size_t size;
  char os_map_range[30];  // Stores the range read from /proc/maps
} MemoryBlock;

// Global array to store block information
MemoryBlock blocks[4];

// --- Helper Functions ---

/**
 * @brief Reads /proc/self/maps and finds the mapped range for a given address.
 * @param addr The start address reported by mmap.
 * @param output_buffer Buffer to store the resulting range (e.g.,
 * 7fff0000-7fff4000).
 */
void Get_os_map_range(void* addr, char* output_buffer, size_t buf_size) {
  FILE* f = fopen(PROC_MAPS_PATH, "r");
  char line[256];
  uintptr_t start = 0, end = 0;

  if (buf_size > 0) {
    strncpy(output_buffer, "NOT_FOUND", buf_size - 1);
    output_buffer[buf_size - 1] = '\0';
  }

  if (!f) {
    if (buf_size > 0) {
      strncpy(output_buffer, "FILE_ERROR", buf_size - 1);
      output_buffer[buf_size - 1] = '\0';
    }
    return;
  }

  char addr_prefix[32];
  snprintf(addr_prefix, sizeof(addr_prefix), "%" PRIxPTR, (uintptr_t)addr);

  while (fgets(line, sizeof(line), f)) {
    if (sscanf(line, "%" SCNxPTR "-" "%" SCNxPTR, &start, &end) == 2) {
      if ((uintptr_t)addr >= start && (uintptr_t)addr < end) {
        if (buf_size > 0) {
          snprintf(output_buffer, buf_size,
             "%" PRIxPTR "-" "%" PRIxPTR, start, end);
          output_buffer[buf_size - 1] = '\0';
        }
        fclose(f);
        return;
      }
    }
  }

  fclose(f);
}

// --- Core Task Function ---

/**
 * @brief Creates four distinct memory blocks using mmap().
 * * REQUIRED POSIX FUNCTION: mmap(void *addr, size_t length, int prot, int
 * flags, int fd, off_t offset)
 * * TASK: Create the following four memory blocks, storing the returned address
 * in the blocks array:
 * * 1. BLOCK 0 (Auto-Assigned): Use NULL for the addr parameter.
 * 2. BLOCK 1 (Another Auto-Assigned): Use NULL for the addr parameter again.
 * 3. BLOCK 2 (Hinted Address 1): Use a specific, high-address hint (e.g.,
 * 0x70000000) for the addr parameter, using the MAP_FIXED flag to make the
 * request mandatory.
 * 4. BLOCK 3 (Hinted Address 2): Use a different specific, high-address hint
 * (e.g., 0x80000000) for the addr parameter, using the MAP_FIXED flag.
 * * NOTE: MAP_FIXED is aggressive and may fail if the address is already in
 * use.
 */
void Create_memory_blocks() {
  strncpy(blocks[0].name, "BLOCK_0", sizeof(blocks[0].name) - 1);
  blocks[0].size = 64 * 1024;

  strncpy(blocks[1].name, "BLOCK_1", sizeof(blocks[1].name) - 1);
  blocks[1].size = 128 * 1024;

  strncpy(blocks[2].name, "BLOCK_2", sizeof(blocks[2].name) - 1);
  blocks[2].size = 64 * 1024;

  strncpy(blocks[3].name, "BLOCK_3", sizeof(blocks[3].name) - 1);
  blocks[3].size = 64 * 1024;

  for (int i = 0; i < 4; i++) {
    blocks[i].name[sizeof(blocks[i].name) - 1] = '\0';
    blocks[i].os_map_range[0] = '\0';
  }

  void* addrs[4];
  int flags[4];

  addrs[0] = NULL;
  flags[0] = MAP_PRIVATE | MAP_ANONYMOUS;

  addrs[1] = NULL;
  flags[1] = MAP_PRIVATE | MAP_ANONYMOUS;

  addrs[2] = (void*)0x70000000;
  flags[2] = MAP_PRIVATE | MAP_ANONYMOUS | MAP_FIXED;

  addrs[3] = (void*)0x80000000;
  flags[3] = MAP_PRIVATE | MAP_ANONYMOUS | MAP_FIXED;

  int values[4] = {100, 200, 300, 400};

  for (int i = 0; i < 4; i++) {
    void* addr = mmap(addrs[i], blocks[i].size, PROT_READ | PROT_WRITE,
                      flags[i], -1, 0);

    if (addr == MAP_FAILED) {
      blocks[i].address = MAP_FAILED;
      strncpy(blocks[i].os_map_range, "MAP_FAILED",
        sizeof(blocks[i].os_map_range) - 1);
      blocks[i].os_map_range[sizeof(blocks[i].os_map_range) - 1] = '\0';
      continue;
    }

    blocks[i].address = (int*)addr;
    blocks[i].address[0] = values[i];

    Get_os_map_range(addr, blocks[i].os_map_range,
       sizeof(blocks[i].os_map_range));
  }
}

/**
 * @brief Prints a single block entry for the final log line.
 * @param pid The PID of the current process (kept for compatibility if needed).
 * @param block Pointer to the MemoryBlock to print.
 */
void Print_result_log(pid_t pid, MemoryBlock* block) {
  printf("%s=%p/%zu/%d/%s|", block->name, (void*)block->address, block->size,
         block->address[0], block->os_map_range);
}

int main() {
  pid_t pid = getpid();

  // Create the memory blocks
  Create_memory_blocks();

  // Print the required log line using a per-block function
  printf("LOG_START|PID=%d|", pid);
  for (int i = 0; i < 4; i++) {
    Print_result_log(pid, &blocks[i]);
  }
  printf("LOG_END\n");

  // Wait for validation script to run. The script will kill the process.
  // We use a safe sleep here to prevent the process from exiting immediately.
  sleep(10);

  // Clean up
  for (int i = 0; i < 4; i++) {
    if (blocks[i].address != MAP_FAILED) {
      munmap(blocks[i].address, blocks[i].size);
    }
  }

  return 0;
}
