#!/bin/bash
gcc -g control_surface_move.c -o control_surface_move -Llibs/quickjs/quickjs-2025-04-26/ -lquickjs -lm
cp -vf ./control_surface_move move_m8.js move_default.js start_control_surface_move.sh stop_control_surface_move.sh ./build/control_surface_move

scp  -r ./build/control_surface_move ableton@move.local:~/Projects/move_spi/build/
# ./copy.sh control_surface_move
# scp move_m8.js ableton@move:./Projects/move_spi/
# scp move_default.js ableton@move:./Projects/move_spi/