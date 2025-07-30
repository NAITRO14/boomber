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

const int rows = 10;         
const int cols = 10;
const int mines_count = 10;
const int max_moves = 15;   

void place_mines(char _field[rows][cols]);
void print_field1(char _field[rows][cols], bool _opened[rows][cols]);
void save_field_to_file(char _field[rows][cols], const char* _filename);
void open_empty(char _field[rows][cols], bool _opened[rows][cols], int _row, int _col);
// true если мина, false если безопасно
bool open(char _field[rows][cols], bool _opened[rows][cols], int _row, int _col);
bool inbounds(int row, int col);


int main()
{
    srand(time(NULL));
    SetConsoleCP(CP_UTF8);
    SetConsoleOutputCP(CP_UTF8);

    char field[rows][cols];
    bool opened[rows][cols] = { false };

    // Инициализация массива
    for (short i = 0; i < rows; i++)
    {
        for (short j = 0; j < cols; j++)
        {
            field[i][j] = '0';
        }
    }
    place_mines(field);

    // такие же команды что и в функции по открытию пустых клеток
    int dx[] = { -1,-1,-1, 0, 0, 1, 1, 1 };
    int dy[] = { -1, 0, 1,-1, 1,-1, 0, 1 };
    for (int i = 0; i < rows; i++)
    {
        for (int j = 0; j < cols; j++)
        {
            if (field[i][j] == '*')
            {
                continue;//мина пропускаем
            }
            int count = 0;
            for (int k = 0; k < 8; k++)
            {
                int num1 = i + dx[k], num2 = j + dy[k];// помогает проверять соседние клетки тожже нейронка прикольно придумала, взял у нее
                if (inbounds(num1,num2) && field[num1][num2] == '*')//если мина счетчик +
                {
                    count++;
                }
            }
            field[i][j] = count + '0';// записывает кол во мин в пустую клетку
        }
    }




    int moves = 0;
    bool game_over = false;

    string input;
    int r, c;
    bool hit;
    while (moves < max_moves && !game_over)
    {
        print_field1(field, opened);

        cout << "Введите размеры(стобец и строку)" << endl;
        cout << "Ввод: ";
        getline(cin, input);
        system("cls");
        // проверка на ввод, чтоб длина строки была не больше 2 символов и вводилось строка столбец
        if (input.length() != 2 || !isdigit(input[0]) || !isdigit(input[1]))
        {
            continue;
        }
        r = input[0]-1 - '0';
        c = input[1]-1 - '0';

        if (r < 0 || r >= rows || c < 0 || c >= cols)
        {
            continue;
        }

        if (opened[r][c])
        {
            continue;          
        }

        hit = open(field, opened, r, c);
        moves++;

        if (hit)
        {
            // Если мину открыли,открываем всё поле и показываем
            for (int i = 0; i < rows; ++i)
            {
                for (int j = 0; j < cols; ++j)
                {
                    opened[i][j] = true;
                }
            }
            print_field1(field, opened);
            
            break;  // Конец игры
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
    save_field_to_file(_field, "MINES.txt");
}


void print_field1(char _field[rows][cols], bool _opened[rows][cols])
{
    cout << "   ";  
    for (int j = 1; j < cols+1; j++)
    {
        cout << j << " ";   // Верхняя строка с номерами столбцов
        
    }
    cout << "\n  +";
    for (int i = 0; i < cols * 2 - 1; i++)
    {
        cout << "-"; // Верхняя рамка
       
    }
    //--------------
    cout << "+\n";
    for (int i = 0; i < rows; i++) 
    {
        cout << i+1 << " |";
        for (int j = 0; j < cols; j++)
        {
            if (_opened[i][j])
            {   // Если клетка открыта

                if (_field[i][j] == '*')
                {
                   // Мина
                    cout << _field[i][j];
                }
                else if (_field[i][j] == '0')
                {
                    // Пустая клетка
                    cout << '-';
                }
                else
                {
                    cout << ch;
                }
            }
            else
            {                 
                // Если клетка закрытая то вывод 0
                cout << "0";              
            }
            if (j < cols - 1)//расставляет пробелы и проверяет если символ не последний то ставит пробел
            {
                cout << " ";
            }
        }
        cout << "|\n";
    }
    cout << "  +";   
    for (int i = 0; i < cols * 2 - 1; i++)
    {
        cout << "-"; // Нижняя рамка
    }
    cout << "+\n";
}

void save_field_to_file(char _field[rows][cols], const char* _filename)
{
    FILE* file = fopen("MINES.txt", "w");
    if (!file)
    {
        cout << "Ошибка записи !" << endl;
        return;
    }
    else
    {
     
        // Записываем номера столбцов
        fprintf(file, "   ");
        for (short j = 1; j < cols+1; j++)
        {
            fprintf(file, "%d ", j);
        }
        fprintf(file, "\n");

        // Записываем верхнюю границу
        fprintf(file, "  +");
        for (short i = 0; i < cols * 2 - 1; i++)
        {
            fprintf(file, "-");
        }
        fprintf(file, "+\n");

        // Записываем строки поля
        for (short i = 0; i < rows; i++)
        {
            fprintf(file, "%d |", i+1); // Номер строки
            for (short j = 0; j < cols; j++)
            {
                fprintf(file, "%c", _field[i][j]); // Символ поля
                if (j < cols - 1)
                    fprintf(file, " "); // Пробел между символами
            }
            fprintf(file, "|\n"); // Закрывающая граница строки
        }

        // Записываем нижнюю границу
        fprintf(file, "  +");
        for (short i = 0; i < cols * 2 - 1; i++)
        {
            fprintf(file, "-");
        }
        fprintf(file, "+\n");
    }
    fclose(file);
    cout << "The playing field has been successfully saved to a file: MINES.txt" << endl;
}

void open_empty(char _field[rows][cols], bool _opened[rows][cols], int _row, int _col)
{
    if (!inbounds(_row, _col))
    {
        return;
    }
    if (_opened[_row][_col])
    {
        return;
    }
    _opened[_row][_col] = true;
    if (_field[_row][_col] != '0')
    {
        return;
    }
    // массивы для проверки мин вокруг открытой клетки проще говоря проверка в дальнейшем 8 клеток вокруг квадратика
    int dx[] = { -1, -1, -1,  0, 0, 1, 1, 1 };
    int dy[] = { -1,  0,  1, -1, 1,-1, 0, 1 };
    // вызываем функцию для открытия соседних клеток
    for (int k = 0; k < 8; k++)
    {
        open_empty(_field, _opened, _row + dx[k], _col + dy[k]);
    }
}

bool inbounds(int row, int col)
{
    return row >= 0 && row < rows && col >= 0 && col < cols;
}



bool open(char _field[rows][cols], bool _opened[rows][cols], int _row, int _col)
{
    if (_field[_row][_col] == '*')
    {
        _opened[_row][_col] = true;
        return true; // мина
    }
    else if (_field[_row][_col] == '0')
    {
        open_empty(_field, _opened, _row, _col);
    }
    else
    {
        _opened[_row][_col] = true;
    }
    return false;
}
