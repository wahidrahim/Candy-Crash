#include "square.h"
#include <iostream>
#include <sstream>
using namespace std;

Square::Square() {

	row = 0;
	col = 0;
	colour = Empty;
	type = Basic;

	gridSize = 0;

	ready = false;
	notified = false;

	neighbour[Up] = NULL;
	neighbour[Down] = NULL;
	neighbour[Left] = NULL;
	neighbour[Right] = NULL;

	grid = NULL;
	view = NULL;
}

void Square::init(int r, int c, int n, Square **g, View *v) {

	row = r;
	col = c;
	gridSize = n;
	grid = g;
	view = v;
}

void Square::setNeighbours() {

	if (row - 1 >=0) {

		neighbour[Up] = &grid[row - 1][col];
	}

	if (row + 1 < gridSize) {

		neighbour[Down] = &grid[row + 1][col];
	}

	if (col - 1 >= 0) {

		neighbour[Left] = &grid[row][col - 1];
	}

	if (col + 1 < gridSize) {

		neighbour[Right] = &grid[row][col + 1];
	}
}

void Square::swap(Direction d) {

	Colour tColour = colour;
	Type tType = type;

	colour = neighbour[d]->colour;
	type = neighbour[d]->type;

	neighbour[d]->colour = tColour;
	neighbour[d]->type = tType;

	neighbour[d]->notify();
	notify();
}

void Square::swapWith(Direction d) {

	Colour tColour = colour;
	Type tType = type;

	setColour(neighbour[d]->getColour());
	setType(neighbour[d]->getType());

	neighbour[d]->setColour(tColour);
	neighbour[d]->setType(tType);

	neighbour[d]->notify();
	notify();
}

void Square::notify() {

	if (colour == Empty) return;

	notified = true;

	if (neighbour[Left] && neighbour[Right]) {
		if (neighbour[Left]->colour == colour &&
				neighbour[Right]->colour == colour) {

			this->ready = true;
			neighbour[Left]->ready = true;
			neighbour[Right]->ready = true;
		}
	}

	if (neighbour[Up] && neighbour[Down]) {
		if (neighbour[Up]->colour == colour &&
				neighbour[Down]->colour == colour) {

			ready = true;
			neighbour[Up]->ready = true;
			neighbour[Down]->ready = true;
		}
	}

	notifyNeighbours();
}

void Square::notifyNeighbours() {

	for (int d = 0; d < 4; d++) {
		if (neighbour[d] &&
			neighbour[d]->colour == this->colour &&
			neighbour[d]->colour != Empty &&
			neighbour[d]->notified == false) {

			neighbour[d]->notify();
		}
	}
}

void Square::clearReady() {

	ready = false;

	for (int d = 0; d < NEIGHBOURS; d++) {

		if (neighbour[d] && neighbour[d]->isReady()) {
			neighbour[d]->clearReady();
		}
	}
}

void Square::clearNotified() {

	notified = false;

	for (int d = 0; d < NEIGHBOURS; d++) {

		if (neighbour[d] && neighbour[d]->isNotified()) {
			neighbour[d]->clearNotified();
		}
	}
}

void Square::clear(int &cleared, int &turnScore, int r = 4) {

	if (colour == Empty)  return;

	Colour tColour = colour;
	Type tType = type;

	setColour(Empty);
	setType(Basic);
	setReady(false);

	cleared++;

	switch (cleared) {
		case 0: case 1: case 2: break;
		case 3: turnScore = 3; break;
		case 4: turnScore = 8; break;
		case 5: turnScore = 15; break;
		default: turnScore = 4 * cleared;
	}

/*
 *    ostringstream ss;
 *    ss << "cleared: " << cleared << endl;
 *    ss << "score  : " << turnScore << endl;
 *
 *    switch (tType) {
 *        case Lateral: ss << "lateral square" << endl; break;
 *        case Upright: ss << "upright square" << endl; break;
 *        case Unstable: ss << "unstable square" << endl; break;
 *        case Psychedelic: ss << "psychedelic square" << endl; break;
 *        case Basic: ss << "basic square" << endl; break;
 *    }
 *
 *    view->print(ss.str());
 */

	switch (tType) {
		case Basic: break;
		case Lateral: 
		{
			for (int c = 0; c < gridSize; c++) {
				grid[row][c].clear(cleared, turnScore);
			}					  
		} break;			  
		case Upright:
		{
			for (int r = 0; r < gridSize; r++) {
				grid[r][col].clear(cleared, turnScore);
			}
		} break;
		case Unstable:
		{
			int sz = gridSize; // looks pretty

			int rMin = (row - r >= 0)? row - r : 0;
			int rMax = (row + r < sz)? row + r : sz - 1;
			int cMin = (col - r >= 0)? col - r : 0;
			int cMax = (col + r < sz)? col + r : sz - 1;

			for (int r = rMin; r <= rMax; r++) {
				for (int c = cMin; c <= cMax; c++) {
					grid[r][c].clear(cleared, turnScore);
				}
			}	   
		} break;
		case Psychedelic:
		{
			for (int i = 0; i < gridSize; i++) {
				for (int j = 0; j < gridSize; j++) {
					if (grid[i][j].getColour() == tColour) {
						grid[i][j].clear(cleared, turnScore);
					}
				}
			}
		} break;
	}
}

void Square::drop() {

	if (neighbour[Down]) {

		if (neighbour[Down]->getColour() == Empty) {

			neighbour[Down]->setColour(colour);
			neighbour[Down]->setType(type);

			setColour(Empty);
			setType(Basic);

			if (neighbour[Up]) neighbour[Up]->drop();

		} else {
			neighbour[Down]->drop();
		}
	}
}

int Square::getRow() { return row; }
void Square::setRow(int r) { row = r; }

int Square::getCol() { return col; }
void Square::setCol(int c) { col = c; }

Colour Square::getColour() { return colour; }
void Square::setColour(Colour c) {
	colour = c;
	view->setColour(row, col, colour);
}

Type Square::getType() { return type;  }
void Square::setType(Type t) {
	type = t;
	view->setType(row, col, type);
}

bool Square::isReady() { return ready; }
void Square::setReady(bool t) { ready = t; }

bool Square::isNotified() { return notified; }

void Square::printInfo() {
	cerr << "---- Square (" << this->row << "," << this->col << ") ----" << endl;
	cerr << "colour  : " << colour << endl;
	cerr << "type    : " << type << endl;
	cerr << "ready   : " << ready << endl;
	cerr << "notified: " << notified << endl;
	cerr << "----------------------" << endl;
}
