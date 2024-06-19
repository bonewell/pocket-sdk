#!/usr/bin/env bash

path=$1
wasm=$2

dir=${path%/*}
project=${path##*/}

mkdir -p build/${dir}
if [ -z ${wasm} ]; then
  echo "Build native"
  c++ -std=c++17 -I. -o build/${dir}/${project} ${path}.cpp `sdl2-config --cflags --libs`
else
  echo "Build wasm"
  emcc -c ${path}.cpp -o build/${dir}/${project}.o -s USE_SDL=2 --std=c++17 -I.
  emcc build/${dir}/${project}.o -o build/${dir}/${project}.html -s USE_SDL=2
fi

