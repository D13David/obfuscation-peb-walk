@echo off
setlocal

set BUILD_DIR=build
set GENERATOR="Visual Studio 17 2022"
set CONFIG=Release

rmdir /s /q %BUILD_DIR%

echo Generating solution...
cmake -S . -B %BUILD_DIR% -G %GENERATOR% -DCMAKE_BUILD_TYPE=%CONFIG%

echo Building solution...
cmake --build %BUILD_DIR% --config %CONFIG%

echo Done.
endlocal
pause
