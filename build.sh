cmake -S . -B build -G "Ninja"
export LD_LIBRARY_PATH=/src/engine/third-party/lib/fmod/fmod-linux:$LD_LIBRARY_PATH
cd build
cmake --build .
./SHA.me