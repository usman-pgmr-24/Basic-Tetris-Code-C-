#include <iostream> // Needed for output (std::cout)
#include <windows.h> // Needed for console manipulation (SetConsoleCursorPosition, Sleep)

// Playfield and block definitions
int pl[150]; // Playfield is 15x10
// Block configurations (binary 3x3 maps of 4 orientations for each block type)
int fig[160] = {
    56, 146, 56, 146, // L-block
    58, 178, 184, 154, // T-block
    312, 210, 57, 150, // Square block
    27, 27, 27, 27 // Line block
};

// Game state variables
int off = 1; // Block position offset (1 row = 10 units)
int r = 1; // Game running flag (1 = running, 0 = game over)
int i, j, j2, t, k, dd, pt, spd, dir, nfig; // Helper variables
// i, j, j2: Loop counters
// t: Time counter, k: Collision flag, dd: Key delay
// pt: Points, spd: Speed flag, dir: Block orientation, nfig: Block type

int np[2]; // Stores new block position for collision testing
COORD c = {0, 0}; // Cursor position for console output

int main(int argc, char** argv) {
    // Initialize the playfield (edges set to 1, rest to 0)
    for (i = 0; i < 150; i++) 
        pl[i] = (i % 10 == 0 || i % 10 == 8 || i > 140);

    while (r) { // Main game loop
        // Update the console cursor position and display points
        SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), c);
        std::cout << "Points: " << pt << "     " << '\n';

        // Advance time and move the block down
        t++;
        off += 10 * (t % (10 - 9 * spd) == 0); // Move down based on speed
        if (dd > 0) dd--; // Decrease key delay

        // Draw the block on the playfield and check for collisions
        k = 0; // Reset collision flag
        for (i = 0; i < 9; i++) {
            k += pl[off + i % 3 + i / 3 * 10 + 10] * ((fig[nfig * 4 + dir] & 1 << i) > 0);
            pl[off + i % 3 + i / 3 * 10] += ((fig[nfig * 4 + dir] & 1 << i) > 0);
        }

        // Render the playfield
        for (i = 0; i < 150; i++)
            std::cout << (char)(32 + 3 * pl[i] - 22 * (i % 10 == 9));

        // Handle collision
        if (k == 0) { // No collision
            for (i = 0; i < 9; i++) 
                pl[off + i % 3 + i / 3 * 10] -= ((fig[nfig * 4 + dir] & 1 << i) > 0);
        } else { // Collision occurred
            if (off < 10) r = 0; // Game over if collision at top
            off = 1; // Reset position
            nfig = rand() % 4; // Spawn new block
            dir = 0;
            spd = 0;
        }

        Sleep(50); // Regulate speed

        // Save current block position and orientation
        np[1] = off;
        np[2] = dir;

        // Handle user input
        if ((GetKeyState(VK_ESCAPE) & 0x8000)) r = 0; // Quit
        if ((GetKeyState(VK_RIGHT) & 0x8000) && (off % 10 < 6)) off++; //right
        if ((GetKeyState(VK_LEFT) & 0x8000) && (off % 10 > 0)) off--; //left
        if ((GetKeyState(VK_DOWN) & 0x8000)) spd = 1; // Speed up
        if ((GetKeyState(VK_UP) & 0x8000) && dd == 0) { // Rotate
            dir = (dir + 1) % 4;
            dd = 4;
        }

        // Collision detection for user input
        k = 0;
        for (i = 0; i < 9; i++)
            if (pl[off + i % 3 + i / 3 * 10] + ((fig[nfig * 4 + dir] & 1 << i) > 0) > 1)
                k = 1;
        if (k == 1) { // Reset if collision
            off = np[1];
            dir = np[2];
        }

        // Clear completed rows
        for (j = 0; j < 14; j++) {
            k = 1;
            for (i = 1; i < 8; i++)
                if (pl[i + j * 10] == 0) k = 0; // Check for full row

            if (k == 1) { // Row is full
                for (i = 1; i < 8; i++) pl[i + j * 10] = 0; // Clear row
                pt++; // Increment

                // Move rows 
                for (j2 = j; j2 > 0; j2--)
                    for (i = 1; i < 8; i++)
                        pl[i + j2 * 10] = pl[i + j2 * 10 - 10];
            }
        }
    } 
}
