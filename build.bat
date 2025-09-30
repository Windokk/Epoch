cmake -S . -B build -G "MinGW Makefiles"
cd build
cmake --build .
start /wait EPOCH.exe --debug warning
echo %ERRORLEVEL%