#include "stdafx.h"

#pragma comment(lib, "Winmm.lib")


//#include <Mmsystem.h> //mci
//#include <Winbase.h> //.ini

#include <fstream>
#include <conio.h> 
#include "Labirinth.h"

#include "Hero.h"
#include "Menu.h"

using namespace std;


bool runGame();
bool runMenu();
void startGame();
void showEnd();

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

Hero m_hero;
Menu m_menu;
Labirinth m_lab;
HANDLE hConsole, iConsole;
WCHAR wchsatiety[] = L"   ";
wstring menu;
const short minWidth = 46, minHeight = 16;
const double promptTime = 2.0;
TCHAR lab_set[3];


void pause(){	
	//FlushConsoleInputBuffer(iConsole);
	cout << "Нажмите любую клавишу для продолжения..."; // << flush; - Зачем?
	system("pause > nul");
	//_getch(); // - временами не работает. м.б. нужен nodelay(stdscr, 0); из nurses.h
}



//BOOL WINAPI HandlerRoutine(DWORD dwCtrlType ){ //Console event.
//	MessageBox(nullptr, TEXT("some event"), TEXT("Message"), MB_OK); return true;
//}; SetConsoleCtrlHandler(HandlerRoutine, 1);

int _tmain(int argc, _TCHAR* argv[])
{
	

	setlocale(LC_ALL, "rus");

	SetConsoleTitle(L"Labirinth");
	hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	iConsole = GetStdHandle(STD_INPUT_HANDLE);
	//HWND hWindow = GetConsoleWindow();
	CONSOLE_CURSOR_INFO info;
	info.dwSize = 1;
	info.bVisible = false;
	SetConsoleCursorInfo(hConsole, &info); //Hide cursor

	CONSOLE_SCREEN_BUFFER_INFOEX scrInfo;
	scrInfo.cbSize = sizeof(scrInfo);
	GetConsoleScreenBufferInfoEx(hConsole, &scrInfo);
	scrInfo.ColorTable[6] = RGB(255, 130, 0);  // Replace brown to orange
	SetConsoleScreenBufferInfoEx(hConsole, &scrInfo);

	SetConsoleTextAttribute(hConsole, Black << 4 | LightGray);

	CONSOLE_FONT_INFOEX fontInfo;
	fontInfo.cbSize = sizeof fontInfo;
	GetCurrentConsoleFontEx(hConsole, false, &fontInfo);
	fontInfo.dwFontSize = { 12, 16 };
	fontInfo.FontFamily = FF_DONTCARE;
	wcscpy_s(fontInfo.FaceName, L"");
	if(!SetCurrentConsoleFontEx(hConsole, false, &fontInfo)){
		cerr << "Ошибка установки шрифта";
	}



	//ARRAYSIZE(mas) - macros from Windows.h instead of sizeof(mas)/sizeof(*mas)
	GetPrivateProfileString(L"GameSet", L"level", L"1", lab_set, ARRAYSIZE(lab_set), L".\\Labirinth.ini");
	m_menu.level = _wtoi(lab_set);
	GetPrivateProfileString(L"GameSet", L"maxlevel", L"1", lab_set, ARRAYSIZE(lab_set), L".\\Labirinth.ini");
	m_menu.maxlev = _wtoi(lab_set);
	GetPrivateProfileString(L"GameSet", L"difficult", L"1", lab_set, ARRAYSIZE(lab_set), L".\\Labirinth.ini");
	m_menu.difficult = _wtoi(lab_set);
	GetPrivateProfileString(L"GameSet", L"gamePassed", L"0", lab_set, ARRAYSIZE(lab_set), L".\\Labirinth.ini");
	m_menu.gamePassed = lab_set[0] == L'1' ? 1 : 0; //_wtoi(lab_set)

	//m_menu.blindLevels[m_menu.level - 1] = 1;
	WCHAR blindlev[m_menu.totalLev * 2];
	GetPrivateProfileString(L"GameSet", L"blindLevels", L"0", blindlev, m_menu.totalLev*2, L".\\Labirinth.ini");
	for(short i = 0, j = 0; i < m_menu.totalLev; i++, j+=2){
		m_menu.blindLevels[i] = blindlev[j]==L'1'?1:0;
	}
	//SetConsoleCursorPosition(hConsole, { 0, m_menu.height - 2 });

	m_menu.init();
	m_menu.show();
	while(runMenu());
	
	return 0;
}

bool runMenu(){
	bool Exit = false;

	INPUT_RECORD eventType;
	DWORD numReading, numWrChars;
	WCHAR wch;

	GetNumberOfConsoleInputEvents(iConsole, &numReading);

	if(numReading){
		//ReadConsoleInput останавливает цикл до получения события.
		ReadConsoleInput(iConsole, &eventType, 1, &numReading);
		if(eventType.EventType == KEY_EVENT && eventType.Event.KeyEvent.bKeyDown){
			switch(eventType.Event.KeyEvent.wVirtualKeyCode){
			case VK_ESCAPE: 
				if(m_menu.window != m_menu.w_main){
					system("cls");
					m_menu.window = m_menu.w_main;
					m_menu.show();
				} else{
					Exit = true;
				}
				break;
			case VK_UP:	m_menu.move(1); break;
			case VK_DOWN: m_menu.move(0); break;
			case VK_LEFT: m_menu.change(1); break;		
			case VK_RIGHT: m_menu.change(0); break;
			case VK_RETURN: 
				if(m_menu.pos == m_menu.menu_pos.startGame){
					startGame();
					m_menu.show();
				} else if(m_menu.pos == m_menu.menu_pos.gameDesc){
					m_menu.showGameDesc();
				} else if(m_menu.pos == m_menu.menu_pos.thanks){
					m_menu.showThanks();
				} else if(m_menu.pos == m_menu.menu_pos.userDesc){
					m_menu.showUserDesc();
				} else if(m_menu.pos == m_menu.menu_pos.trackList){
					m_menu.showTrackList();
				} else if(m_menu.pos == m_menu.menu_pos.exit){
					Exit = true;
				}
				break;
			default: break;
			}
			FlushConsoleInputBuffer(iConsole);
			Sleep(200);

		} else if(eventType.EventType == MOUSE_EVENT){
			//cout << eventType.Event.MouseEvent.dwMousePosition.X;
		}
	}


	if(Exit) return false;
	return true;
}

void startGame(){
	if(m_menu.level > m_menu.maxlev || m_menu.level < 0 || m_menu.maxlev < 0 || m_menu.maxlev > 17){
		cerr << "Ошибка настроек уровня.\n"; pause(); return;
	}
	string levelpath;
	if(m_menu.level == 0){
		levelpath = "levels/user_level.txt";
	} else{
		levelpath = "levels/Level_" + to_string(m_menu.level) + ".lab";
	}
	ifstream i_file(levelpath); //Level_1.txt
	if(!i_file.good()){ cerr << "Уровень не найден.\n"; pause(); return; }
	
	string lab, line;

	i_file >> m_lab.width >> m_lab.height >> m_hero.satiety;
	m_lab.size = m_lab.width*m_lab.height;
	i_file.ignore();
	short l = 0;
	while(getline(i_file, line)){
		lab += line; l++;
	}
	i_file.close();

	if(l != m_lab.height || line.length() != m_lab.width || m_hero.satiety > 10000){
		cerr << "Некорректное описание лабиринта.\n"; pause(); return;
	}

	COORD max_wsize;
	max_wsize = GetLargestConsoleWindowSize(hConsole);
	if(m_lab.width > max_wsize.X || m_lab.height > max_wsize.Y){
		cerr << "Размеры лабиринта превышают \nмаксимально возможный: " 
			 << max_wsize.X << "x" << max_wsize.Y << endl;
		pause(); return;
	}

	SMALL_RECT m_rect = { 0, 0, 0, 0 }; //Console window 0,0 size by default.
	SetConsoleWindowInfo(hConsole, true, &m_rect);

	CONSOLE_SCREEN_BUFFER_INFOEX scrInfo;
	GetConsoleScreenBufferInfoEx(hConsole, &scrInfo);

	//Если размеры лабиринта превышают минимальные, то размер окна ставится по ним. 
	//4 под меню + 1 под нижний отступ. (минимальное разрешение высоты экрана монитора - 1080, 
	// т.к. для последнего уровня требуется высота 50 строк, и это максимум для 1080.
	short width = (minWidth > m_lab.width) ? minWidth : m_lab.width;
	short height = (minHeight > m_lab.height + 6) ? minHeight : m_lab.height + 5;
	if(!SetConsoleScreenBufferSize(hConsole, { width, height + 1 })){
		cerr << "Ошибка установки размера буфера"; pause(); return;
	}
	m_rect = { 0, 0, width - 1, height - 1 }; //left, top, right, bottom //-1
	if(!SetConsoleWindowInfo(hConsole, true, &m_rect)){
		//Get the error message ID, if any.
		DWORD errorMessageID = ::GetLastError();
		LPSTR messageBuffer = nullptr;
		//Ask Win32 to give us the string version of that message ID.
		//The parameters we pass in, tell Win32 to create the buffer that holds the message for us (because we don't yet know how long the message string will be).
		size_t size = FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
			NULL, errorMessageID, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPSTR)&messageBuffer, 0, NULL);
		//Copy the error message into a std::string.
		std::string message(messageBuffer, size);
		//Free the Win32's string's buffer.
		LocalFree(messageBuffer);

		cerr << "Ошибка установки размера окна"; pause(); return;
	}
	//MoveWindow(hWindow, 20, 20, 30, 30, false);
	//MessageBox(nullptr, TEXT("The driver is sleeping!!"), TEXT("Message"), MB_OK);

	system("cls");

	
	m_lab.cells = new CHAR_INFO[m_lab.size];
	int i = 0;
	for(auto wch : lab){
		if(wch == '1'){
			m_lab.cells[i].Char.UnicodeChar = L'█'; //alt219
			m_lab.cells[i].Attributes = LightGray;
		} else if(wch == 'F'){
			m_lab.cells[i].Char.UnicodeChar = L'o';
			m_lab.cells[i].Attributes = Orange;
		} else if(wch == 'E'){
			m_lab.exit.X = i%m_lab.width; m_lab.exit.Y = i / m_lab.width;
			m_lab.cells[i].Char.UnicodeChar = L'▒'; //alt177
			m_lab.cells[i].Attributes = LightGreen;
		} else {
			if(wch == 'H'){ m_hero.pos.X = i%m_lab.width; m_hero.pos.Y = i / m_lab.width; }
			m_lab.cells[i].Char.UnicodeChar = L' ';
			m_lab.cells[i].Attributes = 0;
		}
		i++;
	}


	DWORD numWrChars = 0;
	SetConsoleCursorPosition(hConsole, { 0, m_lab.height + 4 });
	menu = L" Сытость: ";
	WriteConsoleOutputCharacter(hConsole, menu.c_str(), menu.length(), { 0, 1 }, &numWrChars);

	menu = L"Уровень " + to_wstring(m_menu.level);
	WriteConsoleOutputCharacter(hConsole, menu.c_str(), menu.length(), { width - 1 - (short)menu.length(), 1 }, &numWrChars);

	FillConsoleOutputCharacter(hConsole, L'─', width - 1, { 0, 2 }, &numWrChars);

	m_hero.init(m_menu.difficult);
	swprintf_s(wchsatiety, L"%*d", wcslen(wchsatiety), m_hero.satiety);
	WriteConsoleOutputCharacter(hConsole, wchsatiety, wcslen(wchsatiety), { 10, 1 }, &numWrChars);

	m_hero.move_start(&m_lab);


	wstring mp3file, vol = L"750";
	if(m_menu.level == 0){
		ifstream users("sound/user_song.mp3");
		if(users.good()){ mp3file = L"sound/user_song.mp3"; users.close(); } 
		else{ mp3file = L"sound/song_1.mp3"; }
	}
	else if(m_menu.level <= 5){ mp3file = L"sound/song_1.mp3"; } 
	else if(m_menu.level <= 7){ mp3file = L"sound/song_2.mp3"; }
	else if(m_menu.level <= 10){ mp3file = L"sound/song_3.mp3"; }
	else if(m_menu.level <= 12){ mp3file = L"sound/song_4.mp3"; }
	else if(m_menu.level <= 14){ mp3file = L"sound/song_5.mp3"; }
	else if(m_menu.level <= 16){ mp3file = L"sound/song_6.mp3"; }
	else if(m_menu.level == 17){ 
		vol = L"550";
		mp3file = L"sound/song_7.mp3"; 
	}

	//type mpegvideo | waveaudio
	mciSendString((L"open " + mp3file + L" type mpegvideo alias mysong").c_str(), NULL, 0, 0);
	mciSendString(L"open sound/yes.mp3 type mpegvideo alias yes", NULL, 0, 0);
	mciSendString((L"setaudio mysong volume to "+vol).c_str(), NULL, 0, 0);
	mciSendString(L"setaudio yes volume to 500", NULL, 0, 0);
	// from указывается в мс.
	if(mp3file == L"sound/song_3.mp3"){ mciSendString(L"play mysong from 3000 repeat", NULL, 0, 0); } 
	else{ mciSendString(L"play mysong repeat", NULL, 0, 0); }
	

	while(runGame());

	mciSendString(L"stop mysong", NULL, 0, 0);
	mciSendString(L"close all", NULL, 0, 0);
}

bool runGame(){
	bool Exit = false;
	static bool prompt = false;
	static clock_t start = clock();

	INPUT_RECORD eventType;
	DWORD numReading, numWrChars;
	WCHAR wch;

	GetNumberOfConsoleInputEvents(iConsole, &numReading);

	if(numReading){
		//ReadConsoleInput останавливает цикл до получения события.
		ReadConsoleInput(iConsole, &eventType, 1, &numReading);
		if(eventType.EventType == KEY_EVENT && eventType.Event.KeyEvent.bKeyDown){
			switch(eventType.Event.KeyEvent.wVirtualKeyCode){
			case VK_ESCAPE: Exit = true; break;
			case VK_UP:	m_hero.move_up(&m_lab); break;
			case VK_DOWN: m_hero.move_down(&m_lab); break;
			case VK_LEFT: m_hero.move_left(&m_lab); break;
			case VK_RIGHT: m_hero.move_right(&m_lab); break;
			default: break;
			}

			switch(eventType.Event.KeyEvent.wVirtualKeyCode){
			case VK_UP:	case VK_DOWN: case VK_LEFT: case VK_RIGHT: 

			wch = m_lab.coord(m_hero.pos.X, m_hero.pos.Y).Char.UnicodeChar;
			if(wch == L'o'){
				m_hero.satiety += m_hero.donut;
				m_lab.coord(m_hero.pos.X, m_hero.pos.Y).Char.UnicodeChar = L' ';

				swprintf_s(wchsatiety, L"%*d", wcslen(wchsatiety), m_hero.satiety);
				WriteConsoleOutputCharacter(hConsole, wchsatiety, wcslen(wchsatiety), { 10, 1 }, &numWrChars);
				
				start = clock(); 
				prompt = true;
				menu = L"Съел пончик! +"+to_wstring(m_hero.donut)+L"      ";
				WriteConsoleOutputCharacter(hConsole, menu.c_str(), menu.length(), { 1, 3 }, &numWrChars);

			} else if(wch == L'▒'){
				mciSendString(L"stop mysong", NULL, 0, 0);
				menu = L"Поздравляем! Вы нашли выход! :D";
				WriteConsoleOutputCharacter(hConsole, menu.c_str(), menu.length(), { 1, 3 }, &numWrChars);
				Sleep(100);
				mciSendString(L"play yes", NULL, 0, 0);

				if(m_menu.level == m_menu.maxlev && m_menu.level < m_menu.totalLev && m_menu.level != 0){
					m_menu.level = ++m_menu.maxlev;
					_itow_s(m_menu.maxlev, lab_set, 10);
					WritePrivateProfileStringW(L"GameSet", L"maxlevel", lab_set, L".\\Labirinth.ini");	
					WritePrivateProfileStringW(L"GameSet", L"level", lab_set, L".\\Labirinth.ini");
				} else if(m_menu.level == m_menu.totalLev){
					WritePrivateProfileStringW(L"GameSet", L"gamePassed", L"1", L".\\Labirinth.ini");
					m_menu.gamePassed = true;
				}

				if(m_menu.difficult == m_menu.maxdif){
					m_menu.blindLevels[m_menu.level-1] = 1;
					wstring blindlev = L"";
					for(short i = 0; i < m_menu.totalLev; i++){ 
						blindlev = blindlev + (m_menu.blindLevels[i]?L"1":L"0") + ((i == m_menu.totalLev - 1) ? L"" : L",");
					}
					WritePrivateProfileStringW(L"GameSet", L"blindLevels", blindlev.c_str(), L".\\Labirinth.ini");
				}

				Sleep(1400);
				pause();
				if(m_menu.level == m_menu.totalLev /*&& !m_menu.gamePassed*/) showEnd();
				Exit = true;
			}

			FlushConsoleInputBuffer(iConsole);
			Sleep(200);
			break; }
		} else if(eventType.EventType == MOUSE_EVENT){
			//cout << eventType.Event.MouseEvent.dwMousePosition.X;
		}
	}
 

	if(m_hero.died){
		menu = L"Вы умерли от голода! Игра окончена. :(";
		WriteConsoleOutputCharacter(hConsole, menu.c_str(), menu.length(), { 1, 3 }, &numWrChars);

		m_hero.die(&m_lab);
		mciSendString(L"stop mysong", NULL, 0, 0);
		Sleep(400);
		pause();
		Exit = true;
	}

	if(prompt && ((clock() - start) / (double)CLOCKS_PER_SEC) > promptTime){	
		prompt = false;
		menu = L"                                  ";
		WriteConsoleOutputCharacter(hConsole, menu.c_str(), menu.length(), { 1, 3 }, &numWrChars);//menu, wcslen(menu)
	}

	if(Exit) return false;


	if(m_hero.hunger()){
		swprintf_s(wchsatiety, L"%*d", wcslen(wchsatiety), m_hero.satiety);
		WriteConsoleOutputCharacter(hConsole, wchsatiety, wcslen(wchsatiety), { 10, 1 }, &numWrChars);
	}
	
	return true;
}

void showEnd(){
	mciSendString(L"open sound/song_7_credits.mp3 type mpegvideo alias mysong2", NULL, 0, 0);
	mciSendString(L"setaudio mysong2 volume to 40", NULL, 0, 0);
	mciSendString(L"play mysong2 repeat", NULL, 0, 0); 

	system("cls");
	DWORD numWrChars;
	menu = L"Послесловие.";
	WriteConsoleOutputCharacter(hConsole, menu.c_str(), menu.length(), { 1, 1 }, &numWrChars);
	FillConsoleOutputCharacter(hConsole, L'─', m_lab.width - 1, { 0, 2 }, &numWrChars);
	SetConsoleCursorPosition(hConsole, { 0, 3 });
	cout << "\n    Поздравляю, Вы прошли последний уровень, а значит и всю игру! Надеюсь, Вам понравилось)\n\n"
		"    Жизнь чем-то похожа на лабиринт - поиск решений, которые бывает очень сложно найти\n"
		" (особенно, когда слепой:D). Поэтому желаю удачи найти их так же как Вы находили выход здесь!\n\n"
		"    Теперь в меню доступен список используемых композиций.\n\n"
		"    Так же большая благодарность всем советам и примерам по работе с консолью, WinAPI и С++,\n"
		"  которые удалось найти на просторах интернета!\n\n"
		"    Успехов!";
	
	//while(1){mciSendString(L"play endloop from 300", NULL, 0, 0); Sleep(6750);}
	Sleep(1500);
	SetConsoleCursorPosition(hConsole, { 0, m_lab.height + 4 });
	pause();
	mciSendString(L"stop mysong2", NULL, 0, 0);
}


