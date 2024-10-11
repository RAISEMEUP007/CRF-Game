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
clang-tidy src/*.cpp src/lib/*.cpp -- -isystem $env:USERPROFILE\.conan2\p\rayliab5c2bc5df219\p\include -isystem $env:USERPROFILE\.conan2\p\flecsc7a7a62b5e5af\p\include -isystem $env:USERPROFILE\.conan2\p\dia0f6529019f9b\p\include -isystem $env:USERPROFILE\.conan2\p\sml3616c12de77ec\p\include -Werror
```

## Linux

```bash
clang-tidy src/*.cpp -- -isystem $HOME/.conan2/p/raylid3c36e0e7a0e2/p/include -isystem $HOME/.conan2/p/flecsd4ab11213583a/p/include -isystem $HOME/.conan2/p/dia0f6529019f9b/p/include -isystem $HOME/.conan2/p/sml3616c12de77ec/p/include
```

# VSCode configuration

## Ignoring test files

```json
"clang-tidy.blacklist": [".*Test\\.cpp$"],
```

# CPPCHECK

```bash
cppcheck --enable=all --suppress=missingIncludeSystem --suppress=missingInclude --check-level=exhaustive src
```

# CPPLINT

```bash
cpplint --linelength=120 --filter=-build/include_subdir,-legal/copyright --recursive src
```
