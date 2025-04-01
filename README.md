# Tetris Game in C++

## Overview
This is a simple console-based Tetris game implemented in **C++** using Windows API functions. The game features a fully functional Tetris experience with piece rotation, movement, collision detection, line clearing, and scoring.

## Features
- **7 Classic Tetris Pieces**: The game includes the standard Tetris blocks (I, O, T, S, Z, J, and L).
- **Rotation Mechanics**: Pieces can be rotated using the rotation function.
- **Collision Detection**: Ensures that blocks do not overlap or go outside the playing area.
- **Line Clearing**: Full lines are removed, and the score is updated accordingly.
- **Scoring System**: Players earn points for clearing lines.
- **Game Over Condition**: The game ends when a new piece cannot be placed.

## Controls
- **Left Arrow (←)**: Move piece left
- **Right Arrow (→)**: Move piece right
- **Down Arrow (↓)**: Move piece down faster
- **Z Key**: Rotate piece

## How to Compile and Run

### Prerequisites
- A C++ compiler supporting **C++11** or later (MinGW, MSVC, etc.).
- Windows OS (since it uses Windows API functions).

### Compilation (Using g++)
```bash
g++ tetris.cpp -o tetris.exe -std=c++11 -static-libgcc -static-libstdc++
