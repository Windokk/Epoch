cmake -S . -B build -G "MinGW Makefiles"
cd build
cmake --build .
start /wait Epoch.exe
echo %ERRORLEVEL%