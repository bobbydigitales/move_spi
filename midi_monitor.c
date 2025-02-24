#include <sys/mman.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <time.h>
#include <sys/ioctl.h>

int main()
{
    const char *device_path = "/dev/ablspi0.0";
    struct timespec sleep_time;
    sleep_time.tv_sec = 0;
    sleep_time.tv_nsec = 5 * 1000000;
    ;
    int fd;
    void *addr;
    size_t length = 4096;
    int prot = PROT_READ | PROT_WRITE;
    int flags = MAP_SHARED;
    off_t offset = 0;

    // Open the device file.
    printf("Opening file\n");
    fd = open(device_path, O_RDWR);
    if (fd == -1)
    {
        perror("open");
        return 1;
    }

    printf("mmaping\n");
    addr = mmap(NULL, length, prot, flags, fd, offset);

    if (addr == MAP_FAILED)
    {
        perror("mmap");
        close(fd);
        return 1;
    }

    printf("Reading memory...\n");
    // 4096 bytes
    // first 2048 - outgoing data
    // next 2048 bytes
    // 256 bytes of USB MIDI
    // 512 bytes of something (Audio or Display)
    // 1280 left?

    memset(addr, 0, 4096);

    int ioctl_result = 0;
    ioctl_result = (fd, _IOC(_IOC_NONE, 0, 0xb, 0), 0x1312d00);


    while (1)
    {
        // printf("\033[H\033[J");

        ioctl_result = ioctl(fd, _IOC(_IOC_NONE, 0, 0xa, 0), 0x300);
        unsigned char *mapped_memory = (unsigned char *)addr;
        int startByte = 2048;
        int length = 256;
        int endByte = startByte + length;

        for (int i = startByte; i < endByte; i += 4)
        {
            if ((unsigned int)mapped_memory[i] == 0)
            {
                continue;
            }

            // unsigned int packet = mapped_memory[i];
            // if (packet == 0) {
            //     continue;
            // }

            unsigned char *byte = &mapped_memory[i];
            unsigned char cable = (*byte & 0b11110000) >> 4;
            unsigned char code_index_number = (*byte & 0b00001111);
            unsigned char midi_0 = *(byte + 1);
            unsigned char midi_1 = *(byte + 2);
            unsigned char midi_2 = *(byte + 3);

            if (code_index_number == 2 || code_index_number == 1 || (cable == 0xf && code_index_number == 0xb && midi_0 == 176))
            {
                continue;
            }

            // printf("%x ", (unsigned int)mapped_memory[i]);
            // printf("%02x ", (unsigned char)mapped_memory[i + 1]);
            // printf("%02x ", (unsigned char)mapped_memory[i + 2]);
            // printf("%02x ", (unsigned char)mapped_memory[i + 3]);
            // printf("\n");

            printf("cable: %x,\tcode index number:%x,\tmidi_0:%d,\tmidi_1:%d,\tmidi_2:%d\n", cable, code_index_number, midi_0, midi_1, midi_2);
        }
        // printf("\n");
        nanosleep(&sleep_time, NULL);
    }

    // Unmap the memory.
    if (munmap(addr, length) == -1)
    {
        perror("munmap");
    }

    close(fd);

    return 0;
}