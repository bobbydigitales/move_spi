#include <sys/mman.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <time.h>
#include <sys/ioctl.h>


struct SPI_Memory {
    unsigned char outgoing_midi[256];
    unsigned char outgoing_random[512];
    unsigned char outgoing_unknown[1280];
    unsigned char incoming_midi[256];
    unsigned char incoming_random[512];
    unsigned char incoming_unknown[1280];
};

int main() {
    const char *device_path = "/dev/ablspi0.0";
    struct timespec sleep_time;
    sleep_time.tv_sec = 0;
    sleep_time.tv_nsec = 10 * 1000000; 
    ;
    int fd;
    void *addr;
    size_t length = 4096;
    int prot = PROT_READ|PROT_WRITE; 
    int flags = MAP_SHARED;
    off_t offset = 0;

    // Open the device file.
    printf("Opening file");
    fd = open(device_path, O_RDWR); 
    if (fd == -1) {
        perror("open");
        return 1;
    }

    printf("mmaping");
    addr = mmap(NULL, length, prot, flags, fd, offset);

    if (addr == MAP_FAILED) {
        perror("mmap");
        close(fd);
        return 1;
    }

    printf("Reading memory...");
    // 4096 bytes
    // first 2048 - outgoing data
    // next 2048 bytes
    // 256 bytes of USB MIDI
    // 512 bytes of something (Audio or Display)
    // 1280 left?

    struct SPI_Memory* SPIMemory = addr;

    memset(addr, 0, 4096);
    int ioctl_result = 0;
    ioctl_result = (fd, _IOC(_IOC_NONE, 0, 0xb, 0), 0x1312d00);

    while(1) {
        printf("\033[H\033[J");
        
        for (int i=0; i<512; i++) {
            *(SPIMemory->outgoing_midi + i) = rand() % 255;
        }
        
        ioctl_result = ioctl(fd, _IOC(_IOC_NONE, 0, 0xa, 0), 0x300);

        char *mapped_memory = (char *)addr;
        for (int i = 0; i < 4096; ++i) {
            printf("%02x ", (unsigned char)mapped_memory[i]);
            if (i == 2048-1) {
                printf("\n\n");
            }

            if (i == 2048 + 256-1) {
                printf("\n\n");
            }

            if (i== 2048+256+512-1) {
                printf("\n\n");
            }

        }
        printf("\n");
        nanosleep(&sleep_time, NULL);
    }

    // Unmap the memory.
    if (munmap(addr, length) == -1) {
        perror("munmap");
    }

    close(fd);

    return 0;
}