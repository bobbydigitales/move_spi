# Build
I'm currently building on a raspberry pi and copying the binary to the Move. Could very easily cross-compile using Zig

gcc -g mmap.c -o mmap

gcc -g midi_monitor.c -o midi_monitor


# Run hook mmap
LD_PRELOAD=/data/UserData/Projects/move_spi/libhookmmap.so /opt/move/MoveMessageDisplay --message "O O O O"
