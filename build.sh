#!/bin/bash

# Quiz Game Build Script for Linux/macOS
echo "==================================="
echo "   Quiz Game Build Script - Unix   "
echo "==================================="

# Check if g++ is installed
if ! command -v g++ &> /dev/null; then
    echo "Error: g++ compiler not found!"
    echo "Please install g++ using your package manager:"
    echo "  Ubuntu/Debian: sudo apt install g++"
    echo "  Fedora: sudo dnf install gcc-c++"
    echo "  macOS: xcode-select --install"
    exit 1
fi

# Create build directory if it doesn't exist
if [ ! -d "build" ]; then
    echo "Creating build directory..."
    mkdir -p build
fi

# Navigate to QuizGame directory
cd QuizGame || { echo "Error: QuizGame directory not found!"; exit 1; }

# Check which source file exists
if [ -f "MAIN_GAME.cpp" ]; then
    SOURCE_FILE="MAIN_GAME.cpp"
    echo "Found MAIN_GAME.cpp"
elif [ -f "MINI_GAME.cpp" ]; then
    SOURCE_FILE="MINI_GAME.cpp"
    echo "Found MINI_GAME.cpp"
else
    echo "Error: No game source file found! (Looking for MAIN_GAME.cpp or MINI_GAME.cpp)"
    exit 1
fi

# Compile the game
echo "Compiling Quiz Game using $SOURCE_FILE..."
g++ -std=c++11 "$SOURCE_FILE" -o ../build/quiz_game

# Check if compilation was successful
if [ $? -eq 0 ]; then
    echo "✅ Compilation successful!"
    echo "Executable created at: build/quiz_game"
    
    # Copy question files to build directory
    echo "Copying game data files..."
    cp *.txt ../build/ 2>/dev/null
    if [ $? -eq 0 ]; then
        echo "✅ Game data files copied successfully!"
    else
        echo "⚠️  No text files found to copy or copy failed."
    fi
    
    # Make executable runnable
    chmod +x ../build/quiz_game
    
    echo ""
    echo "==================================="
    echo "   Build Complete!                 "
    echo "==================================="
    echo "To run the game:"
    echo "  cd build && ./quiz_game"
    echo ""
else
    echo "❌ Compilation failed!"
    exit 1
fi

# Return to root directory
cd ..