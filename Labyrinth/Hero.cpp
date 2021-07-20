#include "stdafx.h"
#include "Hero.h"
#include "Labirinth.h"
#include <vector>

using namespace std;

enum ConsoleColor
{
	Black = 0,
	Blue = 1,	// BLUE
	Green = 2,	// GREEN
	Cyan = 3,
	Red = 4,	// RED
	Magenta = 5,
	Brown = 6,
	LightGray = 7,
	DarkGray = 8, bDarkGray = 8 << 4, //INTENSITY
	LightBlue = 9,
	LightGreen = 10, //GREEN | INTENSITY
	LightCyan = 11,
	LightRed = 12,
	LightMagenta = 13, //INTENSITY | RED | BLUE
	Yellow = 14,
	White = 15
};



Hero::Hero(){
	
}

Hero::~Hero(){}

void Hero::init(BYTE dif)
{
	hConsole = GetStdHandle(STD_OUTPUT_HANDLE);

	//для level 2 satiety = 14
	switch(dif){
	case 0:
		satiety = 99;
		secPerFood = 0.0;
		viewRadius = 10;
		donut = 0;
		break;
	case 1:
		satiety += satiety/3; //+33%
		secPerFood = 6.0; 
		viewRadius = 6;
		donut = 6;
		break;
	case 2: //Нормальный уровень
		secPerFood = 5.0; 
		viewRadius = 4;
		donut = 5;
		break;
	case 3:
		satiety = (satiety - ((satiety * 1) / 5)>1) ? satiety - ((satiety * 1) / 5) : 1; // -20% от начального уровня
		secPerFood = 4.5; 
		viewRadius = 3;
		donut = 4;
		break;
	case 4:
		satiety = (satiety - ((satiety * 3) / 10) >1) ? satiety - ((satiety * 3) / 10) : 1; // -30% от начального уровня 
		secPerFood = 4.5; 
		viewRadius = 1; 
		donut = 5;
		break;
	default:
		return;
	}

	died = false;

	visibleView.fullW = 2 * viewRadius + 1; 
	visibleView.fullH = 2 * viewRadius + 1;
	visibleView.size = visibleView.fullW * visibleView.fullH;

	/*string sview = 
		"1111111"
		"1111111"
		"111H111"
		"1111111"
		"1111111";*/

	visibleView.cells = new CHAR_INFO[visibleView.size];

	//Установим участок, скрывающий пройденные клетки.
	hideCells = new CHAR_INFO[visibleView.fullW];
	for(short i = 0; i < visibleView.fullW; i++){ hideCells[i].Char.UnicodeChar = L' '; hideCells[i].Attributes = 0; }

	startHung = clock();
}


void Hero::setVisible(const Labirinth *lab)
{
	//Вычисляем границы области видимости
	visibleView.lb = (pos.X > viewRadius) ? pos.X - viewRadius : 0;
	visibleView.tb = (pos.Y > viewRadius) ? pos.Y - viewRadius : 0;
	visibleView.rb = (pos.X + viewRadius > lab->width - 1) ? lab->width - 1 : pos.X + viewRadius;
	visibleView.bb = (pos.Y + viewRadius > lab->height - 1) ? lab->height - 1 : pos.Y + viewRadius;
	visibleView.viewW = visibleView.rb - visibleView.lb + 1;
	visibleView.viewH = visibleView.bb - visibleView.tb + 1;

	visibleView.coord(pos.X - visibleView.lb, pos.Y - visibleView.tb).Attributes = Yellow;
	visibleView.coord(pos.X - visibleView.lb, pos.Y - visibleView.tb).Char.UnicodeChar = L'☻';
	
	//проверяем только по рамке //i,y - строка, j,x - столбец,
	for(short li = visibleView.tb; li <= visibleView.bb; li++)
	for(short lj = visibleView.lb; lj <= visibleView.rb; lj++)
	if(li == visibleView.tb || li == visibleView.bb || lj == visibleView.lb || lj == visibleView.rb){
		
		short X = pos.X, Y = pos.Y, E, newE, E0; //x0, y0. x1 = lj, y1 = li;
		short dx = abs(lj - X), dy = abs(li - Y);

		int sx = X < lj ? 1 : -1;
		int sy = Y < li ? 1 : -1;
		E0 = newE = (dx > dy ? dx : -dy) / 2;
		bool enclose = false;
		bool cross = false;
		short angle = (dx == 0 || dy == 0)?0:(dx > dy ? (dx*10 )/ dy : (dy*10 )/ dx);

		while(!(X == lj && Y == li)){ //x0 == x1, y0 == y1
			
			E = newE;
			if(E > -dx){ newE -= dy; X += sx; }
			if(E < dy){ newE += dx; Y += sy; }


			//Основано на том, что x и y меняются одновременно.
			//Если было пересечение блока, а потом изменение x и y.
			if((E > -dx && E < dy) && cross){ break; } //E > -dx && E < dy

			//Если идём по диагонали или это последний блок, смотрим 2 ближних блока. 
			//Если есть, этот блок невидим и следующие тоже.
			if((cross || (E > -dx && E < dy)) //newE == E0
				&& lab->coord(X - sx, Y).Char.UnicodeChar == L'█'
				&& lab->coord(X, Y - sy).Char.UnicodeChar == L'█'){
				break;
			}

			visibleView.coord(X - visibleView.lb, Y - visibleView.tb).Attributes = lab->coord(X, Y).Attributes;
			visibleView.coord(X - visibleView.lb, Y - visibleView.tb).Char.UnicodeChar = lab->coord(X, Y).Char.UnicodeChar;

			//Проверяем наличие соседнего блока, который может перекрыть обзор.
			if(abs(X - pos.X) > 0 && abs(Y - pos.Y) > 0){
				if(abs(newE) > abs(E0)){
					if((E0 > 0 && lab->coord(X, Y - sy).Char.UnicodeChar == L'█')
						|| (E0 < 0 && lab->coord(X - sx, Y).Char.UnicodeChar == L'█')){
						enclose = true;
					}
				}

				if((E > -dx && E < dy) && (abs(X - pos.X) + abs(Y - pos.Y)) > 2){ //&& angle < 16
					if(lab->coord(X - sx, Y).Char.UnicodeChar == L'█' || lab->coord(X, Y - sy).Char.UnicodeChar == L'█'){
						cross = true;
					}
				}
			}
	
			//Если блок близок к вектору и рядом есть перекрывающие блоки, то этот блок считается
			//перекрывающим обзор, другие блоки за ним не видны. Если рядом блоков нет, то этот блок обзор не перекрывает.
			if(lab->coord(X, Y).Char.UnicodeChar == L'█' || cross){
				if(abs(newE) > abs(E0)){//abs(newE) > abs(E0)
					if(E0 > 0 && (enclose || lab->coord(X, Y - sy).Char.UnicodeChar == L'█' || lab->coord(pos.X + sx, pos.Y).Char.UnicodeChar == L'█')){
						break;
					} else if(E0 < 0 && (enclose || lab->coord(X - sx, Y).Char.UnicodeChar == L'█' || lab->coord(pos.X, pos.Y + sy).Char.UnicodeChar == L'█')){
						break;
					}
				} else {
					break;
				}

				if(newE != E){ cross = true; } //Если есть блок и изменяется E, запоминаем пересечение блока.
			}
			
		}
	}
}


void Hero::move_start(Labirinth *lab)
{
	visibleView.clear(); //Очищаем область	
	setVisible(lab); //Заполняем область видимости клетками лабиринта

	_SMALL_RECT m_rect = visibleView.getRect(); //Получаем текущие границы видимости в игре
	m_rect.Top += 4; m_rect.Bottom += 4;
	WriteConsoleOutput(hConsole, visibleView.cells, { visibleView.viewW, visibleView.viewH }, { 0, 0 }, &m_rect);
}

void Hero::move_up(Labirinth *lab)
{
	wch = lab->coord(pos.X, pos.Y - 1).Char.UnicodeChar;
	if (pos.Y-1<0 || wch == L'█'){ return; }

	pos.Y--;
 
	visibleView.clear(); //Очищаем область	
	setVisible(lab); //Заполняем область видимости клетками лабиринта

	//Выводим видимые блоки в лабиринт
	_SMALL_RECT m_rect = visibleView.getRect(); m_rect.Top += 4; m_rect.Bottom += 4;
	WriteConsoleOutput(hConsole, visibleView.cells, { visibleView.viewW, visibleView.viewH }, { 0, 0 }, &m_rect);

	//Прячем то, что не видно. Если видимость в пределах лабиринта.

	if(visibleView.bb != lab->height - 1){
		m_rect.Bottom++;
		m_rect.Top = m_rect.Bottom;
		WriteConsoleOutput(hConsole, hideCells, { visibleView.viewW , 1 }, { 0, 0 }, &m_rect);
	}
}

void Hero::move_down(Labirinth *lab)
{
	wch = lab->coord(pos.X, pos.Y + 1).Char.UnicodeChar;
	if (pos.Y+1==lab->height || wch == L'█'){ return; }

	pos.Y++;	
	visibleView.clear();
	setVisible(lab);

	_SMALL_RECT m_rect = visibleView.getRect(); m_rect.Top += 4; m_rect.Bottom += 4;
	WriteConsoleOutput(hConsole, visibleView.cells, { visibleView.viewW, visibleView.viewH }, { 0, 0 }, &m_rect);
	
	if(visibleView.tb != 0){
		m_rect.Top--;
		m_rect.Bottom = m_rect.Top;
		WriteConsoleOutput(hConsole, hideCells, { visibleView.viewW, 1 }, { 0, 0 }, &m_rect);
	}
}

void Hero::move_right(Labirinth *lab)
{
	wch = lab->coord(pos.X+1, pos.Y).Char.UnicodeChar;
	if (pos.X+1==lab->width || wch == L'█'){ return; }

	pos.X++;
	visibleView.clear();
	setVisible(lab);

	_SMALL_RECT m_rect = visibleView.getRect(); m_rect.Top += 4; m_rect.Bottom += 4;
	WriteConsoleOutput(hConsole, visibleView.cells, { visibleView.viewW, visibleView.viewH }, { 0, 0 }, &m_rect);
	
	if(visibleView.lb != 0){
		m_rect.Left--;
		m_rect.Right = m_rect.Left;
		WriteConsoleOutput(hConsole, hideCells, { 1, visibleView.viewH }, { 0, 0 }, &m_rect);
	}
}

void Hero::move_left(Labirinth *lab)
{
	wch = lab->coord(pos.X-1, pos.Y).Char.UnicodeChar;
	if (pos.X<0 || wch == L'█'){ return; }

	pos.X--;
	visibleView.clear();
	setVisible(lab);

	_SMALL_RECT m_rect = visibleView.getRect(); m_rect.Top += 4; m_rect.Bottom += 4;
	WriteConsoleOutput(hConsole, visibleView.cells, { visibleView.viewW, visibleView.viewH }, { 0, 0 }, &m_rect);
	
	if(visibleView.rb != lab->width-1){
		m_rect.Right++;
		m_rect.Left = m_rect.Right;
		WriteConsoleOutput(hConsole, hideCells, { 1, visibleView.viewH }, { 0, 0 }, &m_rect);
	}
}

bool Hero::hunger(){
	if(secPerFood == 0.0) return false;

	if(((clock() - startHung) / (double)CLOCKS_PER_SEC) > secPerFood){
		startHung = clock();
		satiety--; 

		if(satiety <= 0){
			died = true;
		} 
		return true;
	}
	return false;
}

void Hero::die(Labirinth *lab){
	visibleView.clear();

	visibleView.lb = (pos.X > viewRadius) ? pos.X - viewRadius : 0;
	visibleView.tb = (pos.Y > viewRadius) ? pos.Y - viewRadius : 0;
	visibleView.rb = (pos.X + viewRadius > lab->width - 1) ? lab->width - 1 : pos.X + viewRadius;
	visibleView.bb = (pos.Y + viewRadius > lab->height - 1) ? lab->height - 1 : pos.Y + viewRadius;

	visibleView.coord(pos.X - visibleView.lb, pos.Y - visibleView.tb).Attributes = Green;
	visibleView.coord(pos.X - visibleView.lb, pos.Y - visibleView.tb).Char.UnicodeChar = L'☻';

	_SMALL_RECT m_rect = visibleView.getRect(); m_rect.Top += 4; m_rect.Bottom += 4;
	WriteConsoleOutput(hConsole, visibleView.cells, { visibleView.viewW, visibleView.viewH }, { 0, 0 }, &m_rect);
}