#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <Windows.h>
#include <iomanip>
#include <stdlib.h>
#include <stdio.h>
using namespace std;
void log_in(char _login[], char _password[]);
void sign_in(char _login[], char _password[]);
int main()
{
	SetConsoleCP(1251);
	SetConsoleOutputCP(1251);
	srand(time(NULL));
	short choose_global = -1;
	while (choose_global != 0)
	{
		system("cls");
		short choose = 0, choose2 = 0;
		SetConsoleCP(1251);
		SetConsoleOutputCP(1251);
		char login[20], password[25];
		FILE* accounts = fopen("accounts.txt", "r");
		if (!accounts)
		{
			// первая регистрация,работающая по принципу отсутствия файла аккаунтов
			sign_in(&login[20], &password[25]);
			FILE* sign_in = fopen("accounts.txt", "a");
			if (!sign_in) cout << "Произшла ошибка при открытии файла." << endl;
			else
			{				
				fprintf(sign_in, login);
				fprintf(sign_in, " ");
				fprintf(sign_in, password);								
				fprintf(sign_in, "\n");
				fclose(sign_in);
			}
		}
		else
		{			
			while (choose != 1 and choose != 2)
			{
				cout << "Выберите действие:\n1. Вход в аккаунт.\n2. Регистрация аккаунта.\n";
				cin >> choose;
				system("cls");
			}
			if (choose == 1)
			{
				log_in(login, password);
				FILE* accounts = fopen("accounts.txt", "r");
				if (!accounts) cout << "Произшла ошибка при открытии файла." << endl;
				else
				{
					cout << "file opened" << endl;
					// хз почему но это дублируется с последний вариантом
					if (fscanf(accounts, "%s", login) == 1 and fscanf(accounts, "%s", password) == 1)
					{
						cout << "Вы успешно вошли в свой аккаунт!\n";					
						system("pause");
					}
					if (fscanf(accounts, "%s", login) == 0)
					{
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
								sign_in(login, password);
								if (fscanf(accounts, "%s", login) == 1)
								{
									cout << "Такой пользователь уже есть.\n";
									system("pause");
									system("cls");
								}
								else cout << "Успешно!\n";
							}
						}
					}
					// хз почему но это дублируется с первым вариантом
					if (fscanf(accounts, "%s", login) == 1 and fscanf(accounts,  password) == 0)
					{
						cout << "Введённый пароль к логину не правильный. \n";
						system("pause");
						system("cls");
					}
					system("cls");

				}
			}
			else
			{
				bool acces = 0;
				while (acces != 1)
				{
					sign_in(login, password);
					if (fscanf(accounts, "%s", login) == 1)
					{
						cout << "Такой пользователь уже есть.\n";
						system("pause");
						system("cls");
					}
					else cout << "Успешно!\n";
				}
			}

		}
		if (choose != 1)
		{
			FILE* result = fopen("accounts.txt", "a");
			if (!result) cout << "Произшла ошибка при открытии файла." << endl;
			else
			{
				fprintf(result, login);
				fprintf(result, " ");
				fprintf(result, password);				
				fprintf(result, "\n");
				fclose(result);
			}			
			cout << "Продолжить?\n1. Да\n0. Нет\n";
			cin >> choose_global;
			system("cls");
		}
	}
}
void log_in(char _login[], char _password[])
{
	cout << "Вход в учётную запись.\n\n";
	cout << "Введите логин:  ";
	scanf("%s", _login);
	cout << "Введите пароль:  ";
	scanf("%s", _password);
	system("cls");

}
void sign_in(char _login[], char _password[])
{
	cout << "Регистрация пользователя.\n\n";
	cout << "Введите логин(не менее 3 символов):  ";
	scanf("%s", _login);
	cout << "Придумайте пароль(не менее 4 символов):  ";
	scanf("%s", _password);		
	
	system("cls");
}

