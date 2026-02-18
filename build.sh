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

# Compile the game
echo "Compiling Quiz Game..."
g++ -std=c++11 MINI_GAME.cpp -o ../build/quiz_game

# Check if compilation was successful
if [ $? -eq 0 ]; then
    echo "✅ Compilation successful!"
    echo "Executable created at: build/quiz_game"
    
    # Copy question files to build directory
    echo "Copying game data files..."
    cp *.txt ../build/ 2>/dev/null
    echo "✅ Game data files copied successfully!"
    
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