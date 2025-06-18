#!/bin/bash
gcc -g control_surface_move.c -o control_surface_move -Llibs/quickjs/quickjs-2025-04-26/ -lquickjs -lm
cp -vf ./control_surface_move move_m8_lpp.js move_default.js move_virtual_knobs.mjs midi_messages.mjs aftertouch_to_modwheel.mjs start_control_surface_move.sh stop_control_surface_move.sh ./build/control_surface_move
ssh ableton@move killall control_surface_move
scp  -r ./build/* ableton@move:~/Projects/move_spi/build/