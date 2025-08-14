#include <iostream>
#include <fstream>
#include <Windows.h>
#include <iomanip>
#include <stdlib.h>
#include <stdio.h>

using namespace std;
/*
Требования к программе:


Добавить возможность регистрации с паролем

Записывать новых пользователей во внешний файл

Проверять нет ли пользователя с таким именем

Добавить возможность зайти в такой аккаунт пользователя

Записывать в нового пользователя случайное кол-во очков(для теста)



Требования к процессу регистрации:


Имя пользователя не менее 3х символов

Пароль пользователя не менее 4х символов(буквы и цифры)*/


// спросил у дипсика как можно организовать правильное чтение данных из файла, и он порекомендовал мне немного другой метод созданию файлов в с++ (ofstream / ifstream)
// потрачено 15 минут на изучение этого метода( изучал не полностью)


void registerUser()
{  
    // работа со стрингом тоже была изучена через дипсик( а именно то, как сделать проверку пароля и логина)
   
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
bool loginUser()
{
    string username, password, fileUsername, filePassword, score, file_score;
    bool loginSuccess = false;

    cout << "\n=== Вход ===" << endl;
    cout << "Логин: ";
    cin >> username;
    cout << "Пароль: ";
    cin >> password;  
    // открыл файл в режиме чтения
    ifstream login("users.txt");
    if (!login.is_open())
    {
        cout << "Ошибка открытия файла для чтения!" << endl;
        return false;
    }  
    // Оператор >> читает данные из файла до первого пробела или перевода строки (мне так сказал дипсик и сказал что это будет удобно, так как если в файле будут храниться одинаковые
    // пароли, то программа не выдаст сбои и отработает нормально)

    // метод с проверкой через while мне тоже подсказал дипсик (я разобрался в том как это работает)
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
int main()
{
    SetConsoleCP(1251);
    SetConsoleOutputCP(1251);
    srand(time(NULL));
    int choose;
    bool acces;
    while (true)
    {
        system("cls");
        cout << "\n=== Главное меню ===" << endl;
        cout << "1. Регистрация\n2. Вход\n3. Выход\n";       
        cout << "Выберите действие: ";     
        cin >> choose;
        system("cls");
        switch (choose)
        {
        case 1:
        {
            registerUser();            
        } break;
        case 2:
        {
            acces = loginUser();
            if (acces = true)
            {
                cout << "Вы вошли в систему!" << endl;
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
        }
        }
        if (choose == 3) break;
    }
}
