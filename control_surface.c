#include <sys/mman.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <time.h>
#include <sys/ioctl.h>

struct SPI_Memory
{
    unsigned char outgoing_midi[256];
    unsigned char outgoing_random[512];
    unsigned char outgoing_unknown[1280];
    unsigned char incoming_midi[256];
    unsigned char incoming_random[512];
    unsigned char incoming_unknown[1280];
};


void clearPads(unsigned char* mapped_memory, int fd) {

    // clear pads
    int j = 0;
    for (int i=0, pad = 0; pad < 32; pad++)
    {
        j = i*4;

        ((struct SPI_Memory *)mapped_memory)->outgoing_midi[j + 0] = 0 | 0x9;
        ((struct SPI_Memory *)mapped_memory)->outgoing_midi[j + 1] = 0x90;
        ((struct SPI_Memory *)mapped_memory)->outgoing_midi[j + 2] = (68 + pad);
        ((struct SPI_Memory *)mapped_memory)->outgoing_midi[j + 3] = 0;


        if (i > 9) {
            int ioctl_result = ioctl(fd, _IOC(_IOC_NONE, 0, 0xa, 0), 0x300);
            // memset(((struct SPI_Memory *)mapped_memory)->outgoing_midi, 0, 256);
            i=0;
        }

        printf("clearing pad %d\n", pad);

        i++;
    }

    int ioctl_result = ioctl(fd, _IOC(_IOC_NONE, 0, 0xa, 0), 0x300);


}

void clearSequencerButtons(unsigned char* mapped_memory, int fd) {

    // clear pads
    int j = 0;
    for (int i=0, pad = 0; pad < 16; pad++)
    {
        j = i*4;

        ((struct SPI_Memory *)mapped_memory)->outgoing_midi[j + 0] = 0 | 0x9;
        ((struct SPI_Memory *)mapped_memory)->outgoing_midi[j + 1] = 0x90;
        ((struct SPI_Memory *)mapped_memory)->outgoing_midi[j + 2] = (16 + pad);
        ((struct SPI_Memory *)mapped_memory)->outgoing_midi[j + 3] = 0;


        if (i > 9) {
            int ioctl_result = ioctl(fd, _IOC(_IOC_NONE, 0, 0xa, 0), 0x300);
            // memset(((struct SPI_Memory *)mapped_memory)->outgoing_midi, 0, 256);
            i=0;
        }

        printf("clearing button %d\n", pad);

        i++;
    }

    int ioctl_result = ioctl(fd, _IOC(_IOC_NONE, 0, 0xa, 0), 0x300);


}

int main()
{
    const char *device_path = "/dev/ablspi0.0";
    struct timespec sleep_time;
    sleep_time.tv_sec = 0;
    sleep_time.tv_nsec = 1 * 1000000;
    ;
    int fd;
    unsigned char *mapped_memory;
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
    mapped_memory = mmap(NULL, length, prot, flags, fd, offset);

    if (mapped_memory == MAP_FAILED)
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

    memset(mapped_memory, 0, 4096);

    int ioctl_result = 0;
    ioctl_result = (fd, _IOC(_IOC_NONE, 0, 0xb, 0), 0x1312d00);

    int padIndex = 0;
    int padColor = 1;

    clearPads(mapped_memory, fd);
    clearSequencerButtons(mapped_memory, fd);

    int knobs[8] = {0,0,0,0,0,0,0,0};

    while (1)
    {
        // printf("\033[H\033[J");

        ioctl_result = ioctl(fd, _IOC(_IOC_NONE, 0, 0xa, 0), 0x300);
        int startByte = 2048;
        int length = 256;
        int endByte = startByte + length;

        // https://www.usb.org/sites/default/files/midi10.pdf
        //  code index number:9,    midi_0:144,     midi_1:83,      midi_2:18

        // for (int i=0; i<265/4; i+=4) {
        //     ((struct SPI_Memory*)mapped_memory)->outgoing_midi[i+0] = 0 | 0x9;
        //     ((struct SPI_Memory*)mapped_memory)->outgoing_midi[i+1] = 0x90;
        //     ((struct SPI_Memory*)mapped_memory)->outgoing_midi[i+2] = 0x44 + padIndex++ % 0x20;
        //     ((struct SPI_Memory*)mapped_memory)->outgoing_midi[i+3] =  padColor % 0x7f;
        // }

        // if (padIndex%0x20 == 0) {
        //     padColor++;
        // }

        // for (int i=0; i<265/4; i+=4) {
        // unsigned char values[] = {0|0x4, 0xF0, 0x00, 0x21,
        //                           0|0x4, 0x1D, 0x01, 0x01,
        //                           0|0x7, 0x06, 1, 0xF7};
        // memcpy(((struct SPI_Memory*)mapped_memory)->outgoing_midi, values, sizeof(values));
        // ((struct SPI_Memory*)mapped_memory)->outgoing_midi[0] = 0 | 0x9;
        // ((struct SPI_Memory*)mapped_memory)->outgoing_midi[1] = 0x90;
        // ((struct SPI_Memory*)mapped_memory)->outgoing_midi[2] = 0x44 + padIndex++ % 0x20;
        // ((struct SPI_Memory*)mapped_memory)->outgoing_midi[3] =  padColor % 0x7f;
        // }

        // if (padIndex%0x20 == 0) {
        //     padColor++;
        // }

        // padIndex++;
        int control_change = 0xb;

        int out_index = 0;

        memset(((struct SPI_Memory *)mapped_memory)->outgoing_midi, 0, 256);
        for (int i = startByte; i < endByte; i += 4)
        {
            if ((unsigned int)mapped_memory[i] == 0)
            {
                continue;
            }

            unsigned char *byte = &mapped_memory[i];
            unsigned char cable = (*byte & 0b11110000) >> 4;
            unsigned char code_index_number = (*byte & 0b00001111);
            unsigned char midi_0 = *(byte + 1);
            unsigned char midi_1 = *(byte + 2);
            unsigned char midi_2 = *(byte + 3);

            // if ((code_index_number == 2 || code_index_number == 1) || (cable == 0xf && code_index_number == control_change && midi_0 == 176))
            // {
            //     continue;
            // }

            if (byte[1] + byte[2] + byte[3] == 0)
            {
                continue;
            }

            printf("cable: %x,\tcode index number:%x,\tmidi_0:%x,\tmidi_1:%d,\tmidi_2:%d\n", cable, code_index_number, midi_0, midi_1, midi_2);
            // printf("first_byte%d,\tmidi_0:%x,\tmidi_1:%d,\tmidi_2:%d\n", byte, midi_0, midi_1, midi_2);

            // swap input channels
            // if (cable == 2) {
            //     cable = 0;
            // }

            // if (cable == 0) {
            //     cable = 2;
            // }

            // This works
            // int i = 0;
            if (byte[1] == 0x90 && cable == 2)
            {
                int oi = out_index;
                ((struct SPI_Memory *)mapped_memory)->outgoing_midi[oi + 0] = 0 | 0x9;
                ((struct SPI_Memory *)mapped_memory)->outgoing_midi[oi + 1] = 0x90;
                ((struct SPI_Memory *)mapped_memory)->outgoing_midi[oi + 2] = byte[2];
                ((struct SPI_Memory *)mapped_memory)->outgoing_midi[oi + 3] = padColor % 0x7f;
                padColor++;

                out_index++;
            }

            // Note-on message on virtual cable 1 (CN=0x1; CIN=0x9) 9n kk vv 19 9n kk vv
            if (byte[1] == 0x90 && cable == 0)
            {
                int channel = 1 - 1;
                int out_cable = 2;

                int oi = out_index;

                ((struct SPI_Memory *)mapped_memory)->outgoing_midi[oi + 0] = out_cable << 4 | 0x9;
                ((struct SPI_Memory *)mapped_memory)->outgoing_midi[oi + 1] = 0x90 | channel;
                ((struct SPI_Memory *)mapped_memory)->outgoing_midi[oi + 2] = byte[2];
                ((struct SPI_Memory *)mapped_memory)->outgoing_midi[oi + 3] = 100;
                printf("Sending Note On %x %x %x %d\n", cable << 4 | 0x9, 0x90 | channel, byte[2], 100);

                out_index++;
            }

            if (byte[1] == 0xb0 && cable == 0)
            {
                int channel = 1 - 1;
                int out_cable = 2;

                int oi = out_index;

                int knob = byte[2] - 71;

                if (byte[3] == 127) {
                    knobs[knob] -=1;
                }

                if (byte[3] == 1) {
                    knobs[knob] += 1;
                }

                if (knobs[knob] >127) {
                    knobs[knob] = 127;
                }

                if (knobs[knob] <0 ) {
                    knobs[knob] = 0;
                }




                ((struct SPI_Memory *)mapped_memory)->outgoing_midi[oi + 0] = out_cable << 4 | 0xb;
                ((struct SPI_Memory *)mapped_memory)->outgoing_midi[oi + 1] = 0xb0;
                ((struct SPI_Memory *)mapped_memory)->outgoing_midi[oi + 2] = byte[2];
                ((struct SPI_Memory *)mapped_memory)->outgoing_midi[oi + 3] =(unsigned char)knobs[knob];
                printf("Sending CC to external %x %x %x %d\n", out_cable << 4 | 0xb, 0xb0, byte[2], byte[3]);

                out_index++;
            }

            if (byte[1] == 0x80 && cable == 0)
            {
                int channel = 1 - 1;
                int out_cable = 2;

                int oi = out_index;

                ((struct SPI_Memory *)mapped_memory)->outgoing_midi[oi + 0] = out_cable << 4 | 0x8;
                ((struct SPI_Memory *)mapped_memory)->outgoing_midi[oi + 1] = 0x80 | channel;
                ((struct SPI_Memory *)mapped_memory)->outgoing_midi[oi + 2] = byte[2];
                ((struct SPI_Memory *)mapped_memory)->outgoing_midi[oi + 3] = 100;
                printf("Sending Note Off %x %x %x %d\n", cable << 4 | 0x8, 0x80 | channel, byte[2], 100);

                out_index++;
            }

            printf("out index: %d\n", out_index);
        }

        // memset(((struct SPI_Memory *)mapped_memory)->incoming_midi, 0, 256);
    }

    if (munmap(mapped_memory, length) == -1)
    {
        perror("munmap");
    }

    close(fd);

    return 0;
}