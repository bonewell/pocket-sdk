#!/usr/bin/env bash

wasm=$1

if [ -z ${wasm} ]; then
  echo "Build native"
  clang++ -std=c++17 -o ctree christmas_tree.cpp `sdl2-config --cflags --libs`
else
  echo "Build wasm"
  emcc -c christmas_tree.cpp -o ctree.o -s USE_SDL=2 --std=c++17
  emcc ctree.o -o ctree.html -s USE_SDL=2
fi

