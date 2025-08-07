#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <windows.h>
#include <iomanip>
#include <stdlib.h>
#include <string.h>
#include <string>

//графика
#include <FL/Fl.H>
#include <FL/Fl_Double_Window.H> // использование двойной буферизации
#include <FL/Fl_Box.H>
#include <FL/Fl_Button.H>
#include <FL/fl_draw.H>
using namespace std;


class TogButton : public Fl_Button 
{
int Y = y(); //чтобы вернуть кнопку на место
public:
	TogButton(int X, int Y, int W, int H, const char* L = 0)
		:Fl_Button(X, Y, W, H, L)
	{
		color(fl_rgb_color(169, 169, 169));
		selection_color(fl_rgb_color(143, 175, 255));
		clear_visible_focus();
	}
	
	int handle(int event) override 
	{
		switch (event)
		{
		case FL_ENTER:
		{
			position(x(), Y - 5);
			redraw();
			if (parent()) { parent()->parent()->redraw(); }
			return 1;
		}
		case FL_LEAVE:
		{
			position(x(), Y);
			redraw();
			if (parent()) { parent()->parent()->redraw(); }
			return 1;
		}
		}
		
		return Fl_Button::handle(event);
	}
};

//создать свой класс, со своими параметрами кнопи
class MyButton : public Fl_Button 
{
public:
	//параметры кнопки
	MyButton(int X, int Y, int W, int H, const char* L = 0)
		: Fl_Button(X, Y, W, H, L) 
	{
		color(fl_rgb_color(169, 169, 169));
		selection_color(fl_rgb_color(100, 100, 105));
		clear_visible_focus();
	}

	//обработка событий наведения
	int handle(int event) override 
	{
		switch (event)
		{
		case FL_ENTER:
		{
			color(fl_rgb_color(100, 100, 105));
			redraw();
			return 1;
		}
		case FL_LEAVE:
		{
			color(fl_rgb_color(169, 169, 169));
			redraw();
			return 1;
			return 1;
		}
		}

		return Fl_Button::handle(event);
	}
	//сброс состояния при переходе в другие меню
	void reset_state() {
		color(fl_rgb_color(169, 169, 169));
		redraw();
	}

};

//структуры определения игры
struct GameLevel {
	int rows;
	int cols;
	int mines_count;
	int max_moves;
	int max_lives;
	string name;
};
GameLevel levels[] = {
	{10, 10, 10, 50, 3, "легкий"},
	{15, 15, 23, 75, 2, "средний"},
	{20, 25, 50, 100, 1, "сложный"}
};

//глобальные переменные для игры
 
int game_level = NULL;

//графика
void ShowConsole() {
	::ShowWindow(::GetConsoleWindow(), SW_SHOW);
}
void HideConsole() {
	::ShowWindow(::GetConsoleWindow(), SW_HIDE);
}

void toGameMenu(Fl_Widget* w, void* data);
void exitf(Fl_Widget* w, void* data);
void toGameSettings(Fl_Widget* w, void* data);
void choose_level(Fl_Widget* w, void* data);

int main(int argc, char** argv)
{
	HideConsole();
	SetConsoleOutputCP(CP_UTF8); SetConsoleCP(CP_UTF8);
	Fl_Double_Window win(1000, 600, "игра");

	win.color(fl_rgb_color(192, 192, 192));

	//группа приветственного меню (0)
	Fl_Group* helloWin = new Fl_Group(0, 0, 1000, 600);
	MyButton begin(435.5, 250, 125, 60, "Начать");
	begin.callback(toGameMenu, helloWin->parent());
	helloWin->end();

	//группа игрового меню (1)
	Fl_Group* mainMenu = new Fl_Group(0, 0, 1000, 600);
	MyButton start(100, 150, 150, 75, "Начать игру");
	MyButton rules(100, 250, 150, 75, "Правила");
	MyButton exit(100, 350, 150, 75, "Выход");
	start.callback(toGameSettings, &win);
	exit.callback(exitf, nullptr);
	mainMenu->hide();
	mainMenu->end();

	//Группа настройки игры(2)
	Fl_Group* game_settings = new Fl_Group(0, 0, 1000, 600);

	TogButton easy(175, 150, 150, 70, "Легко");
	TogButton normal(425, 150, 150, 70, "Нормально");
	TogButton hard(675, 150, 150, 70, "Сложно");

	MyButton back(9, 531, 125, 60, "В меню");

	back.callback(toGameMenu, &win);
	easy.callback(choose_level, &win);
	normal.callback(choose_level, &win);
	hard.callback(choose_level, &win);

	game_settings->hide();
	game_settings->end();

	win.end();
	win.show(argc, argv);
	return Fl::run();
}

void toGameMenu(Fl_Widget* w, void* data)
{
	Fl_Double_Window* win = (Fl_Double_Window*)data;
	Fl_Group* group = (Fl_Group*)win->child(1);
	MyButton* but = NULL;
	if (win->child(0)->visible())
	{
		win->child(0)->hide();
		win->child(1)->show();
		for (short i = 0; i < group->children(); i++)
		{
			but = (MyButton*)group->child(i);
			but->reset_state();
		}
	}
	else if(win->child(2)->visible())
	{
		win->child(2)->hide();
		win->child(1)->show();
		choose_level(w, data);
		for (short i = 0; i < group->children(); i++)
		{
			but = (MyButton*)group->child(i);
			but->reset_state();
		}
	}


}

void exitf(Fl_Widget* w, void* data)
{
	exit(0);
}

void toGameSettings(Fl_Widget* w, void* data)
{
	Fl_Double_Window* win = (Fl_Double_Window*)data;
	Fl_Group* group = (Fl_Group*)win->child(2);
	MyButton* but = NULL;

	for (short i = 0; i < group->children(); i++)
	{
		but = (MyButton*)group->child(i);
		but->reset_state();
	}
	win->child(1)->hide();
	win->child(2)->show();
}

void choose_level(Fl_Widget* w, void* data)
{
	Fl_Double_Window* win = (Fl_Double_Window*)data;
	Fl_Group* group = (Fl_Group*)win->child(2);

	game_level = 0;

	for (short i = 0; i < 3; i++)
	{
		group->child(i)->color(fl_rgb_color(169, 169, 169));
	}

	if (w->label() == "Легко")
	{
		game_level = 1;
		w->color(fl_rgb_color(255, 128, 64));
	}
	else if (w->label() == "Нормально")
	{
		game_level = 2;
		w->color(fl_rgb_color(255, 128, 64));
	}
	else if (w->label() == "Сложно")
	{
		game_level = 3;
		w->color(fl_rgb_color(255, 128, 64));
	}

	if (w->label() != "В меню")
	{
		cout << "Выбрана сложность:" << w->label() << endl;
	}
	else
	{
		cout << "Сложность сброшена" << endl;
	}

	win->redraw();
}


