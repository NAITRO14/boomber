#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <windows.h>
#include <iomanip>
#include <stdlib.h>
#include <string.h>

using namespace std;

enum ConsoleColor
{
	Black, Blue, Green, Cyan, Red, Magenta, Brown, LightGray, DarkGray,
	LightBlue, LightGreen, LightCyan, LightRed, LightMagenta, Yellow, White
};
void SetColor(int text, int background)
{
	HANDLE hStdOut = GetStdHandle(STD_OUTPUT_HANDLE);
	SetConsoleTextAttribute(hStdOut, (WORD)((background << 4) | text));
}
void GotoXY(int X, int Y)
{
	HANDLE hConsole;
	HANDLE hStdOut = GetStdHandle(STD_OUTPUT_HANDLE);
	COORD coord = { X, Y };
	SetConsoleCursorPosition(hStdOut, coord);
}

const int rows = 9;
const int cols = 9;
void print_field1(char _field[rows][cols]);




int main()
{
	srand(time(NULL));
	SetConsoleCP(1251);
	SetConsoleOutputCP(1251);

	//инициализация игрвого поля, в дальнейшем нолик будет заменятся на * для показывания мин
	char field[rows][cols] = {};
	for (short i = 0; i < rows; i++)
	{
		for (short j = 0; j < cols; j++)
		{
			field[i][j] = '0';
		}
	}
	
	print_field1(field);

	system("pause");
}

void print_field1(char _field[rows][cols])
{
	cout << "   ";
	//вывод номеров
	for (short j = 0; j < cols; j++)
	{
		cout << j << " ";
	}
	cout << endl;
	cout << "  +";
	for (short i = 0; i < cols * 2 - 1; i++)
	{
		cout << "-";
	}
	cout << "+" << endl;
	//выводит поле и ставит | по краям
	for (short i = 0; i < rows; i++)
	{
		cout << i << " |"; 
		 for (short j = 0; j < cols; j++) 
		 {
			 cout << _field[i][j];
			 if (j < cols - 1)//расставляет пробелы, и проверяет если символ не последний то ставит пробел
			 {
				cout << " ";
			 }
		 }
		cout << "|\n";
	}
	cout << "  +";
	for (short i = 0; i < cols * 2 - 1; i++)
	{
		cout << "-";
	}
	cout << "+" << endl;
	
}

	
	
