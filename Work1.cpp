#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <windows.h>
#include <iomanip>
#include <stdlib.h>
#include <string.h>
using namespace std;

const int rows = 5;//размер поля 5х5
const int cols = 5;
const int mines_count = 10; // мин по условию должно быть 10

void print_field1(char _field[rows][cols]);//функция для вывода поля на экран
void place_mines(char field[rows][cols]);//функция для размещение на поле мин
void save_field_to_file(char field[rows][cols], const char* filename);// функция для сохранения заполненного поля в файл


int main()
{
    srand(time(NULL));
    SetConsoleCP(1251);
    SetConsoleOutputCP(1251);

    // Инициализация игрового поля
    char field[rows][cols] = {};//двумерный массив минное поле
    for (short i = 0; i < rows; i++)
    {
        for (short j = 0; j < cols; j++)
        {
            field[i][j] = ' ';
        }
    }
    // Размещение мин
    place_mines(field);//так как по условию поле маленькое, а мин много допускаем расположение мин в соседних клеточках

    // Вывод поля с минами
    print_field1(field);
    save_field_to_file(field, "MINES.txt");



    system("pause");
    return 0;
}
// Функция для размещения мин на поле
void place_mines(char field[rows][cols])
{
    int mines_placed = 0;//переменная для подсчета мин

    while (mines_placed < mines_count)
    {
        int x = rand() % rows; // Случайная строка
        int y = rand() % cols; // Случайный столбец

        // Проверяем, не находится ли уже в данной ячейке  мина
        if (field[x][y] != '*')
        {
            field[x][y] = '*'; // Размещаем мину
            mines_placed++; // Увеличиваем счетчик размещенных мин

        }
    }
}
// Функция вывода поля 
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
        }        cout << "|\n";
    }
    cout << "  +";
    for (short i = 0; i < cols * 2 - 1; i++)
    {
        cout << "-";
    }
    cout << "+" << endl;
}
//функция для сохранения заполненного поля в файл
void save_field_to_file(char field[rows][cols], const char* filename)
{
    FILE* file = fopen("MINES.txt", "w"); // Открываем файл для записи

    if (!file) // Проверяем, успешно ли открылся файл
    {
        cout << "Ошибка открытия файла для записи!" << endl;
        return;
    }
    else {

        // Записываем номера столбцов
        fprintf(file, "   ");
        for (short j = 0; j < cols; j++)
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
            fprintf(file, "%d |", i); // Номер строки
            for (short j = 0; j < cols; j++)
            {
                fprintf(file, "%c", field[i][j]); // Символ поля
                if (j < cols - 1)
                {
                    fprintf(file, " "); // Пробел между символами
                }
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
    fclose(file); // Закрываем файл
    cout << "Игровое поле успешно сохранено в файл: " << "MINES.txt" << endl;
}
