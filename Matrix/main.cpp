// �������.cpp: ���������� ����� ����� ��� ����������� ����������.
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
	SetConsoleDisplayMode(consoleHandle, CONSOLE_FULLSCREEN_MODE, 0); // � ����� ������� ������
	SetConsoleTextAttribute(consoleHandle, FOREGROUND_GREEN); // ������ �����.

	CONSOLE_SCREEN_BUFFER_INFO csbi;
	GetConsoleScreenBufferInfo(consoleHandle, &csbi);

	// ����� ������ ������ �������, ����� �������� �������� �� ���������� ������������ ������� ������.
	csbi.dwSize.X = csbi.dwMaximumWindowSize.X;
	csbi.dwSize.Y = csbi.dwMaximumWindowSize.Y;
	SetConsoleScreenBufferSize(consoleHandle, csbi.dwSize);

	HWND consoleWindow = GetConsoleWindow();
	// ShowWindow(consoleWindow, SW_SHOWMAXIMIZED); // ��������������� ����.
	ShowScrollBar(consoleWindow, SB_BOTH, FALSE); // ������ ��������
	

	char content[3] = {' ', '0', '1'};
	int sec;
	bool watch = false;

	do{
		cin.clear();
		cin.ignore(cin.rdbuf()->in_avail());
		//cin.sync();
		cout << "������� ����� ��������� � ��������: ";
		cin >> sec;
		clock_t delay = sec * CLOCKS_PER_SEC;
		clock_t start = clock();

		while(clock() - start < delay)
		{
			cout << content[rand() % 3];
		}

		cin.clear();
		cin.ignore(cin.rdbuf()->in_avail());
		cout << endl << "��� ����������? (������� \"1\"(��) ��� \"0\"(���)): ";
		cin >> watch;

	} while(watch);


	return 0;
}

