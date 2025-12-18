#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>

#define SHM_NAME "/c4_shared_text"
#define SHM_SIZE 1024

int main(void) {
    char buf[SHM_SIZE];
    printf("Enter a phrase: ");
    fflush(stdout);

    if (!fgets(buf, sizeof(buf), stdin)) return 1;

    // Encrypt (+3) until newline; keep newline as terminator
    for (size_t i = 0; buf[i] && buf[i] != '\n'; i++) buf[i] += 3;

    int fd = shm_open(SHM_NAME, O_CREAT | O_RDWR, 0666);
    if (fd < 0) { perror("shm_open"); return 1; }
    if (ftruncate(fd, SHM_SIZE) < 0) { perror("ftruncate"); return 1; }

    char *mem = mmap(NULL, SHM_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (mem == MAP_FAILED) { perror("mmap"); return 1; }

    memset(mem, 0, SHM_SIZE);
    strncpy(mem, buf, SHM_SIZE - 1); // leave space for '\0'
    printf("Encrypted text written to shared memory.\n");

    munmap(mem, SHM_SIZE);
    close(fd);
    return 0;
}