#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <Windows.h>
#include <iomanip>
#include <stdlib.h>
#include <stdio.h>
using namespace std;

// ��� ����������� ������������ � ����. ��� �������� �� �������� �������� ����� �� ������� ������������.
// ������ ������������ ����� ���� ���� � ������ � ���: �������� �����(�� ���� �����) � ������.

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
			// ������ ������ � ������������
			cout << "�������� ��������:\n1. ���� � �������.\n2. ����������� ��������.\n";
			cin >> choose;
			system("cls");
		}
		if (choose == 1)
		{
			// ��� � �����, ������� ������� �� scanf() �� �������, ������� ��� ��������
			cout << "���� � ������� ������.\n\n";
			cout << "������� �����:  ";
			scanf("%s", login);
			cout << "������� ������:  ";
			scanf("%s", password);
			system("cls");
			FILE* accounts = fopen(strcat(login, ".txt"), "r");
			if (!accounts)
			{
				// ��� ����� ��� �����������, ���� ��� ����� ��������� ����� ����������� � ������
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
						cout << "����������� ������������.\n\n";
						cout << "������� �����(�� ����� 3 ��������):  ";
						scanf("%s", login);
						cout << "���������� ������(�� ����� 4 ��������):  ";
						scanf("%s", password);
						system("cls");
						FILE* accounts = fopen(strcat(login, ".txt"), "r");
						if (!accounts)
						{
							cout << "�������!\n"; acces = 1;
						}
						else
						{
							cout << "����� ������������ ��� ����.\n";
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
					cout << "�� ������� ����� � ���� �������!\n";
					system("pause");
				}
				if (fscanf(accounts, "%s", password) == 0)
				{
					cout << "�������� ������ � ������ �� ����������. \n";
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
				cout << "����������� ������������.\n\n";
				cout << "������� �����(�� ����� 3 ��������):  ";
				scanf("%s", login);
				cout << "���������� ������(�� ����� 4 ��������):  ";
				scanf("%s", password);							
				system("cls");
				FILE* accounts = fopen(strcat(login, ".txt"), "r");
				if (!accounts)
				{
					cout << "�������!\n"; acces = 1;					
				}
				else
				{
					cout << "����� ������������ ��� ����.\n";
					system("pause");
					system("cls");
					continue;
				}				
			}						
		}
		// ����, ������������ � ���� ������ ����-��� �� �������
		score = rand()%100;
		FILE* result = fopen(login, "w");
		if (!result) cout << "�������� ������ ��� �������� �����." << endl;
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
		cout << "����������?\n1. ��\n0. ���\n";
		cin >> choose_global;
		system("cls");
	}

}
