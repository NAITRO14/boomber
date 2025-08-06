#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <windows.h>
#include <iomanip>
#include <stdlib.h>
#include <string.h>
#include <string>
#include <sstream> // ��� ��������� �����

using namespace std;

enum ConsoleColor {
    Black, Blue, Green, Cyan, Red, Magenta, Brown, LightGray, DarkGray,
    LightBlue, LightGreen, LightCyan, LightRed, LightMagenta, Yellow, White
};

void SetColor(int text, int background) {
    HANDLE hStdOut = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleTextAttribute(hStdOut, (WORD)((background << 4) | text));
}

// ��������� ��� �������� ���������� ������ ���������
struct GameLevel {
    int rows;
    int cols;
    int mines_count;
    string name;
};

const GameLevel levels[] = {
    {10, 10, 10, "������"},    // ������ �������
    {15, 15, 23, "�������"},   // ������� �������
    {20, 25, 50, "�������"}    // ������� �������
};

const int LEVELS_COUNT = sizeof(levels) / sizeof(levels[0]);

// ��������� �������
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
    SetConsoleCP(1251);
    SetConsoleOutputCP(1251);

    // ����� ������ ���������
    cout << "�������� ������� ���������:" << endl;
    for (int i = 0; i < LEVELS_COUNT; i++) {
        cout << i + 1 << ". " << levels[i].name
            << " (" << levels[i].rows << "x" << levels[i].cols
            << ", ���: " << levels[i].mines_count << ")" << endl;
    }

    int choice;
    do {
        cout << "��� ����� (1-" << LEVELS_COUNT << "): ";
        cin >> choice;
        cin.ignore(); // ������� ����� �����
    } while (choice < 1 || choice > LEVELS_COUNT);

    const GameLevel& level = levels[choice - 1];
    int rows = level.rows;
    int cols = level.cols;
    int mines_count = level.mines_count;

    // ������������ �������� ��������
    char** field = new char* [rows];
    bool** opened = new bool* [rows];
    initialize_field(field, opened, rows, cols);

    int moves = 0;
    bool game_over = false;
    int first_row = -1, first_col = -1;

    while (!game_over) {
        print_field1(field, opened, rows, cols, moves);
        cout << "�������: " << level.name << endl;
        cout << "����: " << moves << endl;

        // �������� ���� ������������
        cout << "������� ���������� (������ �������): ";
        string input;
        getline(cin, input);

        stringstream ss(input);
        int r, c;
        if (!(ss >> r >> c)) {
            cout << "������: ������� ��� ����� ����� ������ (��������: 5 10)" << endl;
            continue;
        }

        // ����������� � ������� ������� (1-based -> 0-based)
        r--; c--;

        // ��������� �������
        if (!inbounds(r, c, rows, cols)) {
            cout << "������: ���������� ��� ��������� (1-" << rows << " 1-" << cols << ")" << endl;
            continue;
        }

        if (opened[r][c]) {
            cout << "��� ������ ��� �������!" << endl;
            continue;
        }

        // ������ ��� - ��������� ���� ����� ����, ������� ������ ������
        if (moves == 0) {
            first_row = r;
            first_col = c;
            place_mines(field, rows, cols, mines_count, first_row, first_col);
            calculate_numbers(field, rows, cols);
        }

        bool hit = open(field, opened, rows, cols, r, c);
        moves++;

        if (hit) {
            // ��������� ��� ���� ��� ���������
            for (int i = 0; i < rows; i++) {
                for (int j = 0; j < cols; j++) {
                    if (field[i][j] == '*') opened[i][j] = true;
                }
            }
            print_field1(field, opened, rows, cols, moves);
            cout << "���� ��������! �� ��������� �� ����." << endl;
            game_over = true;
        }
    }

    // ������������ ������
    for (int i = 0; i < rows; i++) {
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
            field[i][j] = '0'; // �������� ��������� ������
            opened[i][j] = false;
        }
    }
}

void place_mines(char** _field, int rows, int cols, int mines_count, int first_row, int first_col) {
    int mines_placed = 0;
    while (mines_placed < mines_count) {
        int x = rand() % rows;
        int y = rand() % cols;

        // �� ������ ���� � ������ ������ � � �������� ������
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
            field[i][j] = (count == 0) ? '.' : ('0' + count);
        }
    }
}

void print_field1(char** _field, bool** _opened, int rows, int cols, int moves) {
    Sleep(2000);
    system("cls");

    // ����� ������� �������� (���������� �����)
    cout << "    ";
    for (int j = 1; j <= cols; j++) {
        cout << setw(2) << j << " ";
    }
    cout << "\n   +";
    for (int i = 0; i < cols * 3 - 1; i++) {
        cout << "-";
    }
    cout << "+\n";

    // ����� ����� ����
    for (int i = 0; i < rows; i++) {
        cout << setw(2) << i + 1 << " |"; // ����� ������
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

    // ������ �������
    cout << "   +";
    for (int i = 0; i < cols * 3 - 1; i++) {
        cout << "-";
    }
    cout << "+\n";
}

// ��������� ������� �������� ��� ���������
void save_field_to_file(char** _field, int rows, int cols, const char* _filename) {
    FILE* file = fopen(_filename, "w");
    if (!file) {
        cout << "������ ������!" << endl;
        return;
    }

    // ���������� ������ ��������
    fprintf(file, "    ");
    for (int j = 1; j <= cols; j++) {
        fprintf(file, "%2d ", j);
    }
    fprintf(file, "\n");

    // ������� �������
    fprintf(file, "   +");
    for (int i = 0; i < cols * 3 - 1; i++) {
        fprintf(file, "-");
    }
    fprintf(file, "+\n");

    // ���������� ������ ����
    for (int i = 0; i < rows; i++) {
        fprintf(file, "%2d |", i + 1); // ����� ������
        for (int j = 0; j < cols; j++) {
            fprintf(file, " %c ", _field[i][j]);
        }
        fprintf(file, "|\n");
    }

    // ������ �������
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
        return true; // ����
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