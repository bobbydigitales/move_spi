#!/bin/bash
gcc -g3 -shared -fPIC -o libhookmmap.so hook_mmap.c -ldl
