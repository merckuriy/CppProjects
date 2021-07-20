// Матрица.cpp: определяет точку входа для консольного приложения.
//

//#include "stdafx.h"
#include <iostream>
#include <ctime>

#define WIN32_LEAN_AND_MEAN
#include<windows.h>

int main()
{
	using namespace std;
	setlocale(LC_ALL, "Rus");

	HANDLE consoleHandle = GetStdHandle(STD_OUTPUT_HANDLE);
	SetConsoleDisplayMode(consoleHandle, CONSOLE_FULLSCREEN_MODE, 0); // В режим полного экрана
	SetConsoleTextAttribute(consoleHandle, FOREGROUND_GREEN); // Зелёный текст.

	CONSOLE_SCREEN_BUFFER_INFO csbi;
	GetConsoleScreenBufferInfo(consoleHandle, &csbi);

	// Задаём точный размер консоли, иначе скролбар появится по достижении вертикальной границы экрана.
	csbi.dwSize.X = csbi.dwMaximumWindowSize.X;
	csbi.dwSize.Y = csbi.dwMaximumWindowSize.Y;
	SetConsoleScreenBufferSize(consoleHandle, csbi.dwSize);

	HWND consoleWindow = GetConsoleWindow();
	// ShowWindow(consoleWindow, SW_SHOWMAXIMIZED); // Максимизировать окно.
	ShowScrollBar(consoleWindow, SB_BOTH, FALSE); // Убрать скролбар
	

	char content[3] = {' ', '0', '1'};
	int sec;
	bool watch = false;

	do{
		cin.clear();
		cin.ignore(cin.rdbuf()->in_avail());
		//cin.sync();
		cout << "Введите время просмотра в секундах: ";
		cin >> sec;
		clock_t delay = sec * CLOCKS_PER_SEC;
		clock_t start = clock();

		while(clock() - start < delay)
		{
			cout << content[rand() % 3];
		}

		cin.clear();
		cin.ignore(cin.rdbuf()->in_avail());
		cout << endl << "Ещё посмотреть? (введите \"1\"(да) или \"0\"(нет)): ";
		cin >> watch;

	} while(watch);


	return 0;
}

