#pragma once
#include <ctime>

struct Labirinth;

//const clock_t CLOCKS_PER_MSEC = CLOCKS_PER_SEC / 1000;

struct VisibleView{
	CHAR_INFO *cells;

	//√раницы видимости в координатах карты.
	short lb, tb, rb, bb;
	short fullW, fullH;
	short viewW, viewH;
	int size;

	CHAR_INFO& coord(short x, short y){
		return cells[y*viewW + x];
	}

	void clear(){
		for(int i = 0; i < size; i++){
			cells[i].Attributes = 0;
			cells[i].Char.UnicodeChar = L' ';
		}
	}

	_SMALL_RECT getRect() const { return{ lb, tb, rb, bb }; }
};

class Hero
{
public:
	Hero();
	~Hero();
	
	void init(BYTE dif);

	void move_start(Labirinth *lab);
	void move_up(Labirinth *lab);
	void move_down(Labirinth *lab);
	void move_left(Labirinth *lab);
	void move_right(Labirinth *lab);

	bool hunger(); //true, если было изменение сытости.
	void die(Labirinth *lab);

	HANDLE hConsole;

	COORD pos;
	CHAR_INFO *hideCells;
	short viewRadius;
	short donut;
	short satiety; //—ытость
	bool died;
	short h_event;

private:
	VisibleView visibleView;
	double secPerFood;

	DWORD nch; WCHAR wch;
	clock_t startHung;

	void setVisible(const Labirinth*);
};

