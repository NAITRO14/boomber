#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <Windows.h>
#include <iomanip>
#include <stdlib.h>
#include <stdio.h>
using namespace std;

// это регистрация пользователя в игру. она работает по принципу создания файла на каждого пользователя.
// каждый пользователь имеет свой файл и данные в нем: название файла(по сути логин) и пароль.

int main()
{
	SetConsoleCP(1251);
	SetConsoleOutputCP(1251);
	srand(time(NULL));
	short choose_global = -1;
	short score;
	while (choose_global != 0)
	{
		system("cls");
		short choose = 0, choose2 = 0, score = 0;			
		SetConsoleCP(1251);
		SetConsoleOutputCP(1251);
		char login[20], password[25];
		while (choose != 1 and choose != 2)
		{
			// просто вопрос к пользователю
			cout << "Выберите действие:\n1. Вход в аккаунт.\n2. Регистрация аккаунта.\n";
			cin >> choose;
			system("cls");
		}
		if (choose == 1)
		{
			// как я понял, зеленые полоски на scanf() не страшно, главное что работает
			cout << "Вход в учётную запись.\n\n";
			cout << "Введите логин:  ";
			scanf("%s", login);
			cout << "Введите пароль:  ";
			scanf("%s", password);
			system("cls");
			FILE* accounts = fopen(strcat(login, ".txt"), "r");
			if (!accounts)
			{
				// доп отдел для регистрации, если при входе введенный логин отсутсвовал в файлах
				while (choose2 != 1 and choose2 != 2)
				{
					cout << "Такого пользователя нет. Зарегестрироваться?\n1. Да\n2. Нет\n";
					cin >> choose2;
					system("cls");
				}
				if (choose2 == 1)
				{
					bool acces = 0;
					while (acces != 1)
					{
						cout << "Регистрация пользователя.\n\n";
						cout << "Введите логин(не менее 3 символов):  ";
						scanf("%s", login);
						cout << "Придумайте пароль(не менее 4 символов):  ";
						scanf("%s", password);
						system("cls");
						FILE* accounts = fopen(strcat(login, ".txt"), "r");
						if (!accounts)
						{
							cout << "Успешно!\n"; acces = 1;
						}
						else
						{
							cout << "Такой пользователь уже есть.\n";
							system("pause");
							system("cls");
							continue;
						}
					}
				}
			}
			else
			{
				if (fscanf(accounts, "%s", password) == 1)
				{
					cout << "Вы успешно вошли в свой аккаунт!\n";
					system("pause");
				}
				if (fscanf(accounts, "%s", password) == 0)
				{
					cout << "Введённый пароль к логину не правильный. \n";
					system("pause");
				}
				system("cls");
				fclose(accounts);				
			}
		}
		else
		{
			bool acces = 0;
			while (acces != 1)
			{
				cout << "Регистрация пользователя.\n\n";
				cout << "Введите логин(не менее 3 символов):  ";
				scanf("%s", login);
				cout << "Придумайте пароль(не менее 4 символов):  ";
				scanf("%s", password);							
				system("cls");
				FILE* accounts = fopen(strcat(login, ".txt"), "r");
				if (!accounts)
				{
					cout << "Успешно!\n"; acces = 1;					
				}
				else
				{
					cout << "Такой пользователь уже есть.\n";
					system("pause");
					system("cls");
					continue;
				}				
			}						
		}
		// очки, записываются в файл просто пока-что по приколу
		score = rand()%100;
		FILE* result = fopen(login, "w");
		if (!result) cout << "Произшла ошибка при открытии файла." << endl;
		else
		{
			fprintf(result, login);
			fprintf(result, " ");
			fprintf(result, password);
			fprintf(result, " ");
			fprintf(result, "%d", score);
			fprintf(result, "\n");
			fclose(result);
		}
		cout << "Продолжить?\n1. Да\n0. Нет\n";
		cin >> choose_global;
		system("cls");
	}

}
