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
const int max_moves = 25;
const int max_lives = 3;  // Добавил константу для максимального количества жизней

// Добавим константы для системы очков
const int MINE_PENALTY = -5;       // Штраф за мину
const int BASE_POINTS = 5;         // Базовые очки
const int POINTS_INCREMENT = 5;    // Шаг увеличения очков
const int POINTS_GROUP_SIZE = 5;   // Количество полей в группе


void place_mines(char _field[rows][cols]);//размещение мин
void print_field1(char _field[rows][cols], bool _opened[rows][cols]);//вывод на экран
void save_field_to_file(char _field[rows][cols], const char* _filename);//сохранение в файл

void open_empty(char _field[rows][cols], bool _opened[rows][cols], int _row, int _col);
// true если мина, false если безопасно
bool open(char _field[rows][cols], bool _opened[rows][cols], int _row, int _col);
bool inbounds(int row, int col);


int main()
{
    srand(time(NULL));
    /* SetConsoleCP(CP_UTF8);
     SetConsoleOutputCP(CP_UTF8);*///с этим у меня выводит ????????????????

    SetConsoleCP(1251);
    SetConsoleOutputCP(1251);

    //setlocale(LC_ALL, "ru");

    char field[rows][cols];//массив поле
    bool opened[rows][cols] = { false };// 

    // Инициализация массива
    for (short i = 0; i < rows; i++)
    {
        for (short j = 0; j < cols; j++)
        {
            field[i][j] = '0';
        }
    }
    place_mines(field);//вызов функции размещение мин

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
                if (inbounds(num1, num2) && field[num1][num2] == '*')//если мина счетчик +
                {
                    count++;
                }
            }
            field[i][j] = count + '0';// записывает кол во мин в пустую клетку
        }
    }
    int moves = 0;
    bool game_over = false;
    int lives = max_lives;  // Добавил счетчик жизней

    int score = 0;                      // Добавили счетчик очков
    int opened_safe_cells = 0;          // Счетчик открытых безопасных клеток


    string input;
    int r, c;
    bool hit;
    while (moves < max_moves && !game_over)
    {
        print_field1(field, opened);//вызов функции вывод на экран
        cout << "Введите размеры(стобец и строку)" << endl;//строка-столбец
        cout << "Ввод: ";
        getline(cin, input);//функция чтения строки
        system("cls");
        // проверка на ввод, чтоб длина строки была не больше 2 символов и вводилось строка столбец
        if (input.length() != 2 || !isdigit(input[0]) || !isdigit(input[1]))
        {
            cout << "Ошибка: введите 2 цифры (например, 12 для строки 1 столбца 2)" << endl;//добавил
            continue;
        }
        r = input[0] - 1 - '0';
        c = input[1] - 1 - '0';

        if (r < 0 || r >= rows || c < 0 || c >= cols)
        {
            cout << "Ошибка: координаты вне диапазона (1-" << rows << " 1-" << cols << ")" << endl;//добавил
            continue;
        }

        // Выводим статистику игры
        cout << "Жизни: " << lives << "/" << max_lives
            << " | Ходы: " << moves << "/" << max_moves
            << " | Очки: " << score << endl;


        if (opened[r][c])
        {
            cout << "Эта клетка уже открыта!" << endl;//добавил
            continue;
        }
        //если координаты ввели неправильно
        if (input.length() != 2)
        {
            cout << "Введите ровно 2 цифры!";
        }


        hit = open(field, opened, r, c);//инициализируем hit как функцию открытие клетки
        moves++;//складываем количество ходов


        if (hit)
        {
            // Попали на мину
            score += MINE_PENALTY;  // Штрафуем за мину


            lives--;  // Уменьшаем количество жизней при попадании на мину

            if (lives > 0)
            {
                // Если еще есть жизни, сообщаем об этом
                cout << "Вы нашли мину!-5 очков! Осталось жизней: " << lives << endl;
                // Показываем только текущую мину
                opened[r][c] = true;
                print_field1(field, opened);
                cout << "Продолжаем игру..." << endl;
                Sleep(5000);  // Даем время прочитать сообщение
                system("cls");
            }
            else
            {
                // Если жизни закончились, открываем все поле
                for (int i = 0; i < rows; ++i)
                {
                    for (int j = 0; j < cols; ++j)
                    {
                        opened[i][j] = true;
                    }
                }
                print_field1(field, opened);
                cout << "Игра окончена! Вы исчерпали все жизни." << endl;
                cout << "Ваш итоговый счет:  " << score << "  очков  " << endl;
                game_over = true;
            }
        }
        else//если открыли безопасную клетку
        {
            opened_safe_cells++;
            // Вычисляем сколько очков добавляем
            int points_group = (opened_safe_cells - 1) / POINTS_GROUP_SIZE;
            int points_to_add = BASE_POINTS + (points_group * POINTS_INCREMENT);
            score += points_to_add;

            cout << "Вы открыли безопасную клетку! +" << points_to_add << " очков" << endl;
            Sleep(5000);//для задержки сообщения на экране
            system("cls");
        }
    }
    // Проверяем условия завершения игры
    if (!game_over && lives > 0) {
        if (moves >= max_moves) {
            cout << "Поздравляем! Вы сделали " << max_moves << " ходов и выиграли!" << endl;
        }
        else {
            cout << "Игра завершена." << endl;
        }
        cout << "Ваш итоговый счет:  " << score << " очков " << endl;
    }

    return 0;
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
    int ch = 0;
    cout << "   ";
    for (int j = 1; j < cols + 1; j++)
    {
        cout << j << " ";   // Верхняя строка с номерами столбцов

    }
    cout << "\n  +";
    for (int i = 0; i < cols * 2 - 1; i++)
    {
        cout << "-"; // Верхняя рамка

    }
    cout << "+\n";//+ в конце ---------+
    ////вывод строк поля
    for (int i = 0; i < rows; i++)
    {
        cout << i + 1 << " |";
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
                    cout << '.';
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
        for (short j = 1; j < cols + 1; j++)
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
            fprintf(file, "%d |", i + 1); // Номер строки
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
    cout << "Игровое поле успешно сохранено в файл: MINES.txt" << endl;
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