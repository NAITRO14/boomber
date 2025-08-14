#include <iostream>
#include <fstream>
#include <Windows.h>
#include <iomanip>
#include <stdlib.h>
#include <stdio.h>

using namespace std;
/*
���������� � ���������:


�������� ����������� ����������� � �������

���������� ����� ������������� �� ������� ����

��������� ��� �� ������������ � ����� ������

�������� ����������� ����� � ����� ������� ������������

���������� � ������ ������������ ��������� ���-�� �����(��� �����)



���������� � �������� �����������:


��� ������������ �� ����� 3� ��������

������ ������������ �� ����� 4� ��������(����� � �����)*/


// ������� � ������� ��� ����� ������������ ���������� ������ ������ �� �����, � �� �������������� ��� ������� ������ ����� �������� ������ � �++ (ofstream / ifstream)
// ��������� 15 ����� �� �������� ����� ������( ������ �� ���������)


void registerUser()
{  
    // ������ �� �������� ���� ���� ������� ����� ������( � ������ ��, ��� ������� �������� ������ � ������)
   
    string username, password, fileUsername;    
    while (true)
    {
        system("cls");
        cout << "\n=== ����������� ===" << endl;
        do
        {
            cout << "���������� ����� (�� ����� 3 ��������): ";
            cin >> username;
        } while (username.length() < 3);
        do
        {
            cout << "���������� ������ (�� ����� 4 ��������): ";
            cin >> password;
        } while (password.length() < 4);

        // �������� �� ������� ������ � �����
        // ������ ���� � ������ ������    
        ifstream check("users.txt");
        if (check.is_open())
        {
            while (check >> fileUsername)
            {
                if (fileUsername == username)
                {
                    cout << "������������ � ����� ������� ��� ����\n\n";
                    system("pause");
                    check.close();
                    break;
                }
            }
            if (fileUsername != username) break;

        }
        else break;
    }
    // ������ ���� � ������� app (������������ ������ "a" �� ���� ��� �� �������, �� ���� - ���������� � ���� ����������)       
    ofstream reg("users.txt", ios::app);
    if (!reg.is_open())
    {
        cout << "������ �������� ����� ��� ������" << endl;
        return;
    }
    // ������ � ���� �����, ������ � ������ (���� ���������)
    reg << username << " " << password << " " << rand() % 100 << endl;
    reg.close();

    cout << "����������� ������ �������!" << endl;
    system("pause");
}
bool loginUser()
{
    string username, password, fileUsername, filePassword, score, file_score;
    bool loginSuccess = false;

    cout << "\n=== ���� ===" << endl;
    cout << "�����: ";
    cin >> username;
    cout << "������: ";
    cin >> password;  
    // ������ ���� � ������ ������
    ifstream login("users.txt");
    if (!login.is_open())
    {
        cout << "������ �������� ����� ��� ������!" << endl;
        return false;
    }  
    // �������� >> ������ ������ �� ����� �� ������� ������� ��� �������� ������ (��� ��� ������ ������ � ������ ��� ��� ����� ������, ��� ��� ���� � ����� ����� ��������� ����������
    // ������, �� ��������� �� ������ ���� � ���������� ���������)

    // ����� � ��������� ����� while ��� ���� ��������� ������ (� ���������� � ��� ��� ��� ��������)
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
        cout << "���� �������� �������! ����� ����������, " << username << "! " << "��� ������: " << score << endl << endl;
        system("pause");
        return true;
    }
    else
    {
        cout << "������: �������� ����� ��� ������!" << endl << endl;
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
        cout << "\n=== ������� ���� ===" << endl;
        cout << "1. �����������\n2. ����\n3. �����\n";       
        cout << "�������� ��������: ";     
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
                cout << "�� ����� � �������!" << endl;
            }           

        } break;
        case 3:
        {
            cout << "�� ��������!" << endl;
            
        } break;
        default:
        {
            cout << "�������� �����. ���������� �����." << endl;
            system("pause");
        }
        }
        if (choose == 3) break;
    }
}
