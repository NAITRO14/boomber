#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <windows.h>
#include <iomanip>
#include <stdlib.h>
#include <string.h>
#include <string>
#include <sstream>
#include <chrono> // библиотека для работы со временем
#include <fstream>

using namespace std;
using namespace std::chrono;
enum ConsoleColor {
    Black, Blue, Green, Cyan, Red, Magenta, Brown, LightGray, DarkGray,
    LightBlue, LightGreen, LightCyan, LightRed, LightMagenta, Yellow, White
};

void SetColor(int text, int background) {
    HANDLE hStdOut = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleTextAttribute(hStdOut, (WORD)((background << 4) | text));
}

// Структура для хранения параметров уровня сложности
struct GameLevel {
    int rows;
    int cols;
    int mines_count;
    string name;
};

const GameLevel levels[] = {
    {10, 10, 10, "легкий"},    // Легкий уровень
    {15, 15, 23, "средний"},   // Средний уровень
    {20, 25, 50, "сложный"}    // Сложный уровень
};

const int LEVELS_COUNT = sizeof(levels) / sizeof(levels[0]);

void place_mines(char** _field, int rows, int cols, int mines_count, int first_row, int first_col);
void print_field1(char** _field, bool** _opened, int rows, int cols, int moves, steady_clock::time_point start_time);
void save_field_to_file(char** _field, int rows, int cols, const char* _filename);
void open_empty(char** _field, bool** _opened, int rows, int cols, int _row, int _col);
bool open(char** _field, bool** _opened, int rows, int cols, int _row, int _col);
bool inbounds(int row, int col, int rows, int cols);
void initialize_field(char** field, bool** opened, int rows, int cols);
void calculate_numbers(char** field, int rows, int cols);
void win_screen(int moves, int seconds_played, int record);
void lose_screen(char** field, bool** opened, int rows, int cols, int moves, int seconds_played, steady_clock::time_point start_time);//добавил параметр для подсчета времени
bool check_wins(bool** opened, int rows, int cols, int mines_count);
void registerUser();
bool loginUser(string username, string password);

int main() {
    srand(time(NULL));
    SetConsoleCP(CP_UTF8);
    SetConsoleOutputCP(CP_UTF8);


    //обявление переменных для игры  
    int choose, choose_menu, acces = 1;
    bool login_acces = false;
    short record;
    char res = 0;
    int rows;
    int cols;
    int mines_count;
    int moves;
    bool game_over;
    int first_row, first_col;
    bool normal_exit;

    do {
        if (res != 'q' || res != 'q')
        {
            // добавил авторизацию
            system("cls");
            cout << "\n==== Авторизация ====" << endl;
            cout << "1. Регистрация\n2. Вход\n3. Выход\n4. Играть без аккаунта\n";
            cout << "Выберите действие: ";
            cin >> choose;
            if (choose == 4) choose = 2; acces = 0;
            system("cls");
            switch (choose)
            {
            case 1:
            {
                registerUser();
                res = 'y';
            } break;
            case 2:
            {
                string username, password;
                if (acces == 1)
                {                  
                    cout << "\n=== Вход ===" << endl;
                    cout << "Логин: ";
                    cin >> username;
                    cout << "Пароль: ";
                    cin >> password;
                    login_acces = loginUser(username, password);
                }
                if (acces == 0) login_acces = true;
                if (login_acces == false) res = 'y';
                if (login_acces == true)
                {
                    system("cls");
                    while (true)
                    {
                        cout << "\n==== Главное меню ====" << endl;
                        cout << "Выберите действие:\n1. Начать игру";
                        if (acces == 1)
                        {
                            cout << "\n2. Посмотреть профиль";                           
                        }
                        cout << "\n3. Выход из программы\n";
                        if (acces == 1)
                        {
                            cout << "4.Выход из аккаунта\n";
                        }
                        cout << "\n  Ответ : ";

                        cin >> choose_menu;
                        if (acces == 0 and choose_menu == 2) continue;
                        if (acces == 0 and choose_menu == 4) continue;                       
                        system("cls");
                        switch (choose_menu)
                        {
                        case 1:
                        {
                            int choice;
                            do
                            {
                                cout << "Выберите уровень сложности:" << endl;
                                for (int i = 0; i < LEVELS_COUNT; i++)
                                {
                                    cout << i + 1 << ". " << levels[i].name
                                        << " (" << levels[i].rows << "x" << levels[i].cols
                                        << ", мин: " << levels[i].mines_count << ")" << endl;
                                }
                                cout << "Ваш выбор (1-" << LEVELS_COUNT << "): ";
                                cin >> choice;

                                if (choice <= 0 || choice > LEVELS_COUNT || cin.fail())
                                {
                                    system("cls");
                                    cin.clear();
                                    cin.ignore();
                                    cout << "Ошибка! Введите число от 1 до " << LEVELS_COUNT << endl;
                                    choice = 0;
                                    system("pause");
                                    system("cls");
                                }


                            } while (choice <= 0 || choice > LEVELS_COUNT || cin.fail());
                            const GameLevel& level = levels[choice - 1];
                            int rows = level.rows;
                            int cols = level.cols;
                            int mines_count = level.mines_count;
                            int moves = 0;
                            bool game_over = false;
                            int first_row = -1;
                            int first_col = -1;
                            bool normal_exit = false;//нормальное завершение (не досрочное)

                            char** field = new char* [rows];
                            bool** opened = new bool* [rows];
                            initialize_field(field, opened, rows, cols);

                            auto start_time = steady_clock::now();// запускаем таймер
                            while (!game_over) {
                                // Вычисляем прошедшее время
                                auto current_time = steady_clock::now();//текущий момент времени
                                auto elapsed_time = duration_cast<seconds>(current_time - start_time);//текущее время-время старта
                                int seconds_played = elapsed_time.count();

                                print_field1(field, opened, rows, cols, moves, start_time);//добавил параметр start_time  
                                cout << "Уровень: " << level.name << endl;
                                cout << "Ходы: " << moves << endl;
                                cout << "Время: " << seconds_played << " сек." << endl;//
                                cout << "Введите координаты (строка столбец) или 'q' для выхода: ";
                                string input;
                                getline(cin, input);

                                if (input[0] == 'q' || input[0] == 'Q')
                                {
                                    cout << "Выход из игры..." << endl;
                                    game_over = true;
                                    res = 'n';
                                    break; // Выходим без подсчета времени
                                }
                                stringstream ss(input);//положили в ss (input)
                                int r, c;
                                if (!(ss >> r >> c)) {
                                    cout << "Ошибка: введите два числа через пробел (например: 5 10)" << endl;
                                    continue;
                                }

                                // отнимаем 1 для работы
                                r--; c--;

                                // Проверяем границы
                                if (!inbounds(r, c, rows, cols)) {
                                    cout << "Ошибка: координаты вне диапазона (1-" << rows << " 1-" << cols << ")" << endl;
                                    continue;
                                }

                                if (opened[r][c]) {
                                    cout << "Эта клетка уже открыта!" << endl;
                                    Sleep(2000);
                                    continue;
                                }
                                // Первый ход - размещаем мины после него, избегая первой клетки
                                if (moves == 0) {
                                    first_row = r;
                                    first_col = c;
                                    place_mines(field, rows, cols, mines_count, first_row, first_col);
                                    calculate_numbers(field, rows, cols);
                                }
                                bool hit = open(field, opened, rows, cols, r, c);
                                moves++;

                                if (hit) {
                                    // Открываем все клетки при проигрыше
                                    for (int i = 0; i < rows; i++) {
                                        for (int j = 0; j < cols; j++) {
                                            if (field[i][j] == '*') opened[i][j] = true;
                                        }
                                    }

                                    // Вычисляем итоговое время если проиграли
                                    auto end_time = steady_clock::now();
                                    auto total_time = duration_cast<seconds>(end_time - start_time);

                                    print_field1(field, opened, rows, cols, moves, start_time);
                                    lose_screen(field, opened, rows, cols, moves, total_time.count(), start_time); // Передаём start_time

                                    game_over = true;
                                }
                                else if (check_wins(opened, rows, cols, mines_count))
                                {
                                    // Вычисляем итоговое время если победили
                                    auto end_time = steady_clock::now();
                                    auto total_time = duration_cast<seconds>(end_time - start_time);

                                    record = mines_count * 100 - total_time.count() * 5;

                                    print_field1(field, opened, rows, cols, moves, start_time);
                                    win_screen(moves, total_time.count(), record);

                                    normal_exit = true;
                                    game_over = true;

                                }

                            }

                            // Освобождение памяти
                            for (int i = 0; i < rows; i++) {
                                delete[] field[i];
                                delete[] opened[i];
                            }
                            delete[] field;
                            delete[] opened;

                            if (res != 'n' || res != 'N')
                            {
                                cout << "Хотите сыграть еще раз? (Если да нажмите - y / если нет - n): ";
                                cin >> res;
                                cin.ignore();
                                system("cls");
                            }
                        } break;
                        case 2:
                        {
                            int choose_profile = -1;
                            string score, fileUsername, filePassword;
                            ifstream login("users.txt");
                            if (!login.is_open())
                            {
                                cout << "Ошибка открытия файла для чтения!" << endl;
                                break;
                            }
                            // проверка юзернейма и пароля для того чтобы найти рекорд
                            while (login >> fileUsername >> filePassword >> score)
                            {
                                if (fileUsername == username and filePassword == password)
                                {
                                    break;
                                }
                            }
                            while (choose_profile != 0)
                            {
                                cout << "==== Профиль ====\n\n";
                                cout << "\tВаше имя: " << username << endl;
                                cout << "\tВаш рекорд: " << score << endl;
                                cout << "\tВаш пароль(введите 1 чтобы показать его): ";
                                for (int i = 0; i < password.length(); i++)
                                {
                                    cout << "*";
                                }
                                cout << "\n\tВведите 0 чтобы вернуться в главное меню\n";
                                cin >> choose_profile;
                                system("cls");
                                switch (choose_profile)
                                {
                                case 1:
                                {
                                    cout << "\tВаш пароль: " << password << endl;
                                    system("pause");
                                    system("cls");
                                } break;
                                case 0: break;
                                default: continue;
                                }
                            }
                            login.close();
                        } break;     
                        case 3: break;
                        case 4:
                        {
                            res = 'y';
                        } break;   
                        default: continue;
                        }
                        if (choose_menu == 4 or choose_menu == 3) break;
                    }
                }

            } break;
            case 3:
            {
                cout << "До свидания!" << endl;

            } break;                      
            default:
            {
                cout << "Неверный выбор. Попробуйте снова." << endl;
                system("pause");                 
                res = 'y';
                continue;
            } break;           
            }
            system("cls");
            if (choose == 3) break;                              
        }       
    } while (res == 'y' || res == 'Y');   
    cout << "Спасибо за игру!" << endl;
    return 0;
}

void initialize_field(char** field, bool** opened, int rows, int cols)
{
    for (int i = 0; i < rows; i++) {
        field[i] = new char[cols];
        opened[i] = new bool[cols];
        for (int j = 0; j < cols; j++) {
            field[i][j] = '0'; // Временно заполняем нулями
            opened[i][j] = false;
        }
    }
}

void place_mines(char** _field, int rows, int cols, int mines_count, int first_row, int first_col)
{
    int mines_placed = 0;
    while (mines_placed < mines_count)
    {
        int x = rand() % rows;
        int y = rand() % cols;

        // Не ставим мину в первую и в соседние клетки
        if ((x == first_row && y == first_col) || (abs(x - first_row) <= 1 && abs(y - first_col) <= 1))
        {
            continue;
        }

        if (_field[x][y] != '*')
        {
            _field[x][y] = '*';
            mines_placed++;
        }
    }
    save_field_to_file(_field, rows, cols, "MINES.txt");
}
// добавил в функцию  отображение времени
void print_field1(char** _field, bool** _opened, int rows, int cols, int moves, steady_clock::time_point start_time) {
    system("cls");

    // Вычисляем текущее время игры
    auto current_time = steady_clock::now();
    auto elapsed_time = duration_cast<seconds>(current_time - start_time);
    int seconds_played = elapsed_time.count();

    // Вывод номеров столбцов (двузначные числа)
    cout << "    ";
    for (int j = 1; j <= cols; j++)
    {
        cout << setw(2) << j << " ";
    }
    cout << "\n   +";
    for (int i = 0; i < cols * 3 - 1; i++)
    {
        cout << "-";
    }
    cout << "-+\n";

    // Вывод строк поля
    for (int i = 0; i < rows; i++) {
        cout << setw(2) << i + 1 << " |"; // Номер строки
        for (int j = 0; j < cols; j++) {
            if (_opened[i][j]) {

                if (_field[i][j] == '*') {
                    SetColor(4, 0);
                    cout << " * ";
                    SetColor(7, 0);
                }
                else if (_field[i][j] == '0') {
                    SetColor(2, 0);
                    cout << " V ";
                    SetColor(7, 0);
                }
                else
                {
                    // Разные цвета для разных цифр
                    switch (_field[i][j]) {
                    case '1': SetColor(3, 0); break;
                    case '2': SetColor(9, 0); break;
                    case '3': SetColor(1, 0); break;
                    case '4': SetColor(10, 0); break;
                    case '5': SetColor(11, 0); break;
                    case '6': SetColor(6, 0); break;
                    case '7': SetColor(12, 0); break;
                    case '8': SetColor(8, 0); break;
                    }

                    cout << " " << _field[i][j] << " ";
                    SetColor(7, 0);
                }
            }
            else
            {
                SetColor(14, 0);
                cout << " W ";
                SetColor(7, 0);
            }
        }
        cout << "|\n";
    }

    // Нижняя граница
    cout << "   +";
    for (int i = 0; i < cols * 3 - 1; i++) {
        cout << "-";
    }
    cout << "-+\n";

    // Вывод времени и ходов
    cout << "Время: " << seconds_played << " сек. | Ходы: " << moves << endl;
}

void calculate_numbers(char** field, int rows, int cols)
{
    int dx[] = { -1,-1,-1, 0, 0, 1, 1, 1 };
    int dy[] = { -1, 0, 1,-1, 1,-1, 0, 1 };

    for (int i = 0; i < rows; i++)
    {
        for (int j = 0; j < cols; j++)
        {
            if (field[i][j] == '*') continue;

            int count = 0;
            for (int k = 0; k < 8; k++)
            {
                int ni = i + dx[k], nj = j + dy[k];
                if (inbounds(ni, nj, rows, cols) && field[ni][nj] == '*')
                {
                    count++;
                }
            }
            field[i][j] = '0' + count;
        }
    }
}

void win_screen(int moves, int seconds_played, int record) {
    system("cls");
    cout << "*******************************" << endl;
    cout << " Поздравляем! Вы выиграли!     " << endl;
    cout << " Количество ходов: " << moves << setw(5) << endl;
    cout << " Затраченное время: " << seconds_played << " сек." << endl;// добавил строку время
    cout << " Ваш результат: " << record << " очков!" << endl; // добавлен результат (только при победе)
    cout << "*******************************" << endl;
}
//доработал эту функцию для того, чтобы при проигрыше показывалось поле с минами с задержкой
void lose_screen(char** field, bool** opened, int rows, int cols, int moves, int seconds_played, steady_clock::time_point start_time) {
    system("cls");
    // Показываем поле с минами с актуальным временем
    print_field1(field, opened, rows, cols, moves, start_time);
    cout << "\nВы наступили на мину! Игра окончена.\n";
    Sleep(5000); // Задержка 5 секунд
    // Затем показываем экран поражения
    system("cls");
    cout << "*******************************" << endl;
    cout << " К сожалению, вы наступили на мину! " << endl;
    cout << " Игра окончена! Вы проиграли! " << endl;
    cout << " Количество ходов: " << moves << setw(5) << endl;
    cout << " Затраченное время: " << seconds_played << " сек." << endl;
    cout << "*******************************" << endl;
}

void save_field_to_file(char** _field, int rows, int cols, const char* _filename)
{
    FILE* file = fopen(_filename, "w");
    if (!file)
    {
        cout << "Ошибка записи!" << endl;
        return;
    }

    // Записываем номера столбцов
    fprintf(file, "    ");
    for (int j = 1; j <= cols; j++)
    {
        fprintf(file, "%2d ", j);
    }
    fprintf(file, "\n");

    // Верхняя граница
    fprintf(file, "   +");
    for (int i = 0; i < cols * 3 - 1; i++)
    {
        fprintf(file, "-");
    }
    fprintf(file, "+\n");

    // Записываем строки поля
    for (int i = 0; i < rows; i++)
    {
        fprintf(file, "%2d |", i + 1); // Номер строки
        for (int j = 0; j < cols; j++)
        {
            fprintf(file, " %c ", _field[i][j]);
        }
        fprintf(file, "|\n");
    }

    // Нижняя граница
    fprintf(file, "   +");
    for (int i = 0; i < cols * 3 - 1; i++)
    {
        fprintf(file, "-");
    }
    fprintf(file, "+\n");

    fclose(file);
}
void open_empty(char** _field, bool** _opened, int rows, int cols, int _row, int _col)
{
    if (!inbounds(_row, _col, rows, cols)) return;
    if (_opened[_row][_col]) return;

    _opened[_row][_col] = true;
    if (_field[_row][_col] != '0') return;

    int dx[] = { -1, -1, -1, 0, 0, 1, 1, 1 };
    int dy[] = { -1, 0, 1, -1, 1, -1, 0, 1 };

    for (int k = 0; k < 8; k++)
    {
        open_empty(_field, _opened, rows, cols, _row + dx[k], _col + dy[k]);
    }
}

bool open(char** _field, bool** _opened, int rows, int cols, int _row, int _col)
{
    if (_field[_row][_col] == '*')
    {
        _opened[_row][_col] = true;
        return true; // мина
    }
    else if (_field[_row][_col] == '0')
    {
        open_empty(_field, _opened, rows, cols, _row, _col);
    }
    else
    {
        _opened[_row][_col] = true;
    }
    return false;
}

bool inbounds(int row, int col, int rows, int cols)
{
    return row >= 0 && row < rows && col >= 0 && col < cols;
}

bool check_wins(bool** opened, int rows, int cols, int mines_count)
{
    int opened_cells = 0;
    for (int i = 0; i < rows; i++)
    {
        for (int j = 0; j < cols; j++)
        {
            if (opened[i][j]) opened_cells++;
        }
    }
    return opened_cells == rows * cols - mines_count;
}

void registerUser()
{  

    string username, password, fileUsername;
    while (true)
    {
        system("cls");
        cout << "\n=== Регистрация ===" << endl;
        do
        {
            cout << "Придумайте логин (не менее 3 символов): ";
            cin >> username;
        } while (username.length() < 3);
        do
        {
            cout << "Придумайте пароль (не менее 4 символов): ";
            cin >> password;
        } while (password.length() < 4);

        // проверяю на наличие логина в файле
        // открыл файл в режиме чтения    
        ifstream check("users.txt");
        if (check.is_open())
        {
            while (check >> fileUsername)
            {
                if (fileUsername == username)
                {
                    cout << "Пользователь с таким логином уже есть\n\n";
                    system("pause");
                    check.close();
                    break;
                }
            }
            if (fileUsername != username) break;

        }
        else break;
    }
    // создаю файл с режимом app (альтернатива режиму "a" из того что мы изучали, то есть - добавление в файл информации)       
    ofstream reg("users.txt", ios::app);
    if (!reg.is_open())
    {
        cout << "Ошибка открытия файла для записи" << endl;
        return;
    }
    // запись в файл логин, пароль и рекорд (пока рандомный)
    reg << username << " " << password << " " << rand() % 100 << endl;
    reg.close();

    cout << "Регистрация прошла успешно!" << endl;
    system("pause");
}

bool loginUser(string username, string password)
{
    string fileUsername, filePassword, score, file_score;
    bool loginSuccess = false;
    
    // открыл файл в режиме чтения
    ifstream login("users.txt");
    if (!login.is_open())
    {
        cout << "Ошибка открытия файла для чтения!" << endl;
        return false;
    }
    // Оператор >> читает данные из файла до первого пробела или перевода строки (мне так сказал дипсик и сказал что это будет удобно, так как если в файле будут храниться одинаковые
    // пароли, то программа не выдаст сбои и отработает нормально)
   
    while (login >> fileUsername >> filePassword >> file_score)
    {
        if (fileUsername == username and filePassword == password) {
            loginSuccess = true;
            score = file_score;
            break;
        }
    }
    login.close();

    if (loginSuccess == true)
    {
        cout << "Вход выполнен успешно! Добро пожаловать, " << username << "! " << "Ваш рекорд: " << score << endl << endl;
        system("pause");
        return true;
    }
    else
    {
        cout << "Ошибка: неверный логин или пароль!" << endl << endl;
        system("pause");
        return false;
    }
}
