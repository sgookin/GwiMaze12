// libpng + png++
#pragma warning(disable : 4996) 
#include <iostream>
#include <vector>
#include <png++/png.hpp>

// Simple Struct to represent the x and y position of a cell
struct Coords {
public:
	int64_t x;
	int64_t y;

	Coords() {
		x = 0;
		y = 0;
	}

	Coords(int64_t xi, int64_t yi) {
		x = xi;
		y = yi;
	}
};

// Patterns for differnt cells images, X is black & 0 is white
const static char patterns[16][4][5] = {
	// in binary
	// 1 << 3 left, 1 << 2 top, 1 << 1 right, 1 bottom
	{ //0 all
		"XXXX",
		"X00X",
		"X00X",
		"XXXX"
	},
	{ //1 
		"XXXX",
		"X00X",
		"X00X",
		"X00X"
	},
	{ //10 
		"XXXX",
		"X000",
		"X000",
		"XXXX"
	},
	{ //11
		"XXXX",
		"X000",
		"X000",
		"X00X"
	},
	{ //100
		"X00X",
		"X00X",
		"X00X",
		"XXXX"
	},
	{ //101
		"X00X",
		"X00X",
		"X00X",
		"X00X"
	},
	{ //110
		"X00X",
		"X000",
		"X000",
		"XXXX"
	},
	{ //111
		"X00X",
		"X000",
		"X000",
		"X00X"
	},
	{ //1000
		"XXXX",
		"000X",
		"000X",
		"XXXX"
	},
	{ //1001
		"XXXX",
		"000X",
		"000X",
		"X00X"
	},
	{ //1010
		"XXXX",
		"0000",
		"0000",
		"XXXX"
	},
	{ //1011
		"XXXX",
		"0000",
		"0000",
		"X00X"
	},
	{ //1100
		"X00X",
		"000X",
		"000X",
		"XXXX"
	},
	{ //1101
		"X00X",
		"000X",
		"000X",
		"X00X"
	},
	{ //1110
		"X00X",
		"0000",
		"0000",
		"XXXX"
	},
	{ //1111
		"X00X",
		"0000",
		"0000",
		"X00X"
	}
};

int64_t w;
int64_t h;
int64_t xStart;
Coords endCoords;
std::vector<std::vector<int8_t>> data;

//Checks to see how many unvisited cells are orthogonally adjacent
int_fast8_t __stdcall getAvailable(Coords cell) {
	int_fast8_t pathsAvailable = 0;

	//check if current cell is the end cell
	if (cell.x == endCoords.x && cell.y == endCoords.y) return 0;

	//check left
	if (cell.x != 0 && data[cell.x - 1][cell.y] == 0) pathsAvailable++;

	//check right
	if (cell.x != w - 1 && data[cell.x + 1][cell.y] == 0) pathsAvailable++;

	//check top
	if (cell.y != 0 && data[cell.x][cell.y - 1] == 0) pathsAvailable++;

	//check bottom
	if (cell.y != h - 1 && data[cell.x][cell.y + 1] == 0) pathsAvailable++;

	return pathsAvailable;
}

//Updates cell data to remove wall
void __stdcall update(Coords coords, Coords result) {
	int_fast8_t config = data[coords.x][coords.y];
	switch (result.x)
	{
	case 0:
		if (result.y == 1)
		{
			// down
			config += 1;
		}
		else
		{
			// up
			config += 0b100;
		}
		break;
	case 1:
		// right
		config += 0b10;
		break;
	case -1:
		// left
		config += 0b1000;
		break;
	}
	data[coords.x][coords.y] = config;
}

int main()
{
	// Checking for potential overflow
	const int64_t maxLength = (int64_t)std::sqrt((double)(LLONG_MAX >> 2));
	std::cout << "Enter your desired width:\n";
	std::cin >> w;
	while (w > maxLength || w < 0) {
		std::cout << "Please enter a width less than " << maxLength << ":";
		std::cin.clear();
		std::cin >> w;
	}
	std::cout << "Enter your desired height:\n";
	std::cin >> h;
	while (h > maxLength || h < 0) {
		std::cout << "Please enter a height less than " << maxLength << ":";
		std::cin.clear();
		std::cin >> h;
	}

	// Maze is represented as a 2D grid, with an 8-bit integer representing
	// the configuration of the cell. 
	data = std::vector<std::vector<int8_t>>(w, std::vector<int8_t>(h, 0));
	// Vector to store list of cells visited in a connected path.
	std::vector<Coords> cellBackTrack;
	srand(time(NULL));
	// Randomly choosing start and end positions
	endCoords = Coords(rand() % w + 1, 0);
	xStart = rand() % w + 1;
	Coords cell = Coords(xStart, h - 1);
	cellBackTrack.push_back(cell);

	// Main Logic Loop
	while (true) {
		// Navigate backwards in path if 
		if (getAvailable(cell) == 0) {
			while (true) {
				cellBackTrack.pop_back();

				// checking for end condition
				if (cellBackTrack.size() == 0) goto Finish;

				// check if previous cell has open paths available
				if (getAvailable(cellBackTrack.back()) != 0) {
					cell = cellBackTrack.back();
					break;
				}
			}
		}

		//goes to new cell and removes wall
		Coords result = Coords();
		Coords nextCoords = Coords();
		while (true) {
			// Choose random direction
			int8_t dir = rand() & 0b11; //bit operation hack for fast mod 4
			switch (dir) {
			case 0:
				result = Coords(-1, 0);
				break;
			case 1:
				result = Coords(0, 1);
				break;
			case 2:
				result = Coords(1, 0);
				break;
			case 3:
				result = Coords(0, -1);
				break;
			}
			// Checks if chosen direction is valid
			int64_t x = cell.x + result.x;
			int64_t y = cell.y + result.y;
			if (x < w && x > -1 && y < h && y > -1 && data[x][y] == 0) {
				if (x == endCoords.x && y == endCoords.y && getAvailable(endCoords)) continue;
				nextCoords = Coords(x, y);
				break;
			}
		}

		update(cell, result);
		update(nextCoords, Coords(-result.x, -result.y));

		if (nextCoords.x != endCoords.x || nextCoords.y != endCoords.y) {
			cellBackTrack.push_back(nextCoords);
			cell.x = nextCoords.x;
			cell.y = nextCoords.y;
		}
	}
Finish:

	std::cout << "creating image...\n";

	// Use png++ to generate maze image
	png::image<png::rgb_pixel> maze(w << 2, h << 2);

	int_fast64_t X = 0;
	int_fast64_t Y = 0;
	png::rgb_pixel black(0, 0, 0);
	png::rgb_pixel white(255, 255, 255);

	for (int_fast64_t x = 0; x < w; x++)
	{
		Y = 0;
		for (int_fast64_t y = 0; y < h; y++)
		{
			for (int_fast8_t a = 0; a < 4; a++) for (int_fast8_t b = 0; b < 4; b++) maze[Y + b][X + a] = patterns[data[x][y]][b][a] == 'X' ? black : white;
			Y += 4;
		}
		X += 4;
	}

	png::rgb_pixel green(0, 255, 0);
	png::rgb_pixel red(255, 0, 0);
	for (int_fast8_t a = 0; a < 4; a++) for (int_fast8_t b = 0; b < 4; b++) maze[(h - 1) * 4 + b][xStart * 4 + a] = red;


	for (int_fast8_t a = 0; a < 4; a++) for (int_fast8_t b = 0; b < 4; b++) maze[b][endCoords.x * 4 + a] = green;	
	data.clear();
	maze.write("maze.png");
	std::cout << "Done!\n";
	return 0;
}

