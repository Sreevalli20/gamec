@echo off
echo Building ChronoRaid...
if not exist build mkdir build
cd build
cmake -S .. -B . -G "Visual Studio 17 2022" -A x64
if %ERRORLEVEL% NEQ 0 (
    echo CMake configuration failed
    exit /b 1
)
cmake --build . --config Release
if %ERRORLEVEL% NEQ 0 (
    echo Build failed
    exit /b 1
)
echo Build successful!
cd ..
