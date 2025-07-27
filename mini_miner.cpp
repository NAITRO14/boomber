#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <windows.h>
#include <iomanip>
#include <stdlib.h>
#include <string.h>
#include <string>

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
    HANDLE hStdOut = GetStdHandle(STD_OUTPUT_HANDLE);
    COORD coord = { (SHORT)X, (SHORT)Y };
    SetConsoleCursorPosition(hStdOut, coord);
}

const int rows = 5;         
const int cols = 5;
const int mines_count = 10;
const int max_moves = 15;   

void place_mines(char _field[rows][cols]);
void print_field1(char _field[rows][cols], bool _opened[rows][cols]);
void save_field_to_file(char _field[rows][cols], const char* _filename);
// true ���� ����, false ���� ���������
bool open(char _field[rows][cols], bool _opened[rows][cols], int _row, int _col);


// ����� �����
int main()
{
    srand(time(NULL));
    SetConsoleCP(1251);
    SetConsoleOutputCP(1251);

    char field[rows][cols];
    bool opened[rows][cols] = { false };

    // ������������� �������
    for (short i = 0; i < rows; i++)
    {
        for (short j = 0; j < cols; j++)
        {
            field[i][j] = '0';
        }
    }
    place_mines(field);      // ����������� ����
    int moves = 0;
    bool game_over = false;

    while (moves < max_moves && !game_over)
    {
        print_field1(field, opened);

        string input;
        getline(cin, input);

        // �������� �� ����, ���� ����� ������ ���� �� ������ 2 �������� � ��������� ������ �������
        if (input.length() != 2 || !isdigit(input[0]) || !isdigit(input[1]))
        {
            continue;
        }
        int r = input[0] - '0';
        int c = input[1] - '0';

        if (opened[r][c])
        {
            continue;          // ���� ��� ��������� � ������ �����
        }

        bool hit = open(field, opened, r, c);
        moves++;

        if (hit)
        {
            // ���� ���� �������,��������� �� ���� � ����������
            for (int i = 0; i < rows; ++i)
            {
                for (int j = 0; j < cols; ++j)
                {
                    opened[i][j] = true;
                }
            }
            print_field1(field, opened);
            save_field_to_file(field, "MINES.txt");
            break;  // ����� ����
        }
    }

    if (!game_over && moves >= max_moves)
    {
        print_field1(field, opened);
        
    }


}

void place_mines(char _field[rows][cols])
{
    int mines_placed = 0;
    while (mines_placed < mines_count)
    {
        int x = rand() % rows;
        int y = rand() % cols;
        if (_field[x][y] != '*')
        { 
            _field[x][y] = '*';
            mines_placed++;
        }
    }
}


void print_field1(char _field[rows][cols], bool _opened[rows][cols])
{
    cout << "   ";  
    for (int j = 0; j < cols; j++)
    {
        cout << j << " ";   // ������� ������ � �������� ��������
        
    }
    cout << "\n  +";
    for (int i = 0; i < cols * 2 - 1; i++)
    {
        cout << "-"; // ������� �����
       
    }
    cout << "+\n";
    for (int i = 0; i < rows; i++) 
    {
        cout << i << " |";
        for (int j = 0; j < cols; j++)
        {
            if (_opened[i][j])
            {   // ���� ������ �������
                char ch = _field[i][j];

                if (ch == '*')
                {
                   // ����
                    cout << ch;
                }
                else if (ch == '0')
                {
                    // ������ ������
                    cout << '-';
                }      
            }
            else
            {                 
                // ���� ������ �������� �� ����� 0
                cout << "0";              
            }
            if (j < cols - 1)//����������� ������� � ��������� ���� ������ �� ��������� �� ������ ������
            {
                cout << " ";
            }
        }
        cout << "|\n";
    }
    cout << "  +";   
      for (int i = 0; i < cols * 2 - 1; i++)
      {
          cout << "-"; // ������ �����
      }
      cout << "+\n";
}

void save_field_to_file(char _field[rows][cols], const char* _filename)
{
    FILE* file = fopen("MINES.txt", "w");

    if (!file)
    {
        cout << "������ �������� ����� ��� ������!" << endl;
        return;
    }
    else {

        // ���������� ������ ��������
        fprintf(file, "   ");
        for (short j = 0; j < cols; j++)
        {
            fprintf(file, "%d ", j);
        }
        fprintf(file, "\n");

        // ���������� ������� �������
        fprintf(file, "  +");
        for (short i = 0; i < cols * 2 - 1; i++)
        {
            fprintf(file, "-");
        }
        fprintf(file, "+\n");

        // ���������� ������ ����
        for (short i = 0; i < rows; i++)
        {
            fprintf(file, "%d |", i); // ����� ������
            for (short j = 0; j < cols; j++)
            {
                fprintf(file, "%c", _field[i][j]); // ������ ����
                if (j < cols - 1)
                {
                    fprintf(file, " "); // ������ ����� ���������
                }
            }
            fprintf(file, "|\n"); // ����������� ������� ������
        }

        // ���������� ������ �������
        fprintf(file, "  +");
        for (short i = 0; i < cols * 2 - 1; i++)
        {
            fprintf(file, "-");
        }
        fprintf(file, "+\n");
    }
    fclose(file); // ��������� ����
    cout << "������� ���� ������� ��������� � ����: " << "MINES.txt" << endl;
}

bool open(char _field[rows][cols], bool _opened[rows][cols], int _row, int _col)
{
   _opened[_row][_col] = true;
    return _field[_row][_col] == '*';
}
