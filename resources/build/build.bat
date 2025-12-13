@echo off

cd /d "%~dp0\..\.."

echo Cleaning up...
IF EXIST Anime4K-GUI.syso (
  del Anime4K-GUI.syso
)

echo Generating RC data...
windres resources\build\app.rc -O coff -o Anime4K-GUI.syso

IF EXIST build (
  IF NOT "%1"=="--keep" (
    rmdir /s /Q build
  )
)
IF NOT EXIST build (
  mkdir build
)

echo Building project...
go build -ldflags "-s -w -H=windowsgui -extldflags=-static" -o build\Anime4K-GUI.exe

echo Copying runtime resources...
IF NOT EXIST build\shaders (
  xcopy resources\shaders build\shaders\ /E > NUL
)
IF NOT EXIST build\ffmpeg (
  xcopy resources\ffmpeg build\ffmpeg\ /E > NUL
)

del Anime4K-GUI.syso

echo.
echo Compilation finished successfully
pause
