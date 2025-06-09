#!/bin/bash
gcc -g $1.c -o $1
ssh ableton@move killall -9 midi_monitor 
scp $1 ableton@move:./Projects/move_spi/ 
#gcc -g $1.c -o $1 && ssh ableton@move killall -9 midi_monitor && scp $1 ableton@move:./Projects/move_spi/ 
