#!/usr/bin/env bash

project=$1
wasm=$2

if [ -z ${wasm} ]; then
  echo "Build native"
  c++ -std=c++17 -o ${project} build/${project}.cpp `sdl2-config --cflags --libs`
else
  echo "Build wasm"
  emcc -c ${project}.cpp -o build/${project}.o -s USE_SDL=2 --std=c++17
  emcc build/${project}.o -o build/${project}.html -s USE_SDL=2
fi

