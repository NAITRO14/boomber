#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <windows.h>
#include <iomanip>
#include <stdlib.h>
#include <string.h>
#include <string>
#include <sstream> // Для обработки ввода

using namespace std;

enum ConsoleColor {
    Black, Blue, Green, Cyan, Red, Magenta, Brown, LightGray, DarkGray,
    LightBlue, LightGreen, LightCyan, LightRed, LightMagenta, Yellow, White
};

void SetColor(int text, int background) {
    HANDLE hStdOut = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleTextAttribute(hStdOut, (WORD)((background << 4) | text));
}

// Структура для хранения параметров уровня сложности
struct GameLevel
{
    int rows;
    int cols;
    int mines_count;
    string name;
};

const GameLevel levels[] =
{
    {10, 10, 10, "легкий"},
    {15, 15, 23, "средний"},
    {20, 25, 50, "сложный"}
};

const int LEVELS_COUNT = sizeof(levels) / sizeof(levels[0]);

// Прототипы функций
void place_mines(char** _field, int rows, int cols, int mines_count, int first_row, int first_col);
void print_field1(char** _field, bool** _opened, int rows, int cols, int moves);
void save_field_to_file(char** _field, int rows, int cols, const char* _filename);
void open_empty(char** _field, bool** _opened, int rows, int cols, int _row, int _col);
bool open(char** _field, bool** _opened, int rows, int cols, int _row, int _col);
bool inbounds(int row, int col, int rows, int cols);
void initialize_field(char** field, bool** opened, int rows, int cols);
void calculate_numbers(char** field, int rows, int cols);

int main() {
    srand(time(NULL));
    SetConsoleCP(CP_UTF8);
    SetConsoleOutputCP(CP_UTF8);

    // Выбор уровня сложности
    cout << "Выберите уровень сложности:" << endl;
    for (int i = 0; i < LEVELS_COUNT; i++) {
        cout << i + 1 << ". " << levels[i].name
            << " (" << levels[i].rows << "x" << levels[i].cols
            << ", мин: " << levels[i].mines_count << ")" << endl;
    }

    int choice;
    do {
        cout << "Ваш выбор (1-" << LEVELS_COUNT << "): ";
        cin >> choice;
        cin.ignore(); // Очищаем буфер ввода
    } while (choice < 1 || choice > LEVELS_COUNT);

    const GameLevel& level = levels[choice - 1];
    int rows = level.rows;
    int cols = level.cols;
    int mines_count = level.mines_count;

    // Динамическое создание массивов
    char** field = new char* [rows];
    bool** opened = new bool* [rows];
    initialize_field(field, opened, rows, cols);

    int moves = 0;
    bool game_over = false;
    int first_row = -1, first_col = -1;

    while (!game_over) {
        print_field1(field, opened, rows, cols, moves);
        cout << "Уровень: " << level.name << endl;
        cout << "Ходы: " << moves << endl;

        // Получаем ввод пользователя
        cout << "Введите координаты (строка столбец) через пробел: ";
        string input;
        getline(cin, input);

        stringstream ss(input);
        int r, c;
        if (!(ss >> r >> c)) {
            cout << "Ошибка: введите два числа через пробел " << endl;
            continue;
        }

        // Преобразуем в индексы массива (1-based -> 0-based)
        r--; c--;

        // Проверяем границы
        if (!inbounds(r, c, rows, cols)) {
            cout << "Ошибка: координаты вне диапазона (1-" << rows << " 1-" << cols << ")" << endl;
            continue;
        }

        if (opened[r][c])
        {
            cout << "Эта клетка уже открыта!" << endl;
            continue;
        }

        // Первый ход - размещаем мины после него, избегая первой клетки
        if (moves == 0)
        {
            first_row = r;
            first_col = c;
            place_mines(field, rows, cols, mines_count, first_row, first_col);
            calculate_numbers(field, rows, cols);
        }

        bool hit = open(field, opened, rows, cols, r, c);
        moves++;

        if (hit)
        {
            // Открываем все мины при проигрыше
            for (int i = 0; i < rows; i++)
            {
                for (int j = 0; j < cols; j++)
                {
                    if (field[i][j] == '*') { opened[i][j] = true; }
                }
            }
            print_field1(field, opened, rows, cols, moves);
            cout << "Игра окончена! Вы наступили на мину." << endl;
            game_over = true;
        }
    }

    // Освобождение памяти
    for (int i = 0; i < rows; i++)
    {
        delete[] field[i];
        delete[] opened[i];
    }
    delete[] field;
    delete[] opened;

    return 0;
}

void initialize_field(char** field, bool** opened, int rows, int cols) {
    for (int i = 0; i < rows; i++) {
        field[i] = new char[cols];
        opened[i] = new bool[cols];
        for (int j = 0; j < cols; j++) {
            field[i][j] = '0'; // Временно заполняем нулями
            opened[i][j] = false;
        }
    }
}

void place_mines(char** _field, int rows, int cols, int mines_count, int first_row, int first_col) {
    int mines_placed = 0;
    while (mines_placed < mines_count) {
        int x = rand() % rows;
        int y = rand() % cols;

        // Не ставим мину в первую клетку и в соседние клетки
        if ((x == first_row && y == first_col) ||
            (abs(x - first_row) <= 1 && abs(y - first_col) <= 1)) {
            continue;
        }

        if (_field[x][y] != '*') {
            _field[x][y] = '*';
            mines_placed++;
        }
    }
    save_field_to_file(_field, rows, cols, "MINES.txt");
}

void calculate_numbers(char** field, int rows, int cols) {
    int dx[] = { -1,-1,-1, 0, 0, 1, 1, 1 };
    int dy[] = { -1, 0, 1,-1, 1,-1, 0, 1 };

    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            if (field[i][j] == '*') continue;

            int count = 0;
            for (int k = 0; k < 8; k++) {
                int ni = i + dx[k], nj = j + dy[k];
                if (inbounds(ni, nj, rows, cols) && field[ni][nj] == '*') {
                    count++;
                }
            }
            field[i][j] = '-';

        }
    }
}

void print_field1(char** _field, bool** _opened, int rows, int cols, int moves) {

    Sleep(2000);
    system("cls");

    // Вывод номеров столбцов (двузначные числа)
    cout << "    ";
    for (int j = 1; j <= cols; j++) {
        cout << setw(2) << j << " ";
    }
    cout << "\n   +";
    for (int i = 0; i < cols * 3 - 1; i++) {
        cout << "-";
    }
    cout << "+\n";

    // Вывод строк поля
    for (int i = 0; i < rows; i++) {
        cout << setw(2) << i + 1 << " |"; // Номер строки
        for (int j = 0; j < cols; j++) {
            if (_opened[i][j]) {
                if (_field[i][j] == '*') {
                    cout << " * ";
                }
                else if (_field[i][j] == '.') {
                    cout << " . ";
                }
                else {
                    cout << " " << _field[i][j] << " ";
                }
            }
            else {
                cout << " 0 ";
            }
        }
        cout << "|\n";
    }

    // Нижняя граница
    cout << "   +";
    for (int i = 0; i < cols * 3 - 1; i++) {
        cout << "-";
    }
    cout << "+\n";
}

void save_field_to_file(char** _field, int rows, int cols, const char* _filename) {
    FILE* file = fopen(_filename, "w");
    if (!file) {
        cout << "Ошибка записи!" << endl;
        return;
    }

    // Записываем номера столбцов
    fprintf(file, "    ");
    for (int j = 1; j <= cols; j++) {
        fprintf(file, "%2d ", j);
    }
    fprintf(file, "\n");

    // Верхняя граница
    fprintf(file, "   +");
    for (int i = 0; i < cols * 3 - 1; i++) {
        fprintf(file, "-");
    }
    fprintf(file, "+\n");

    // Записываем строки поля
    for (int i = 0; i < rows; i++) {
        fprintf(file, "%2d |", i + 1); // Номер строки
        for (int j = 0; j < cols; j++) {
            fprintf(file, " %c ", _field[i][j]);
        }
        fprintf(file, "|\n");
    }

    // Нижняя граница
    fprintf(file, "   +");
    for (int i = 0; i < cols * 3 - 1; i++) {
        fprintf(file, "-");
    }
    fprintf(file, "+\n");

    fclose(file);
}

void open_empty(char** _field, bool** _opened, int rows, int cols, int _row, int _col) {
    if (!inbounds(_row, _col, rows, cols)) return;
    if (_opened[_row][_col]) return;

    _opened[_row][_col] = true;
    if (_field[_row][_col] != '.') return;

    int dx[] = { -1, -1, -1, 0, 0, 1, 1, 1 };
    int dy[] = { -1, 0, 1, -1, 1, -1, 0, 1 };

    for (int k = 0; k < 8; k++) {
        open_empty(_field, _opened, rows, cols, _row + dx[k], _col + dy[k]);
    }
}

bool open(char** _field, bool** _opened, int rows, int cols, int _row, int _col) {
    if (_field[_row][_col] == '*') {
        _opened[_row][_col] = true;
        return true; // мина
    }
    else if (_field[_row][_col] == '.') {
        open_empty(_field, _opened, rows, cols, _row, _col);
    }
    else {
        _opened[_row][_col] = true;
    }
    return false;
}

bool inbounds(int row, int col, int rows, int cols) {
    return row >= 0 && row < rows && col >= 0 && col < cols;
}