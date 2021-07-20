#pragma once
using namespace std;
class Menu
{
public:
	Menu();
	~Menu();

	HANDLE hConsole, iConsole;
	short width;
	short height;

	void init();
	void show();
	void move(bool up);
	void change(bool left);
	void showGameDesc();
	void showThanks();
	void showTrackList();
	void showUserDesc();
	void select(); //не используется

	struct Menu_ipos{
		BYTE startGame = 1,
		level = 2,
		dif = 3,
		gameDesc = 4,
		thanks = 5,
		userDesc = 6,
		trackList = 7,
		exit = 8;
	} menu_pos;

	BYTE pos;

	enum Menu_window{
		w_main = 1,
		w_gameDesc = 2,
		w_thanks = 3,
		w_trackList = 4,
		w_userDesc = 5
	} window;

	void showdifficult();
	void showlevel();
	static const BYTE maxdif = 4, totalLev = 17; //Всего уровней.
	bool gamePassed;

	BYTE difficult, level, mindif, maxlev;
	bool blindLevels[totalLev+1];
	
private:
	
	DWORD nch; 
	WCHAR wch;
	CHAR_INFO selector;
	TCHAR lab_set[3];
	
	BYTE margin;	
};

