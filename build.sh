#!/usr/bin/env bash

wasm=$1

if [ -z ${wasm} ]; then
  echo "Build native"
  c++ -std=c++17 -o ctree ctree.cpp `sdl2-config --cflags --libs`
else
  echo "Build wasm"
  emcc -c ctree.cpp -o ctree.o -s USE_SDL=2 --std=c++17
  emcc ctree.o -o ctree.html -s USE_SDL=2
fi

