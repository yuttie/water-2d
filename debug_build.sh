#!/bin/sh
gcc -g -pg -Wall -std=gnu99 main.c imgscale.c -o water `sdl-config --cflags --libs`
