# pocket-sdk
Header only SDK for Mobile C [C/C++ Compiler] to develop on mobile devices

# Build and Run example in Mobile C [C/C++ Compiler] applicatioin

1. Copy `*.cpp` file from examples into root directory of `pocket-sdk`.
2. Tap `Run` button in the application.

# Build project on computer
```./build.sh <path> <mode>```

## Build Native
```./build.sh examples/ctree.cpp```

## Build Webassembly
```./build.sh examples/ctree.cpp wasm```

## Build all examples
```./build.sh examples```

# Dependencies
1. SDL2 for GUI examples
2. Emscripten for Webassembly
