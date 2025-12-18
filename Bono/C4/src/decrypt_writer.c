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
    int fd = shm_open(SHM_NAME, O_RDWR, 0666);
    if (fd < 0) { perror("shm_open"); return 1; }

    char *mem = mmap(NULL, SHM_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (mem == MAP_FAILED) { perror("mmap"); return 1; }

    // Read until newline or end
    char buf[SHM_SIZE];
    size_t i = 0;
    while (i < SHM_SIZE - 1 && mem[i] && mem[i] != '\n') {
        buf[i] = mem[i];
        i++;
    }
    if (i < SHM_SIZE) buf[i++] = '\n';
    buf[i] = '\0';

    printf("Encrypted: %s", buf);

    for (size_t j = 0; j < i; j++) {
        if (buf[j] != '\n') buf[j] -= 3;
    }
    printf("Decrypted: %s", buf);

    memset(mem, 0, SHM_SIZE);
    printf("Shared memory cleared.\n");

    munmap(mem, SHM_SIZE);
    close(fd);
    return 0;
}