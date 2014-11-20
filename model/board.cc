#include <fstream>
#include <sstream>
#include "board.h"
#include "../view/textview/textview.h"
#include "../public/global.h"

using namespace std;

Board::Board(int n) : size(n) {
	// initializing grid
	grid = new Square *[size];

	for (int i = 0; i < size; i++) {
		grid[i] = new Square[size];
	}

	level = 0;
	score = 0;

	view = new View(size);
	loadLevel(level);
	view->draw();

#ifdef DEBUG
	for (int i = 0; i < size; i++) {
		for (int j = 0; j < size; j++) {
			cerr << grid[i][j].colour << " ";
		}
		cerr << endl;
	}
#endif
}

Board::~Board() {
	for (int i = 0; i < size; i++) {
		delete[] grid[i];
	}
	delete[] grid;

	delete view;
}

void Board::loadLevel(int level) {

	if (level == 0) {

		ifstream file("sequence.txt");

		string square;

		for (int i = 0; i < size; i++) {
			for (int j = 0; j < size; j++) {

				file >> square;

				// not used at the moment
				//int advanced;
				//advanced = (square[0] == '_') ? 0 : (square[0] - '0');

				Type type;

				switch (square[1]) {
					case '_': type = Basic; break;
					case 'h': type = Lateral; break;
					case 'v': type = Upright; break;
					case 'b': type = Unstable; break;
					case 'p': type = Psychedelic; break;
				}

				Colour colour;
				colour = (Colour)(square[2] - '0');

				grid[i][j].row = i;
				grid[i][j].col = j;
				grid[i][j].colour = colour;
				grid[i][j].type = type;

				view->setColour(i, j, colour);
				view->setType(i, j, type);
				view->setScore(score);
				view->setLevel(level);
			}
		}
	}
}

void swapWith(Square &a, Square &b) {
#ifdef DEBUG
	cerr << "square a: " << endl;
	cerr << "row: " << a.row << " col: "<< a.col << endl;
	cerr << "colour: " << a.colour << " type: " << a.type << endl;

	cerr << endl << "square b: " << endl;
	cerr << "row: " << b.row << " col: "<< b.col << endl;
	cerr << "colour: " << b.colour << " type: " << b.type << endl;
#endif

	int tRow = a.row; 
	int tCol = a.col; 
	Colour tColour = a.colour; 
	Type tType = a.type;

	a.row = b.row;
	a.col = b.col;
	a.colour = b.colour;
	a.type = b.type;
	
	b.row = tRow;
	b.col = tCol;
	b.colour = tColour;
	b.type = tType;

#ifdef DEBUG
	cerr << endl;

	cerr << "square a: " << endl;
	cerr << "row: " << a.row << " col: "<< a.col << endl;
	cerr << "colour: " << a.colour << " type: " << a.type << endl;

	cerr << endl << "square b: " << endl;
	cerr << "row: " << b.row << " col: "<< b.col << endl;
	cerr << "colour: " << b.colour << " type: " << b.type << endl;
#endif
}

void Board::swap(int row, int col, Direction d) {
    
    if (row == 0 && d == Up) {return;}
    if (col == 0 && d == Left) {return;}
    if (row == this->size - 1 && d == Down) {return;}
    if (col == this->size - 1 && d == Right) {return;}
    
	switch (d) {
		case Up: swapWith(grid[row][col], grid[row - 1][col]); break;
		case Down: swapWith(grid[row][col], grid[row + 1][col]); break;
		case Left: swapWith(grid[row][col], grid[row][col - 1]); break;
		case Right: swapWith(grid[row][col], grid[row][col + 1]); break;
	}

	view->swap(row, col, d);
	view->draw();

#ifdef DEBUG
	for (int i = 0; i < size; i++) {
		for (int j = 0; j < size; j++) {
			cerr << grid[i][j].colour << " ";
		}
		cerr << endl;
	}
#endif

}

vector<Square> Board::findMatches(int row, int col) {
	vector<Square> matched;

	matched.push_back(grid[0][0]);

	return matched;
}

string Board:: validMove() {
    #if DEBUG_BOARD
        fprintf(stderr,"BOARD:: validMove()\n");
    #endif
	ostringstream ss;
    Board b(this->size);
    for (int i = 0; i < this->size; i++) {
        for (int j = 0; j < this->size; j++) {
            b.grid[i][j] = this->grid[i][j];
        }
    }
    for (int i = 0; i < this->size; i++) {
        for (int j = 0; j < this->size; j++) {
            b.swap(i,j,Up);
            if (! b.findMatches(i,j).empty()) {
            	ss << i << " " << j << " " << (int)Up;
            	return ss.str();
            };
            b.swap(i,j,Up);
            b.swap(i,j,Down);
            if (! b.findMatches(i,j).empty()) {
            	ss << i << " " << j << " " << (int)Down;
            	return ss.str();
            };
            b.swap(i,j,Down);
            b.swap(i,j,Left);
            if (! b.findMatches(i,j).empty()) {
            	ss << i << " " << j << " " << (int)Left;
            	return ss.str();
            };
            b.swap(i,j,Left);
            b.swap(i,j,Right);
            if (! b.findMatches(i,j).empty()) {
            	ss << i << " " << j << " " << (int)Right;
            	return ss.str();
            };
        }
    }
    return string();
}

bool Board:: hasMove() {
    return this->validMove().length();
}

void Board:: hint() {
    string str = this->validMove();
    #if DEBUG_BOARD
        fprintf(stderr,"BOARD hint(%s)\n",str.c_str());
    #endif
    this->view->print(str);
}
