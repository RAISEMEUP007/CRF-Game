# Install dependencies with conan
conan install . --output-folder=build

# Cmake configuration and build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
cmake --build . --config Release