@echo off
cd ..
cd ..

echo Cleaning up...
IF EXIST Anime4K-GUI.syso (
    del Anime4K-GUI.syso
)

IF EXIST build (
    rmdir /s /Q build
)

echo Generating RC data...
windres resources\build\app.rc -O coff -o Anime4K-GUI.syso

mkdir build

echo Building project...
go build -ldflags "-s -w -H=windowsgui -extldflags=-static" -o build\Anime4K-GUI.exe

echo Copying runtime resources...
xcopy resources\shaders build\shaders\ /E > NUL
xcopy resources\ffmpeg build\ffmpeg\ /E > NUL

del Anime4K-GUI.syso

echo.
echo Compilation finished successfully
pause