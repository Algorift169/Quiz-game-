# Quiz Game in C++

A console-based quiz game built with C++, featuring multiple categories to test your knowledge across different domains.

## 🎮 Features

- **Multiple Categories**:
  - 💻 Programming Knowledge
  - 📜 History
  - ➗ Mathematics
  - 🌍 General Knowledge

- **Rich Question Bank**: Hundreds of questions stored in external text files
- **Multiple Choice**: Four options per question with correct answer validation
- **Progress Tracking**: Automatic progress saving to continue where you left off
- **Score System**: Tracks highest scores and category-wise progress

## 📁 Project Structure
Quiz-game-/
├── QuizGame/
│ ├── MINI_GAME.cpp # Main game source code
│ ├── MINI_GAME.exe # Windows executable
│ ├── GENERAL_KNOWLEDGE_.txt # General knowledge questions/answers
│ ├── HISTORY_.txt # History questions/answers
│ ├── MATH_.txt # Mathematics questions/answers
│ ├── PROGRAMMING_.txt # Programming questions/answers
│ └── USER_PROGRESS.txt # Player progress tracking
├── build/ # Build output directory
├── build.sh # Unix/Linux/macOS build script
├── build.bat # Windows build script
└── README.md # This file


## 🚀 How to Build and Run

### Prerequisites

- **C++ Compiler**:
  - **Linux/macOS**: GCC (g++) or Clang
  - **Windows**: MinGW-w64 or Visual Studio Build Tools

### Building the Game

#### For Linux/macOS Users

1. Open terminal in the root directory (`Quiz-game-`)
2. Make the build script executable:
   ```bash
   chmod +x build.sh
   Run the build script:
bash

./build.sh

Run the game:
bash

cd build && ./quiz_game

For Windows Users

    Open Command Prompt or PowerShell in the root directory (Quiz-game-)

    Run the build script:
    batch

build.bat

Run the game:
batch

cd build && quiz_game.exe

Manual Build (Alternative)
Linux/macOS:
bash

cd QuizGame
g++ -std=c++11 MINI_GAME.cpp -o quiz_game
./quiz_game

Windows (MinGW):
batch

cd QuizGame
g++ -std=c++11 MINI_GAME.cpp -o quiz_game.exe
quiz_game.exe

🎯 How to Play

    Launch the game using the instructions above

    Select a category from the menu

    Answer multiple-choice questions

    View your score at the end

    Progress is automatically saved for next time

📊 Progress Tracking

The game automatically saves:

    Your highest score per category

    Questions answered

    Overall progress

    Last played category

Progress is stored in USER_PROGRESS.txt and will be loaded automatically when you restart the game.
🤝 Contributing

Feel free to add more questions to the existing categories or suggest new features!