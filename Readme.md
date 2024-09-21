conan install . --output-folder=build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
cmake --build . --config Release

clang-tidy src/Main.cpp  *>$null