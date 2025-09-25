cmake -S . -B build -G "MinGW Makefiles"
cd build
cmake --build .
start EPOCH.exe --debug info
echo %ERRORLEVEL%