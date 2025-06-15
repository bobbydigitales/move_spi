#!/bin/bash
cp -vf ./control_surface_move move_m8.js move_default.js start_control_surface_move.sh stop_control_surface_move.sh ./build/control_surface_move
tar -czvf control_surface_move.tar.gz ./build/control_surface_move