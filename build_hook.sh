#!/bin/bash
gcc -g3 -shared -fPIC -o $1.so $1.c -ldl && scp $1.so ableton@move:./Projects/move_spi/