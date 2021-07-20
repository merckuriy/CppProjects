#pragma once
struct Labirinth{
	CHAR_INFO *cells;

	short width, height;
	int size;
	COORD exit;

	CHAR_INFO& coord(short x, short y) const {
		return cells[y*width + x];
	}
};

