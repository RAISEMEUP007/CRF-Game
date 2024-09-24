# Install dependencies with conan

``` bash
conan install . --output-folder=build
```

# Cmake configuration and build

```bash
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
cmake --build . --config Release
```

# Check Clang-Tidy

## Windows

```bash
clang-tidy src/*.cpp -- -IC:\Users\mtaim\.conan2\p\rayliab5c2bc5df219\p\include -IC:\Users\mtaim\.conan2\p\flecsc7a7a62b5e5af\p\include
```

## Linux

```bash
clang-tidy src/*.cpp -- -I$HOME/.conan2/p/raylid3c36e0e7a0e2/p/include -I$HOME/.conan2/p/flecsd4ab11213583a/p/include
```