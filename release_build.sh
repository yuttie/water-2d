#!/bin/sh
gcc -O3 -Wall -std=gnu99 main.c imgscale.c -o water `sdl-config --cflags --libs`
