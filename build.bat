@echo off
REM Build script for Windows
REM Usage: build.bat or build.bat clean

setlocal enabledelayedexpansion

if "%1"=="clean" (
    echo Cleaning build artifacts...
    del /f /q mylib.dll mylib.o 2>nul
    if exist mylib.dll (
        echo Error: Could not delete mylib.dll
        exit /b 1
    )
    echo Build cleaned successfully
    exit /b 0
)

echo Compiling mylib.c to mylib.dll...
gcc -fPIC -shared -o mylib.dll mylib.c -Wall -Wextra -O2

if %errorlevel% neq 0 (
    echo Compilation failed!
    exit /b 1
)

echo.
echo ============================================
echo Build successful: mylib.dll created
echo ============================================
echo.
echo Run the examples:
echo   python mylib_wrapper.py
echo   python simple_example.py
echo.
exit /b 0
