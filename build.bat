@echo off
REM Quiz Game Build Script for Windows
echo ===================================
echo    Quiz Game Build Script - Windows
echo ===================================
echo.

REM Check for compiler (MSVC or MinGW)
where g++ >nul 2>nul
if %errorlevel% equ 0 (
    set COMPILER=g++
    echo Found MinGW g++ compiler
) else (
    where cl >nul 2>nul
    if %errorlevel% equ 0 (
        set COMPILER=cl
        echo Found MSVC compiler
    ) else (
        echo Error: No C++ compiler found!
        echo Please install MinGW or Visual Studio Build Tools
        echo   MinGW: https://www.mingw-w64.org/
        echo   Visual Studio: https://visualstudio.microsoft.com/downloads/#build-tools-for-visual-studio-2022
        pause
        exit /b 1
    )
)

REM Create build directory if it doesn't exist
if not exist build (
    echo Creating build directory...
    mkdir build
)

REM Navigate to QuizGame directory
cd QuizGame
if %errorlevel% neq 0 (
    echo Error: QuizGame directory not found!
    pause
    exit /b 1
)

REM Compile the game
echo Compiling Quiz Game...
if "%COMPILER%"=="g++" (
    g++ -std=c++11 MINI_GAME.cpp -o ..\build\quiz_game.exe
) else (
    cl /EHsc MINI_GAME.cpp /Fe..\build\quiz_game.exe
)

REM Check if compilation was successful
if %errorlevel% equ 0 (
    echo [SUCCESS] Compilation successful!
    echo Executable created at: build\quiz_game.exe
    
    REM Copy question files to build directory
    echo Copying game data files...
    copy *.txt ..\build\ >nul
    echo [SUCCESS] Game data files copied successfully!
    
    echo.
    echo ===================================
    echo    Build Complete!                 
    echo ===================================
    echo To run the game:
    echo   cd build ^&^& quiz_game.exe
    echo.
) else (
    echo [ERROR] Compilation failed!
    pause
    exit /b 1
)

cd ..
pause