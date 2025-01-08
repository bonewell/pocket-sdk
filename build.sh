#!/usr/bin/env bash

path=$1
wasm=$2

if [ -z ${wasm} ]; then
  echo "Native mode"
else
  echo "Wasm mode"
fi

build_project()
{
  path=$1
  dir=${path%/*}
  file=${path##*/}
  project=${file%.*}

  echo "Build ${project}"
  mkdir -p build/${dir}
  if [ -z ${wasm} ]; then
    c++ -std=c++17 -I. -o build/${dir}/${project} ${path} `sdl2-config --cflags --libs`
  else
    emcc -c ${path} -o build/${dir}/${project}.o -s USE_SDL=2 --std=c++17 -I.
    emcc build/${dir}/${project}.o -o build/${dir}/${project}.html -s USE_SDL=2
  fi
}

if [ "${path}" == "examples" ]; then
  for project_path in `find examples -type f`
  do
    build_project ${project_path}
  done
else
  build_project ${path}
fi
