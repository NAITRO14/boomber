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
			// ������ �����������,���������� �� �������� ���������� ����� ���������
			sign_in(&login[20], &password[25]);
			FILE* sign_in = fopen("accounts.txt", "a");
			if (!sign_in) cout << "�������� ������ ��� �������� �����." << endl;
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
				cout << "�������� ��������:\n1. ���� � �������.\n2. ����������� ��������.\n";
				cin >> choose;
				system("cls");
			}
			if (choose == 1)
			{
				log_in(login, password);
				FILE* accounts = fopen("accounts.txt", "r");
				if (!accounts) cout << "�������� ������ ��� �������� �����." << endl;
				else
				{
					cout << "file opened" << endl;
					// �� ������ �� ��� ����������� � ��������� ���������
					if (fscanf(accounts, "%s", login) == 1 and fscanf(accounts, "%s", password) == 1)
					{
						cout << "�� ������� ����� � ���� �������!\n";					
						system("pause");
					}
					if (fscanf(accounts, "%s", login) == 0)
					{
						while (choose2 != 1 and choose2 != 2)
						{
							cout << "������ ������������ ���. ������������������?\n1. ��\n2. ���\n";
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
									cout << "����� ������������ ��� ����.\n";
									system("pause");
									system("cls");
								}
								else cout << "�������!\n";
							}
						}
					}
					// �� ������ �� ��� ����������� � ������ ���������
					if (fscanf(accounts, "%s", login) == 1 and fscanf(accounts,  password) == 0)
					{
						cout << "�������� ������ � ������ �� ����������. \n";
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
						cout << "����� ������������ ��� ����.\n";
						system("pause");
						system("cls");
					}
					else cout << "�������!\n";
				}
			}

		}
		if (choose != 1)
		{
			FILE* result = fopen("accounts.txt", "a");
			if (!result) cout << "�������� ������ ��� �������� �����." << endl;
			else
			{
				fprintf(result, login);
				fprintf(result, " ");
				fprintf(result, password);				
				fprintf(result, "\n");
				fclose(result);
			}			
			cout << "����������?\n1. ��\n0. ���\n";
			cin >> choose_global;
			system("cls");
		}
	}
}
void log_in(char _login[], char _password[])
{
	cout << "���� � ������� ������.\n\n";
	cout << "������� �����:  ";
	scanf("%s", _login);
	cout << "������� ������:  ";
	scanf("%s", _password);
	system("cls");

}
void sign_in(char _login[], char _password[])
{
	cout << "����������� ������������.\n\n";
	cout << "������� �����(�� ����� 3 ��������):  ";
	scanf("%s", _login);
	cout << "���������� ������(�� ����� 4 ��������):  ";
	scanf("%s", _password);		
	
	system("cls");
}

