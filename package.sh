#!/bin/bash
cp -vf ./control_surface_move move_m8_lpp.js move_default.js start_control_surface_move.sh stop_control_surface_move.sh ./build/control_surface_move
tar -C ./build/ -czvf control_surface_move.tar.gz control_surface_move