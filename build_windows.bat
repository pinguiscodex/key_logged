@echo off
echo Building Windows executable...
if exist "C:\mingw64\bin\gcc.exe" (
    "C:\mingw64\bin\gcc.exe" -o keylogger.exe -D_WIN32 keylogger_cross_platform.c -luser32
) else if exist "C:\MinGW\bin\gcc.exe" (
    "C:\MinGW\bin\gcc.exe" -o keylogger.exe -D_WIN32 keylogger_cross_platform.c -luser32
) else (
    echo Error: MinGW-w64 not found. Please install MinGW-w64 and add it to your PATH.
    pause
    exit /b 1
)
echo Windows executable built successfully!
pause