#include <iostream>
#include <Windows.h>
#include <thread>
#include <string>

using namespace std;

// Game Constants
const int SPACE_WIDTH = 12;        // Game field width (including borders)
const int SPACE_HEIGHT = 18;       // Game field height (including floor)
const int SCREEN_WIDTH = 80;       // Console screen width 
const int SCREEN_HEIGHT = 30;      // Console screen height

// Game Pieces (Tetrominoes)
wstring tetrominoes[7];            // Array to store all 7 standard Tetris pieces
unsigned char* pField = nullptr;   // Pointer to game field (play area)

// Function to rotate a point in a 4x4 matrix
int Rotate(int px, int py, int rotation) {
    switch (rotation % 4) {
        case 0: return py * 4 + px;        // 0 degrees
        case 1: return 12 + py - (px * 4); // 90 degrees
        case 2: return 15 - (py * 4) - px; // 180 degrees
        case 3: return 3 - py + (px * 4);   // 270 degrees
    }
    return 0; // Default case (should never be reached)
}

// Check if a piece fits in the current position
bool DoesPieceFit(int tetromino, int rotation, int posX, int posY) {
    for (int px = 0; px < 4; px++) {
        for (int py = 0; py < 4; py++) {
            // Get index after rotation
            int pieceIndex = Rotate(px, py, rotation);
            
            // Get index in the field
            int fieldIndex = (posY + py) * SPACE_WIDTH + (posX + px);
            
            // Check boundaries
            if (posX + px >= 0 && posX + px < SPACE_WIDTH) {
                if (posY + py >= 0 && posY + py < SPACE_HEIGHT) {
                    // Check for collision with existing blocks
                    if (tetrominoes[tetromino][pieceIndex] == L'X' && pField[fieldIndex] != 0) {
                        return false;
                    }
                }
            }
        }
    }
    return true;
}

int main() {
    // Initialize Tetrominoes (7 standard Tetris pieces)
    // I-piece
    tetrominoes[0].append(L"..X.");
    tetrominoes[0].append(L"..X.");
    tetrominoes[0].append(L"..X.");
    tetrominoes[0].append(L"..X.");
    
    // O-piece
    tetrominoes[1].append(L"....");
    tetrominoes[1].append(L".XX.");
    tetrominoes[1].append(L".XX.");
    tetrominoes[1].append(L"....");
    
    // T-piece
    tetrominoes[2].append(L".X..");
    tetrominoes[2].append(L".XX.");
    tetrominoes[2].append(L"..X.");
    tetrominoes[2].append(L"....");
    
    // L-piece
    tetrominoes[3].append(L"..X.");
    tetrominoes[3].append(L".XX.");
    tetrominoes[3].append(L"..X.");
    tetrominoes[3].append(L"....");
    
    // J-piece
    tetrominoes[4].append(L"....");
    tetrominoes[4].append(L".XX.");
    tetrominoes[4].append(L"..X.");
    tetrominoes[4].append(L"..X.");
    
    // S-piece
    tetrominoes[5].append(L"....");
    tetrominoes[5].append(L".XX.");
    tetrominoes[5].append(L".X..");
    tetrominoes[5].append(L".X..");
    
    // Z-piece
    tetrominoes[6].append(L"..X.");
    tetrominoes[6].append(L".XX.");
    tetrominoes[6].append(L".X..");
    tetrominoes[6].append(L"....");

    // Create playing field
    pField = new unsigned char[SPACE_WIDTH * SPACE_HEIGHT];
    for (int x = 0; x < SPACE_WIDTH; x++) {
        for (int y = 0; y < SPACE_HEIGHT; y++) {
            // 9 represents borders, 0 is empty space
            pField[y * SPACE_WIDTH + x] = (x == 0 || x == SPACE_WIDTH - 1 || y == SPACE_HEIGHT - 1) ? 9 : 0;
        }
    }

    // Create screen buffer
    wchar_t* screen = new wchar_t[SCREEN_WIDTH * SCREEN_HEIGHT];
    for (int i = 0; i < SCREEN_WIDTH * SCREEN_HEIGHT; i++) {
        screen[i] = L' ';
    }
    
    // Initialize console
    HANDLE hConsole = CreateConsoleScreenBuffer(GENERIC_READ | GENERIC_WRITE, 0, NULL, CONSOLE_TEXTMODE_BUFFER, NULL);
    SetConsoleActiveScreenBuffer(hConsole);
    DWORD dwBytesWritten = 0;

    // Game variables
    int currentPiece = rand() % 7;  // Random starting piece
    int currentRotation = 0;
    int currentX = SPACE_WIDTH / 2;
    int currentY = 0;
    bool key[4];                    // Right, Left, Down, Rotate
    bool rotateHold = true;         // Prevents multiple rotations on single key press
    
    int speed = 20;                 // Game speed (lower is faster)
    int speedCounter = 0;
    bool forceDown = false;
    int score = 0;
    bool gameOver = false;

    // Main game loop
    while (!gameOver) {
        // Game timing
        this_thread::sleep_for(50ms);
        speedCounter++;
        forceDown = (speedCounter == speed);

        // Input handling
        for (int k = 0; k < 4; k++) {  // Right, Left, Down, Rotate (Z key)
            key[k] = (0x8000 & GetAsyncKeyState((unsigned char)("\x27\x25\x28Z"[k]))) != 0;
        }

        // Game logic
        // Handle movement
        if (key[1] && DoesPieceFit(currentPiece, currentRotation, currentX - 1, currentY)) {
            currentX--;  // Move left
        }
        if (key[0] && DoesPieceFit(currentPiece, currentRotation, currentX + 1, currentY)) {
            currentX++;  // Move right
        }
        if (key[2] && DoesPieceFit(currentPiece, currentRotation, currentX, currentY + 1)) {
            currentY++;  // Move down
        }

        // Handle rotation
        if (key[3]) {
            if (DoesPieceFit(currentPiece, currentRotation + 1, currentX, currentY) && rotateHold) {
                currentRotation++;
                rotateHold = false;
            }
        } else {
            rotateHold = true;
        }

        // Force piece down and handle locking
        if (forceDown) {
            if (DoesPieceFit(currentPiece, currentRotation, currentX, currentY + 1)) {
                currentY++;  // Move down
            } else {
                // Lock the piece in place
                for (int px = 0; px < 4; px++) {
                    for (int py = 0; py < 4; py++) {
                        if (tetrominoes[currentPiece][Rotate(px, py, currentRotation)] == L'X') {
                            pField[(currentY + py) * SPACE_WIDTH + (currentX + px)] = currentPiece + 1;
                        }
                    }
                }

                // Check for completed lines
                for (int py = 0; py < 4; py++) {
                    if (currentY + py < SPACE_HEIGHT - 1) {
                        bool lineComplete = true;
                        for (int px = 1; px < SPACE_WIDTH - 1; px++) {
                            lineComplete &= (pField[(currentY + py) * SPACE_WIDTH + px]) != 0;
                        }

                        if (lineComplete) {
                            // Remove the line and shift above lines down
                            for (int px = 1; px < SPACE_WIDTH - 1; px++) {
                                for (int py2 = currentY + py; py2 > 0; py2--) {
                                    pField[py2 * SPACE_WIDTH + px] = pField[(py2 - 1) * SPACE_WIDTH + px];
                                }
                                pField[px] = 0;  // Clear top line
                            }
                            score += 25;  // Increase score
                        }
                    }
                }

                // Prepare next piece
                currentX = SPACE_WIDTH / 2;
                currentY = 0;
                currentRotation = 0;
                currentPiece = rand() % 7;

                // Check if game over (new piece doesn't fit)
                gameOver = !DoesPieceFit(currentPiece, currentRotation, currentX, currentY);
            }
            speedCounter = 0;
        }

        // Draw score
        swprintf_s(&screen[2 * SCREEN_WIDTH + SPACE_HEIGHT + 6], 16, L"SCORE: %8d", score);

        // Draw field
        for (int x = 0; x < SPACE_WIDTH; x++) {
            for (int y = 0; y < SPACE_HEIGHT; y++) {
                // Characters: " OOOOOOO=#" represent different block types
                screen[(y + 2) * SCREEN_WIDTH + (x + 2)] = L" OOOOOOO=#"[pField[y * SPACE_WIDTH + x]];
            }
        }

        // Draw current piece
        for (int px = 0; px < 4; px++) {
            for (int py = 0; py < 4; py++) {
                if (tetrominoes[currentPiece][Rotate(px, py, currentRotation)] == L'X') {
                    screen[(currentY + py + 2) * SCREEN_WIDTH + (currentX + px + 2)] = L'O';
                }
            }
        }

        // Display frame
        WriteConsoleOutputCharacterW(hConsole, screen, SCREEN_WIDTH * SCREEN_HEIGHT, { 0, 0 }, &dwBytesWritten);
    }

    // Cleanup and game over message
    CloseHandle(hConsole);
    cout << "Game Over! Final Score: " << score << endl;
    system("pause");

    return 0;
}