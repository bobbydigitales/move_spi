#define _GNU_SOURCE
#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <dlfcn.h>
#include <sys/mman.h>
#include <unistd.h>
#include <fcntl.h>


unsigned char *global_mmap_addr = NULL;
FILE* output_file;
int frame_counter = 0;

void write_audio()
{
    if (!output_file) {
        return;
    }

    fwrite(&global_mmap_addr[2048 + 256], 512, 1, output_file);
    sync();

    frame_counter++;
}

void *(*real_mmap)(void *, size_t, int, int, int, off_t) = NULL;

void *mmap(void *addr, size_t length, int prot, int flags, int fd, off_t offset)
{
    if (!real_mmap)
    {
        real_mmap = dlsym(RTLD_NEXT, "mmap");
        if (!real_mmap)
        {
            fprintf(stderr, "Error: dlsym failed to find mmap\n");
            exit(1);
        }
    }

    void *result = real_mmap(addr, length, prot, flags, fd, offset);


    if (length == 4096) {
        global_mmap_addr = result;
    }

    printf("mmap hooked! addr=%p, length=%zu, prot=%d, flags=%d, fd=%d, offset=%ld, result=%p\n",
           addr, length, prot, flags, fd, offset, result);

           
    output_file = fopen("spi_memory.txt", "w+");
           
    // print_mem();
    // write_mem();

    return result;
}

// // The actual implementation function that gets the real ioctl
// static int (*get_real_ioctl(void))(int, unsigned long, ...) {
//     int (*real_ioctl)(int, unsigned long, ...) = dlsym(RTLD_NEXT, "ioctl");
//     if (!real_ioctl) {
//         fprintf(stderr, "Error getting real ioctl: %s\n", dlerror());
//         //errno = ENOSYS;
//     }
//     return real_ioctl;
// }

// // Macro to wrap ioctl calls
// #define ioctl(fd, request, ...) \
//     ({ \
//         printf("hello world\n"); \
//         int (*real_ioctl)(int, unsigned long, ...) = get_real_ioctl(); \
//         real_ioctl ? real_ioctl(fd, request, ##__VA_ARGS__) : -1; \
//     })

// Function pointer to the real ioctl
int (*real_ioctl)(int, unsigned long, char *) = NULL;

// Our hooked ioctl function


// read the number of arguments passed and fail if it's more than we're supporting.
int ioctl(int fd, unsigned long request, char *argp)
{
    // Initialize the real_ioctl function pointer if it's not already set
    if (!real_ioctl)
    {
        real_ioctl = dlsym(RTLD_NEXT, "ioctl");
        if (!real_ioctl)
        {
            fprintf(stderr, "Error: dlsym failed to find ioctl\n");
            exit(1);
        }
    }

    // print_mem();
    write_audio();

    int result = real_ioctl(fd, request, argp);

    return result;
}

// // Function pointer to the real futex
// int (*real_futex)(int *, int, int, const struct timespec *, int *, int) = NULL;

// // Our hooked futex function
// int futex(int *uaddr, int futex_op, int val, const struct timespec *timeout, int *uaddr2, int val3) {
//     // Initialize the real_futex function pointer if it's not already set
//     if (!real_futex) {
//         real_futex = dlsym(RTLD_NEXT, "futex");
//         if (!real_futex) {
//             fprintf(stderr, "Error: dlsym failed to find futex\n");
//             exit(1);
//         }
//     }

//     // Print "hello world" every time futex is called
//     printf("hello world\n");

//     // Call the real futex function with the same arguments
//     return real_futex(uaddr, futex_op, val, timeout, uaddr2, val3);
// }
