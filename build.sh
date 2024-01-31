#!/bin/bash

rm -rf ./build
mkdir -p ./build
g++ -Wall -Werror -Wpedantic -g ./src/sdl_handmade.cpp -o ./build/handmade `sdl2-config --cflags --libs`
./build/handmade
