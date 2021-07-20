#include "stdafx.h"
#include "Menu.h"

Menu::Menu(){}
Menu::~Menu(){}

using namespace std;

enum ConsoleColor
{
	Black = 0,
	Blue = 1,	// BLUE
	Green = 2,	// GREEN
	Cyan = 3,
	Red = 4,	// RED
	Magenta = 5,
	Orange = 6,
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

void Menu::init(){
	hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	if(gamePassed){
		width = 40;
		height = 27;
	} else{
		width = 40;
		height = 25;
	}
		
	margin = 6;

	selector.Attributes = Yellow;
	selector.Char.UnicodeChar = L'☻';
};

void Menu::show()
{
	system("cls");
	
	window = w_main;

	SMALL_RECT m_rect = { 0, 0, 0, 0 }; //Console window 0,0 size by default.
	SetConsoleWindowInfo(hConsole, true, &m_rect);

	if(!SetConsoleScreenBufferSize(hConsole, { width, height })){
		cerr << "Ошибка установки размера буфера";
	}
	m_rect = { 0, 0, width - 1, height - 1 };
	if(!SetConsoleWindowInfo(hConsole, true, &m_rect)){
		cerr << "Ошибка установки размера окна";
	}

	WCHAR *menu = L"Лабиринт";
	WriteConsoleOutputCharacter(hConsole, menu, wcslen(menu), { width / 2 - (short)wcslen(menu) / 2 - 1, 1 }, &nch);

	if(!gamePassed){ menu_pos.trackList = 0; menu_pos.exit = 7; }
	else{ menu_pos.exit = 8; }
	if(!pos) pos = menu_pos.startGame;//menu_startGame; 

	short topmarg = 2;
	
	menu = L"Начать игру";
	WriteConsoleOutputCharacter(hConsole, menu, wcslen(menu), { margin, topmarg+=2 }, &nch);

	m_rect = { margin - 2, 2 + pos * 2, margin - 2, 2 + pos * 2 };
	WriteConsoleOutput(hConsole, &selector, { 1, 1 }, { 0, 0 }, &m_rect);

	showlevel();
	showdifficult();

	menu = L"Описание игры";
	WriteConsoleOutputCharacter(hConsole, menu, wcslen(menu), { margin, topmarg+=6 }, &nch);

	menu = L"Благодарности";
	WriteConsoleOutputCharacter(hConsole, menu, wcslen(menu), { margin, topmarg += 2 }, &nch);

	menu = L"Пользовательский уровень";
	WriteConsoleOutputCharacter(hConsole, menu, wcslen(menu), { margin, topmarg += 2 }, &nch);

	if(gamePassed){
		menu = L"Список композиций";
		WriteConsoleOutputCharacter(hConsole, menu, wcslen(menu), { margin, topmarg += 2 }, &nch);
	}

	menu = L"Выход";
	WriteConsoleOutputCharacter(hConsole, menu, wcslen(menu), { margin, topmarg+=2 }, &nch);

	menu = L"Управление: ↑↓→←";
	WriteConsoleOutputCharacter(hConsole, menu, wcslen(menu), { margin, topmarg += 4 }, &nch);
	menu = L"Выбор: Enter; Выход: Esc";
	WriteConsoleOutputCharacter(hConsole, menu, wcslen(menu), { margin, topmarg += 1 }, &nch);

	menu = L"by Vit`Cont";
	WriteConsoleOutputCharacter(hConsole, menu, wcslen(menu), { width - (short)wcslen(menu) - 2, height - 2 }, &nch);
}

void Menu::move(bool up)
{
	if(window != w_main) return;

	FillConsoleOutputCharacter(hConsole, L' ', 1, { margin - 2, 2 + pos*2 }, &nch);

	if(up){ pos = (pos - 1 < 1) ? menu_pos.exit : pos - 1; }
	else{ pos = (pos + 1 > menu_pos.exit) ? 1 : pos + 1; }

	SMALL_RECT m_rect = { margin - 2, 2 + pos * 2, margin - 2, 2 + pos * 2 };
	WriteConsoleOutput(hConsole, &selector, { 1, 1 }, { 0, 0 }, &m_rect);
}


void Menu::change(bool left)
{
	if(pos == menu_pos.level && (left ? level > 0:level < maxlev)){
		level = (left ? level-1 : level+1 );
		showlevel();
		_itow_s(level, lab_set, 10);
		WritePrivateProfileStringW(L"GameSet", L"level", lab_set, L".\\Labirinth.ini");
	} else if(pos == menu_pos.dif && (left ? difficult > mindif : difficult < maxdif)){
		difficult = (left ? difficult - 1 : difficult + 1);;
		showdifficult();
		_itow_s(difficult, lab_set, 10);
		WritePrivateProfileStringW(L"GameSet", L"difficult", lab_set, L".\\Labirinth.ini");
	}
}


void Menu::showlevel(){
	wstring menu;


	if(level == 0){
		if(level == maxlev){
			menu = L"Уровень             0   ";
			mindif = 1;
		} else{
			menu = menu + L"Уровень             0" + L" ► " + (blindLevels[0] == 1 ? L"☺" : L" ");
			mindif = 0; if(difficult != mindif){ showdifficult(); }
		}
	} else if(level == maxlev){
		menu = L"Уровень           ◄ " + to_wstring(level) + L"   " + (blindLevels[level-1] == 1 ? L"☺" : L" ");
		if(level != totalLev){
			mindif = 1; 
			if(difficult < mindif) difficult = 1;
			if(difficult == 1) showdifficult();
		}
	} else{
		menu = L"Уровень           ◄ " + to_wstring(level) + L" ► " + (blindLevels[level - 1] == 1 ? L"☺" : L" ");
		mindif = 0; if(difficult != mindif){ showdifficult(); }
	}

	WriteConsoleOutputCharacter(hConsole, menu.c_str(), menu.length(), { margin, 2+menu_pos.level*2 }, &nch); //wcslen(menu)
}

void Menu::showdifficult(){
	wstring menu, wdif;
	
	switch(difficult){
	case 0: wdif = L"неголодающий"; break;
	case 1: wdif = L"  ребёнок   "; break;
	case 2: wdif = L" нормальный "; break;
	case 3: wdif = L"  сложный   "; break;
	case 4: wdif = L"   слепой   "; break;
	}

	if(difficult == mindif){
		menu = L"Уровень сложности  " + wdif + L"►";
	} else if(difficult == maxdif){
		menu = L"Уровень сложности ◄" + wdif + L" ";
	} else{
		menu = L"Уровень сложности ◄" + wdif + L"►";
	}

	WriteConsoleOutputCharacter(hConsole, menu.c_str(), menu.length(), { margin, 2 + menu_pos.dif * 2 }, &nch);
}

void Menu::showGameDesc(){
	system("cls");
	window = w_gameDesc;
	BYTE descmargin = 2, topmargin = 1;

	cout << "\n    Используйте клавиши стрелок:\x18\x19\x1A\x1B \n  для передвижения (можно удерживать).\n    Esc - выход.\n\n"; //↑↓→←

	cout << "    Ваш герой голодает, ешьте пончики:"; SetConsoleTextAttribute(hConsole, Orange); cout << "o";
	SetConsoleTextAttribute(hConsole, LightGray); cout << "\n  и найдите выход:";
	SetConsoleTextAttribute(hConsole, LightGreen);
	WriteConsole(hConsole, L"▒", 1, &nch, NULL); //177 символ почему-то не поддерживается cout-ом.
	SetConsoleTextAttribute(hConsole, LightGray); cout << "!\n\n";

	cout << "    В игре присутствует музыка, \n  включите звук. \n\n     Удачи!";
	SetConsoleCursorPosition(hConsole, { 2, height - 2 });
	cout << "Esc - назад";
}

void Menu::showThanks(){
	system("cls");
	window = w_thanks;

	cout << "\n    Мне пока не приходилось задавать \n вопросы (спасибо google). "
		    "\n    Эти люди помогли мне, отвечая на \n вопросы других и делая статьи. ";

	WCHAR * text = L"Благодарности:";
	WriteConsoleOutputCharacter(hConsole, text, wcslen(text), { width/2 - (short)wcslen(text)/2, 7 }, &nch);
	cout << "\n\n\n\n\n  Martee, Goz, Croessmah, Roger,\n\n  Kastaneda, dubistyroo, Igor81,\n\n"
		"Dolph, Mr.C64, Bertrand Marron,\n\n  Kerrek SB, castaway.";

	SetConsoleCursorPosition(hConsole, { 2, height - 2 });
	cout << "Esc - назад";
}

void Menu::showTrackList(){
	system("cls");
	window = w_trackList;

	cout << endl <<
		"ур.1-5:  \n      Mysthive - Good Little Hero\n\n"
		"ур.6,7:  \n   Mysthive - Biggest Little Adventure\n\n"
		"ур.8-10:\n    Asaf Avidan - One Day (Karaoke)\n\n"
		"ур.11,12:\n Parov Stelar - Chambermaid Swing\n" 
				 "         (ChaseR remix)\n\n"
		"ур.13,14:\n             Rammstein - Los\n\n"
		"ур.15,16:\n Metric - Help I'm Alive (Alcala Remix)\n\n"
		"ур.17:   \n          Metric - Collect Call\n"
		         "          (Adventure Club Remix)\n\n";

	SetConsoleCursorPosition(hConsole, { 2, height - 2 });
	cout << "Esc - назад";
}

void Menu::showUserDesc(){
	system("cls");
	window = w_trackList;

	cout << endl <<
		" Вы можете создать собственный уровень!\n\n"
		" Для этого важно правильно описать его \n"
		" структуру.\n"
		" Назовите свой уровень user_level.\n" 
		" Расширение должно быть .txt (можете \n"
		" создать его в блокноте)\n"
		" Теперь введите 3 числа через пробел,\n"
		" где 1-ое ширина вашего лабиринта в\n"
		" клетках, 2-ое высота, 3-е начальное\n"
		" количество еды для нормального уровня.\n"
		" Далее рисуйте карту, где 1 - это стена,"
		" пробел: пустота, F: пончик, E: выход, \n"
		" H: начальная позиция героя.\n"
		" Не забудьте сохранить изменения!\n"
		" В меню выберите уровень 0.\n\n"
		" В этом уровне может играть Ваша песня!\n"
		" Назовите её user_song.mp3 и положите\n"
		" в папку sound.";

	SetConsoleCursorPosition(hConsole, { 1, height - 2 });
	cout << "Esc - назад";
}

void Menu::select(){}


