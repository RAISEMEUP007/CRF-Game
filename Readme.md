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