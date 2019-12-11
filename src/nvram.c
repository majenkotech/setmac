#include <stdio.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <errno.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>
#include <alloca.h>

#define NVRAM_START 0x1d0fc000

#define min(A, B) (((A) < (B)) ? (A) : (B))

int nvram_read(const char *key, char *val, size_t maxlen) {

    if (val == NULL) return -1;

    // Obtain handle to physical memory
    int fd = open("/dev/mem", O_RDWR | O_SYNC);
    if (fd < 0) {
        fprintf(stderr, "Unable to open /dev/mem: %s\n", strerror(errno));
        return -1;
    }

    // Map a page of memory to the physical address
    int32_t nvram_base = (int32_t) mmap(0, 0x4000, PROT_READ|PROT_WRITE, MAP_SHARED, fd, NVRAM_START);
    if (nvram_base < 0) {
        fprintf(stderr, "Mmap failed: %s\n", strerror(errno));
        close(fd);
        return -1;
    }
    char *data = (char *)(nvram_base + 4);
    char *keyeq = alloca(strlen(key) + 2);
    strcpy(keyeq, key);
    strcat(keyeq, "=");

    int klen = strlen(keyeq);
    while (data[0] != 0) {
        int dlen = strlen(data);
        if (strncmp(data, keyeq, min(klen, dlen)) == 0) {
            strncpy(val, data+klen, maxlen);
            munmap((void *)nvram_base, 0x4000);
            close(fd);
            return 0;
        }
        data += (strlen(data) + 1);
    }

    munmap((void *)nvram_base, 0x4000);
    close(fd);
    fprintf(stderr, "Key %s not found\n", key);

    return -1;
}
