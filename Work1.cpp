#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <windows.h>
#include <iomanip>
#include <stdlib.h>
#include <string.h>
#include <string>

using namespace std;

enum ConsoleColor {
    Black, Blue, Green, Cyan, Red, Magenta, Brown, LightGray, DarkGray,
    LightBlue, LightGreen, LightCyan, LightRed, LightMagenta, Yellow, White
};

void SetColor(int text, int background) {
    HANDLE hStdOut = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleTextAttribute(hStdOut, (WORD)((background << 4) | text));
}

void GotoXY(int X, int Y) {
    HANDLE hStdOut = GetStdHandle(STD_OUTPUT_HANDLE);
    COORD coord = { (SHORT)X, (SHORT)Y };
    SetConsoleCursorPosition(hStdOut, coord);
}

//движение вокруг клетки
int dx[] = { -1,-1,-1, 0, 0, 1, 1, 1 };
int dy[] = { -1, 0, 1,-1, 1,-1, 0, 1 };

// Структура для хранения параметров уровня сложности
struct GameLevel {
    int rows;
    int cols;
    int mines_count;
    int max_moves;
    int max_lives;
    string name;
};

GameLevel levels[] = {
    {10, 10, 10, 50, 3, "легкий"},    
    {15, 15, 23, 75, 2, "средний"},   
    {20, 25, 50, 100, 1, "сложный"}  
};

void place_mines(char** _field, int rows, int cols, int mines_count);
void print_field1(char** _field, bool** _opened, int rows, int cols);
void save_field_to_file(char** _field, int rows, int cols, const char* _filename);
void open_empty(char** _field, bool** _opened, int rows, int cols, int _row, int _col);
bool open(char** _field, bool** _opened, int rows, int cols, int _row, int _col);
bool inbounds(int row, int col, int rows, int cols);

int main() {
    srand(time(NULL));
    SetConsoleCP(CP_UTF8);
    SetConsoleOutputCP(CP_UTF8);

    // Вывод уровней сложности
    cout << "Выберите уровень сложности:" << endl;
    for (int i = 0; i < 3; i++) {
        cout << i + 1 << ". " << levels[i].name
            << " (" << levels[i].rows << "x" << levels[i].cols
            << ", мин: " << levels[i].mines_count << ")" << endl;
    }

    int choice;
    do {
        cout << "Ваш выбор (1-3): ";
        cin >> choice;
        cin.ignore(); // Очищаем буфер ввода
    } while (choice < 1 || choice > 3);

    //Взять параметры из структуры
    const GameLevel& level = levels[choice - 1];
    int rows = level.rows;
    int cols = level.cols;
    int mines_count = level.mines_count;
    int max_moves = level.max_moves;
    int max_lives = level.max_lives;

    // Динамическое создание массивов
    char** field = new char* [rows];
    bool** opened = new bool* [rows];
    for (int i = 0; i < rows; i++) {
        field[i] = new char[cols];
        opened[i] = new bool[cols];
        for (int j = 0; j < cols; j++) {
            field[i][j] = '0';
            opened[i][j] = false;
        }
    }

    place_mines(field, rows, cols, mines_count);

    
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            if (field[i][j] == '*') continue;
            int count = 0;
            for (int k = 0; k < 8; k++) {
                int num1 = i + dx[k], num2 = j + dy[k];
                if (inbounds(num1, num2, rows, cols) && field[num1][num2] == '*') {
                    count++;
                }
            }
            field[i][j] = count + '0';
        }
    }

    int moves = 0;
    bool game_over = false;
    int lives = max_lives;
    string input;
    int r, c;
    bool hit;

    while (moves < max_moves && !game_over && lives > 0) {
        print_field1(field, opened, rows, cols);
        cout << "Уровень: " << level.name << endl;
        cout << "Жизни: " << lives << "/" << max_lives
            << " | Ходы: " << moves << "/" << max_moves << endl;
        cout << "Введите координаты (строка столбец): ";
        getline(cin, input);
        system("cls");

        if (input.length() != 2 || !isdigit(input[0]) || !isdigit(input[1])) {
            cout << "Ошибка: введите 2 цифры (например, 12 для строки 1 столбца 2)" << endl;
            continue;
        }

        //привод цифры к строке
        r = input[0] - '0' - 1;
        c = input[1] - '0' - 1;

        if (r < 0 || r >= rows || c < 0 || c >= cols) {
            cout << "Ошибка: координаты вне диапазона (1-" << rows << " 1-" << cols << ")" << endl;
            continue;
        }

        if (opened[r][c]) {
            cout << "Эта клетка уже открыта!" << endl;
            continue;
        }

        hit = open(field, opened, rows, cols, r, c);
        moves++;

        if (hit) {
            lives--;
            if (lives > 0) {
                cout << "Вы нашли мину! Осталось жизней: " << lives << endl;
                opened[r][c] = true;
                print_field1(field, opened, rows, cols);
                cout << "Продолжаем игру..." << endl;
                Sleep(2000);
                system("cls");
            }
            else {
                for (int i = 0; i < rows; ++i) {
                    for (int j = 0; j < cols; ++j) {
                        opened[i][j] = true;
                    }
                }
                print_field1(field, opened, rows, cols);
                cout << "Игра окончена! Вы исчерпали все жизни." << endl;
                game_over = true;
            }
        }
    }

    if (!game_over && lives > 0) {
        if (moves >= max_moves) {
            cout << "Поздравляем! Вы сделали " << max_moves << " ходов и выиграли!" << endl;
        }
        else {
            cout << "Игра завершена." << endl;
        }
    }

    // Освобождение памяти
    for (int i = 0; i < rows; i++) {
        delete[] field[i];
        delete[] opened[i];
    }
    delete[] field;
    delete[] opened;

    return 0;
}


void place_mines(char** _field, int rows, int cols, int mines_count) {
    int mines_placed = 0;
    while (mines_placed < mines_count) {
        int x = rand() % rows;
        int y = rand() % cols;
        if (_field[x][y] != '*') {
            _field[x][y] = '*';
            mines_placed++;
        }
    }
    save_field_to_file(_field, rows, cols, "MINES.txt");
}

void print_field1(char** _field, bool** _opened, int rows, int cols) {
    cout << "   ";
    for (int j = 1; j < cols + 1; j++) {
        cout << j % 10 << " "; // Выводим только последнюю цифру для двузначных чисел
    }
    cout << "\n  +";
    for (int i = 0; i < cols * 2 - 1; i++) {
        cout << "-";
    }
    cout << "+\n";

    for (int i = 0; i < rows; i++) {
        cout << (i + 1) % 10 << " |"; // Выводим только последнюю цифру для двузначных чисел
        for (int j = 0; j < cols; j++) {
            if (_opened[i][j]) {
                if (_field[i][j] == '*') {
                    cout << _field[i][j];
                }
                else if (_field[i][j] == '0') {
                    cout << '.';
                }
                else {
                    cout << _field[i][j];
                }
            }
            else {
                cout << "0";
            }
            if (j < cols - 1) {
                cout << " ";
            }
        }
        cout << "|\n";
    }

    cout << "  +";
    for (int i = 0; i < cols * 2 - 1; i++) {
        cout << "-";
    }
    cout << "+\n";
}

void save_field_to_file(char** _field, int rows, int cols, const char* _filename)
{

    FILE* file = fopen(_filename, "w");
    if (!file) {
        cout << "Ошибка записи !" << endl;
        return;
    }

    // Записываем номера столбцов
    fprintf(file, "   ");
    for (int j = 1; j <= cols; j++) {
        fprintf(file, "%d ", j % 10); // Только последняя цифра для больших чисел
    }
    fprintf(file, "\n");

    // Верхняя граница
    fprintf(file, "  +");
    for (int i = 0; i < cols * 2 - 1; i++) {
        fprintf(file, "-");
    }
    fprintf(file, "+\n");

    // Записываем строки поля
    for (int i = 0; i < rows; i++) {
        fprintf(file, "%d |", (i + 1) % 10); // Номер строки
        for (int j = 0; j < cols; j++) {
            fprintf(file, "%c", _field[i][j]);
            if (j < cols - 1) {
                fprintf(file, " ");
            }
        }
        fprintf(file, "|\n");
    }

    // Нижняя граница
    fprintf(file, "  +");
    for (int i = 0; i < cols * 2 - 1; i++) {
        fprintf(file, "-");
    }
    fprintf(file, "+\n");

    fclose(file);
    cout << "Игровое поле успешно сохранено в файл: " << _filename << endl;
}

void open_empty(char** _field, bool** _opened, int rows, int cols, int _row, int _col)
{
    if (!inbounds(_row, _col, rows, cols)) {
        return;
    }
    if (_opened[_row][_col]) {
        return;
    }
    _opened[_row][_col] = true;
    if (_field[_row][_col] != '0') {
        return;
    }

    int dx[] = { -1, -1, -1, 0, 0, 1, 1, 1 };
    int dy[] = { -1, 0, 1, -1, 1, -1, 0, 1 };

    for (int k = 0; k < 8; k++) {
        open_empty(_field, _opened, rows, cols, _row + dx[k], _col + dy[k]);
    }
}

bool open(char** _field, bool** _opened, int rows, int cols, int _row, int _col)
{
    if (_field[_row][_col] == '*') {
        _opened[_row][_col] = true;
        return true; // мина
    }
    else if (_field[_row][_col] == '0') {
        open_empty(_field, _opened, rows, cols, _row, _col);
    }
    else {
        _opened[_row][_col] = true;
    }

    return false;
}

bool inbounds(int row, int col, int rows, int cols)
{
    return row >= 0 && row < rows && col >= 0 && col < cols;
}