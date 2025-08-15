#define _CRT_SECURE_NO_WARNINGS
//функцонал
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

//звуки
#include <mmsystem.h>
#pragma comment(lib, "winmm.lib")
using namespace std;

void ShowSign(void* data);

class TogButton : public Fl_Button 
{
int Y = y(); //чтобы вернуть кнопку на место
public:
	bool inFocus = false;

	TogButton(int X, int Y, int W, int H, const char* L = 0)
		:Fl_Button(X, Y, W, H, L)
	{
		color(fl_rgb_color(169, 169, 169));
		selection_color(fl_rgb_color(143, 175, 255));
		clear_visible_focus();
		labelsize(24);
	}
	
	int handle(int event) override 
	{
		switch (event)
		{
		case FL_PUSH:
		{
			if (!PlaySound(TEXT("sounds/selection.wav"), NULL, SND_FILENAME | SND_ASYNC))
			{
				cout << "Звук не найден" << endl;
			}
			return Fl_Button::handle(event);
		}
		case FL_ENTER:
		{
			if (!PlaySound(TEXT("sounds/on_aim.wav"), NULL, SND_FILENAME | SND_ASYNC))
			{
				cout << "Звук не найден" << endl;
			}
			inFocus = true;
			ShowSign(this);
			position(x(), Y - 5);
			redraw();
			if (parent()) { parent()->parent()->redraw(); }
			return 1;
		}
		case FL_LEAVE:
		{
			inFocus = false;
			ShowSign(this);
			position(x(), Y);
			redraw();
			if (parent()) { parent()->parent()->redraw(); }
			return 1;
		}
		}

		
		return Fl_Button::handle(event);
	}

	void reset_state() {
		color(fl_rgb_color(169, 169, 169));
		redraw();
		if (parent()) { parent()->parent()->redraw(); }
	}
};

//обычная кнопка 
class MyButton : public Fl_Button 
{
	int Wold = w(), Hold = h(), Xold = x(), Yold = y();
	int Wnew = 0, Hnew = 0, Xnew = 0, Ynew = 0;

	int fW = w() + 20, fH = h() + 10;
public:
	bool inFocus = NULL;
	//параметры кнопки
	MyButton(int X, int Y, int W, int H, const char* L = 0)
		: Fl_Button(X, Y, W, H, L) 
	{
		color(fl_rgb_color(169, 169, 169));
		selection_color(fl_rgb_color(100, 100, 105));
		clear_visible_focus();
		labelsize(20);
	}

	//обработка событий наведения
	int handle(int event) override 
	{
		switch (event)
		{
		case FL_PUSH:
		{
			if (!PlaySound(TEXT("sounds/on_click.wav"), NULL, SND_FILENAME | SND_ASYNC))
			{
				cout << "Звук не найден" << endl;
			}
			inFocus = false;
			rsOfBlock();
			return Fl_Button::handle(event);
		}
		case FL_ENTER:
		{
			if (!PlaySound(TEXT("sounds/on_aim.wav"), NULL, SND_FILENAME | SND_ASYNC))
			{
				cout << "Звук не найден" << endl;
			}
			inFocus = true;
			color(fl_rgb_color(100, 100, 105));
			startAnimation(true);
			ShowSign(this);
			/*ShowSignData* data = new ShowSignData{ this, this->inFocus };
			Fl::add_timeout(0.02, ShowSign, data);*/
			return 1;
		}
		case FL_LEAVE:
		{ 
			inFocus = false;
			color(fl_rgb_color(169, 169, 169));
			startAnimation(false);
			ShowSign(this);
			/*ShowSignData* data = new ShowSignData{ this, this->inFocus };
			Fl::add_timeout(0.02, ShowSign, data);*/
			return 1;
		}
		}
		return Fl_Button::handle(event);
	}
	//сброс состояния при переходе в другие меню
	void reset_state() {
		resize(Xold, Yold, Wold, Hold);
		color(fl_rgb_color(169, 169, 169));
		redraw();
		if (parent()) { parent()->parent()->redraw(); }
	}
	void rsOfBlock()//вернуть детей на место после перехода
	{
		if (strcmp(label(), "Начать игру") == 0)
		{
			parent()->child(3)->resize(300, -32, 291, 32);
			parent()->child(4)->resize(1000, 272, 291, 32);
			parent()->child(5)->resize(300, 600, 291, 32);
		}
	}
private:
	struct ShowSignData
	{
		MyButton* but;
		bool inFocus;
	};

	struct animationVal
	{//переменные для анимации
		MyButton* but;
		int final_w, final_h;
		int cur_w, cur_h;
		int orig_x, orig_y;
		int curX, curY;
		int orig_W, orig_H;
		bool action; // увеличчивать или уменьшать

		//кнструктор для анимации
		animationVal(MyButton* b, int fw, int fh, int cw, int ch, int ox, int oy, int cx, int cy, int ow, int oh, bool act)
			:but(b), final_w(fw), final_h(fh), cur_w(cw), cur_h(ch), orig_x(ox), orig_y(oy), curX(cx), curY(cy),orig_W(ow), orig_H(oh), action(act){}
	};
	void startAnimation(bool ent)
	{

		animationVal* anim = new animationVal(this, fW, fH, w(), h(), Xold, Yold, x(), y(), Wold, Hold, ent);
		Fl::add_timeout(0.02, animation, anim);
	}

	//статическая, чтобы не было this
	static void animation(void*data)
	{
		animationVal* anim = (animationVal*)data;
		MyButton* but = anim->but;
		if (!anim or !anim->but) return;//проверка чтобы нопка была
		
		//увеличивать блок
		if (anim->action == true)
		{
			if (anim->final_w > anim->cur_w and anim->final_h > anim->cur_h)
			{
				anim->cur_w += 2;
				anim->cur_h += 1;
				anim->curX = anim->orig_x - (anim->cur_w - anim->orig_W) / 2;
				anim->curY = anim->orig_y - (anim->cur_h - anim->orig_H) / 2;

				but->resize(anim->curX, anim->curY, anim->cur_w, anim->cur_h);
				but->redraw();
				but->parent()->parent()->redraw();

				if (but->inFocus)
				{
					if (!(anim->final_w == anim->cur_w and anim->final_h == anim->cur_h))
					{
						Fl::add_timeout(0.02, animation, anim);
					}
					else
					{//если закончили - освободить память
						delete anim;
					}
				}
				else
				{
					return;
				}
			}
		}
		else
		{//уменьшать блок
			if (anim->orig_W < anim->cur_w and anim->orig_H < anim->cur_h)
			{
				anim->cur_w -= 2;
				anim->cur_h -= 1;

				anim->curX = anim->orig_x + (anim->orig_W - anim->cur_w) / 2;
				anim->curY = anim->orig_y + (anim->orig_H - anim->cur_h) / 2;

				but->resize(anim->curX, anim->curY, anim->cur_w, anim->cur_h);
				but->redraw();
				but->parent()->parent()->redraw();
				bool done = anim->final_w == anim->cur_w and anim->final_h == anim->cur_h;
				if (!done)
				{
					Fl::add_timeout(0.02, animation, anim);
				}
				else
				{//если закончили - освободить память
					delete anim;
				}
			}
		}

	}
};

//выезжающие тексты
class BoxForBut : Fl_Box
{
public:
	BoxForBut(int X, int Y, int W, int H, const char* L = 0)
		:Fl_Box(X, Y, W, H, L)
	{
		color(fl_rgb_color(169, 169, 169));
		box(FL_FLAT_BOX);
		labelsize(18);
	}
};

//структуры определения игры
struct GameLevel
{
	int rows;
	int cols;
	int mines_count;
	string name;
};

const GameLevel levels[] =
{
	{10, 10, 10, "легкий"},
	{15, 15, 23, "средний"},
	{20, 25, 50, "сложный"}
};

//глобальные переменные для игры
 
int game_level = 0;

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
	//настройка звука приложения
	/*WORD vl = (WORD)(0xFFFF);
	DWORD volume = MAKELONG(vl, vl);
	waveOutSetVolume(NULL, volume);*/

	//ЕСЛИ НОВЫЕ ВИДЖЕТЫ НЕ ОТРИСОВЫВАЮТСЯ, НУЖНО ПРОВЕРИТЬ ФУНКЦИИ. ГДЕ-ТО ОНИ МОГУТ ЯВНО ПРИВОДИТЬСЯ К НЕВЕРНОМУ КЛАССУ!!!

	HideConsole();
	SetConsoleOutputCP(CP_UTF8); SetConsoleCP(CP_UTF8);
	Fl_Double_Window win(1000, 600, "Boomber");

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

	BoxForBut Tstart(300, -32, 291, 32, "Перейти к настройке сложности");
	BoxForBut Trules(1000, 272, 291, 32, "Ознакомиться с правилами игры");
	BoxForBut Texit(300, 600, 291, 32, "Закрыть приложение");

	
	start.callback(toGameSettings, &win);
	exit.callback(exitf, nullptr);
	mainMenu->hide();
	mainMenu->end();

	//Группа настройки игры(2)
	Fl_Group* game_settings = new Fl_Group(0, 0, 1000, 600);

	TogButton easy(175, 150, 150, 70, "Легко");
	TogButton normal(425, 150, 150, 70, "Нормально");
	TogButton hard(675, 150, 150, 70, "Сложно");
	MyButton back(15, 525, 125, 60, "Назад");

	BoxForBut Teasy(166, -56, 166, 56, "Размер поля: 10х10\nКоличество мин: 10");
	BoxForBut Tnormal(416, -56, 166, 56, "Размер поля: 15х15\nКоличество мин: 23");
	BoxForBut Thard(666, -56, 166, 56, "Размер поля: 25х20\nКоличество мин: 50");


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
		for (short i = 0; i < 3; i++)
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
		for (short i = 0; i < 3; i++)
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
	MyButton* Mybut = NULL; TogButton* TogBut = NULL;

	for (short i = 0; i < 4; i++)
	{
		if (i == 3) 
		{ 
			Mybut = (MyButton*)group->child(i);
			Mybut->reset_state();
		}
		else
		{
			TogBut = (TogButton*)group->child(i);
			TogBut->reset_state();
		}
		
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

void ShowSign(void* data)
{
	MyButton* but = (MyButton*)data;
	//начать игру
	if (strcmp(but->label(), "Начать игру") == 0)
	{
		Fl_Widget* box = but->parent()->child(3);
		if (but->inFocus and box->y() < 172)
		{
			box->resize(box->x(), box->y() + 4, box->w(), box->h());
			box->redraw();
			box->parent()->parent()->redraw();

			Fl::add_timeout(0.005, ShowSign, data);
		}
		else if(!but->inFocus and box->y() > -32)
		{
			box->resize(box->x(), box->y() - 4, box->w(), box->h());
			box->redraw();
			box->parent()->parent()->redraw();

			Fl::add_timeout(0.005, ShowSign, data);
		}
	}
	//правила
	else if ((strcmp(but->label(), "Правила") == 0))
	{
		Fl_Widget* box = but->parent()->child(4);
		if (but->inFocus and box->x() > 300)
		{
			box->resize(box->x() - 10, box->y(), box->w(), box->h());
			box->redraw();
			box->parent()->parent()->redraw();

			Fl::add_timeout(0.005, ShowSign, data);
		}
		else if (!but->inFocus and box->x() < 1000)
		{
			box->resize(box->x() + 10, box->y(), box->w(), box->h());
			box->redraw();
			box->parent()->parent()->redraw();

			Fl::add_timeout(0.005, ShowSign, data);
		}
	}
	//выход
	else if (strcmp(but->label(), "Выход") == 0)
	{
		Fl_Widget* box = but->parent()->child(5);
		if (but->inFocus and box->y() > 372)
		{
			box->resize(box->x(), box->y() - 4, box->w(), box->h());
			box->redraw();
			box->parent()->parent()->redraw();

			Fl::add_timeout(0.005, ShowSign, data);
		}
		else if (!but->inFocus and box->y() < 600)
		{
			box->resize(box->x(), box->y() + 4, box->w(), box->h());
			box->redraw();
			box->parent()->parent()->redraw();

			Fl::add_timeout(0.005, ShowSign, data);
		}
	}
	//легко
	else if (strcmp(but->label(), "Легко") == 0)
	{
		Fl_Widget* box = but->parent()->child(4);
		TogButton* tBut = (TogButton*)data;

		if (tBut->inFocus and box->y() < 0)
		{
			box->resize(box->x(), box->y() + 4, box->w(), box->h());
			box->redraw();
			box->parent()->parent()->redraw();

			Fl::add_timeout(0.005, ShowSign, data);
		}
		else if (!tBut->inFocus and box->y() > -56)
		{
			box->resize(box->x(), box->y() - 4, box->w(), box->h());
			box->redraw();
			box->parent()->parent()->redraw();

			Fl::add_timeout(0.005, ShowSign, data);
		}
	}
	//нормально
	else if (strcmp(but->label(), "Нормально") == 0)
	{
		Fl_Widget* box = but->parent()->child(5);
		TogButton* tBut = (TogButton*)data;

		if (tBut->inFocus and box->y() < 0)
		{
			box->resize(box->x(), box->y() + 4, box->w(), box->h());
			box->redraw();
			box->parent()->parent()->redraw();

			Fl::add_timeout(0.005, ShowSign, data);
		}
		else if (!tBut->inFocus and box->y() > -56)
		{
			box->resize(box->x(), box->y() - 4, box->w(), box->h());
			box->redraw();
			box->parent()->parent()->redraw();

			Fl::add_timeout(0.005, ShowSign, data);
		}
	}
	//сложно
	else if (strcmp(but->label(), "Сложно") == 0)
	{
		Fl_Widget* box = but->parent()->child(6);
		TogButton* tBut = (TogButton*)data;

		if (tBut->inFocus and box->y() < 0)
		{
			box->resize(box->x(), box->y() + 4, box->w(), box->h());
			box->redraw();
			box->parent()->parent()->redraw();

			Fl::add_timeout(0.005, ShowSign, data);
		}
		else if (!tBut->inFocus and box->y() > -56)
		{
			box->resize(box->x(), box->y() - 4, box->w(), box->h());
			box->redraw();
			box->parent()->parent()->redraw();

			Fl::add_timeout(0.005, ShowSign, data);
		}
		}
}


