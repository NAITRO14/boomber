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
//переключаемые кнопки
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

//общий класс кнопки
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
			inFocus = false;
			rsOfBlock();
			return Fl_Button::handle(event);
		}
		case FL_ENTER:
		{
			inFocus = true;
			color(fl_rgb_color(100, 100, 105));
			startAnimation(true);
			ShowSign(this);
			return 1;
		}
		case FL_LEAVE:
		{
			inFocus = false;
			color(fl_rgb_color(169, 169, 169));
			startAnimation(false);
			ShowSign(this);
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
			:but(b), final_w(fw), final_h(fh), cur_w(cw), cur_h(ch), orig_x(ox), orig_y(oy), curX(cx), curY(cy), orig_W(ow), orig_H(oh), action(act) {}
	};
	void startAnimation(bool ent)
	{

		animationVal* anim = new animationVal(this, fW, fH, w(), h(), Xold, Yold, x(), y(), Wold, Hold, ent);
		Fl::add_timeout(0.02, animation, anim);
	}

	//статическая, чтобы не было this
	static void animation(void* data)
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

//дочерний для кнопок в меню
class menuBut : public MyButton
{

public:
	menuBut(int X, int Y, int W, int H, const char* L = 0)
		: MyButton(X, Y, W, H, L) {}

	int handle(int event)
	{
		switch (event)
		{
		case FL_PUSH:
		{
			if (!PlaySound(TEXT("sounds/on_click.wav"), NULL, SND_FILENAME | SND_ASYNC))
			{
				cout << "Звук не найден" << endl;
			}
		}break;
		case FL_ENTER:
		{
			if (!PlaySound(TEXT("sounds/on_aim.wav"), NULL, SND_FILENAME | SND_ASYNC))
			{
				cout << "Звук не найден" << endl;
			}
		}break;
		}
		return MyButton::handle(event);
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

//дочерний класс для кнопки "играть"
class  PlayBut : public MyButton
{
	

	Fl_Color startColor = fl_rgb_color(169, 169, 169);
	Fl_Color endColor = fl_rgb_color(237, 55, 55);
public:
	PlayBut(int X, int Y, int W, int H, const char* L = 0)
		: MyButton(X, Y, W, H, L) {}

	float Progress = 0.0f;
	bool anim = false;
	bool icreasing = false;

	int handle(int event)
	{
		switch (event)
		{
		case FL_ENTER:
		{
			startAnim(true);
			if (!PlaySound(TEXT("sounds/pbip.wav"), NULL, SND_FILENAME | SND_ASYNC))
			{
				cout << "Звук не найден" << endl;
			}

		}break;
		case FL_LEAVE:
		{
			startAnim(false);
		}

		}
		return MyButton::handle(event);
	}

	void startAnim(bool inc)
	{
		icreasing = inc;
		anim = true;
		Progress = icreasing ? 0.0f : 1.0f; //тернарный оператор(if/else)

		Fl::add_timeout(0.02, animation, this);
	}

	static void animation(void* data)
	{
		PlayBut* but = (PlayBut*)data;

		if (but->anim)
		{
			if (but->icreasing)
			{
				if (but->Progress < 1.0f)
				{
					but->Progress += 0.05f;
				}
				else { but->anim = false; }
			}
			else
			{
				if (but->Progress > 0.0f)
				{
					but->Progress -= 0.05f;
				}
				else { but->anim = false; }
			}
		}
		but->newColor();
		but->redraw();
		if (but->anim)
		{
			Fl::repeat_timeout(0.02, animation, but);
		}

	}

	void newColor()
	{
		int r = 169 + (237 - 169) * Progress;
		int g = 169 + (55 - 169) * Progress;
		int b = 169 + (55 - 169) * Progress;
		color(fl_rgb_color(r, g, b));
	}

};

class PGBut : public Fl_Button
{
public:
	PGBut(int X, int Y, int W, int H, const char* L = 0)
		: Fl_Button(X, Y, W, H, L)
	{
		color(fl_rgb_color(233, 240, 234));
		selection_color(fl_rgb_color(100, 100, 105));
		labelfont(FL_HELVETICA_BOLD);
		clear_visible_focus();
		labelsize(25);
	}
	
	void reset_state()
	{
		color(fl_rgb_color(233, 240, 234));
		box(FL_UP_BOX);
		labelcolor(FL_BLACK);
		label("");
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
struct GameData
{
	short level;
	PGBut* ButAr[20][25];
	char** field;
	PGBut* pedBut;
	bool** opened;
	Fl_Double_Window* win;
	short curX;
	short curY;
};

const GameLevel levels[] =
{
	{10, 10, 10, "легкий"},
	{15, 15, 23, "средний"},
	{20, 25, 50, "сложный"}
};

//глобальные переменные для игры

int game_level = 0;
GameData GData;

//функционал
void initialize_field();
void place_mines(char** _field, int rows, int cols, int mines_count, PGBut* ButAr);
void save_field_to_file(char** _field, int rows, int cols, const char* _filename);
bool open(short i, short j);
bool inbounds(int row, int col, int rows, int cols);
void open_empty(char** _field, bool** _opened, int rows, int cols, int _row, int _col);
void calculate_numbers(char** field, int rows, int cols);
void findXY();


int dx[] = { -1, -1, -1, 0, 0, 1, 1, 1 };
int dy[] = { -1, 0, 1, -1, 1, -1, 0, 1 };

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
void Game(Fl_Widget* w, void* data);
void ButPressed(Fl_Widget* w, void* data);
void again(Fl_Widget* w, void* data);
void redraw();

short moves = 0;

int main(int argc, char** argv)
{
	//настройка звука приложения
	/*WORD vl = (WORD)(0xFFFF);
	DWORD volume = MAKELONG(vl, vl);
	waveOutSetVolume(NULL, volume);*/

	//ЕСЛИ НОВЫЕ ВИДЖЕТЫ НЕ ОТРИСОВЫВАЮТСЯ, НУЖНО ПРОВЕРИТЬ ФУНКЦИИ. ГДЕ-ТО ОНИ МОГУТ ЯВНО ПРИВОДИТЬСЯ К НЕВЕРНОМУ КЛАССУ!!!

	

	HideConsole();
	SetConsoleOutputCP(CP_UTF8); SetConsoleCP(CP_UTF8); srand(time(NULL));
	Fl_Double_Window win(1000, 600, "Boomber");
	GData.win = &win;

	win.color(fl_rgb_color(192, 192, 192));

	//группа приветственного меню (0)
	Fl_Group* helloWin = new Fl_Group(0, 0, 1000, 600);
	menuBut begin(435.5, 250, 125, 60, "Начать");
	begin.callback(toGameMenu, helloWin->parent());
	helloWin->end();

	//группа игрового меню (1)
	Fl_Group* mainMenu = new Fl_Group(0, 0, 1000, 600);

	menuBut start(100, 150, 150, 75, "Начать игру");
	menuBut rules(100, 250, 150, 75, "Правила");
	menuBut exit(100, 350, 150, 75, "Выход");

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
	menuBut back(15, 525, 125, 60, "Назад");
	 
	PlayBut play(408, 353, 184, 76, "Играть");

	BoxForBut Teasy(166, -56, 166, 56, "Размер поля: 10х10\nКоличество мин: 10");
	BoxForBut Tnormal(416, -56, 166, 56, "Размер поля: 15х15\nКоличество мин: 23");
	BoxForBut Thard(666, -56, 166, 56, "Размер поля: 25х20\nКоличество мин: 50");

	

	play.callback(Game, &win);
	back.callback(toGameMenu, &win);
	easy.callback(choose_level, &win);
	normal.callback(choose_level, &win);
	hard.callback(choose_level, &win);
	game_settings->end();
	game_settings->hide();

	//группа легкой сложности (3)
	
	Fl_Group* ez_g = new Fl_Group(0, 0, 1000, 600);
	short x, y;
	const short size = 10;
	PGBut* ez[size][size];

	BoxForBut et1(620, 37, 350, 60); 
	Fl_Box cur_time(620, 37, 175, 60, "Время:"); 

	Fl_Box cur_steps(795, 37, 175, 60, "Ходов:0");

	BoxForBut et2(620, 131, 350, 246);

	menuBut leaveLvl(606, 510, 169, 63, "В меню");
	menuBut againLvl(814, 510, 169, 63, "Заново");

	//игровое поле
	y = 10;
	for(short i = 0; i < size; i++)
	{
		x = 10;
		for (short j = 0; j < size; j++)
		{
			ez[i][j] = new PGBut(x, y, 58, 58);
			GData.ButAr[i][j] = ez[i][j];
			ez[i][j]->callback(ButPressed, ez);
			x += 58;
		}
		y += 58;
	}

	leaveLvl.callback(toGameMenu, &win);
	againLvl.callback(again, nullptr);

	ez_g->end();
	ez_g->hide();

	win.end();
	win.show(argc, argv);
	return Fl::run();
}

void toGameMenu(Fl_Widget* w, void* data)
{
	Fl_Double_Window* win = (Fl_Double_Window*)data;
	menuBut* self = (menuBut*)w;
	self->reset_state();

	game_level = 0; moves = 0;

	if (win->child(0)->visible())
	{
		win->child(0)->hide();
		win->child(1)->show();
	}
	else if (win->child(2)->visible())
	{
		win->child(2)->hide();
		win->child(1)->show();
		choose_level(w, data);
	}
	else if (win->child(3)->visible())
	{
		win->child(3)->hide();
		win->child(1)->show();
		again(nullptr, nullptr);
		choose_level(w, data);
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
	TogButton* TogBut = NULL;
	menuBut* self = (menuBut*)w;
	self->reset_state();

	for (short i = 0; i < 3; i++)
	{
		TogBut = (TogButton*)group->child(i);
		TogBut->reset_state();
	}
	win->child(1)->hide();
	win->child(2)->show();
}

void choose_level(Fl_Widget* w, void* data)
{
	Fl_Double_Window* win = (Fl_Double_Window*)data;
	Fl_Group* group = (Fl_Group*)win->child(2);

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

	GData.level = game_level;

	win->redraw();
}

void Game(Fl_Widget* w, void* data)
{
	Fl_Double_Window* win = (Fl_Double_Window*)data;
	win->child(2)->hide();
	PlayBut* self = (PlayBut*)w;
	self->icreasing = false;
	self->reset_state();

	if (game_level == 0)
	{
		win->child(2)->show();
	}
	else if (game_level == 1)
	{
		win->child(3)->show();
	}
	else if (game_level == 2)
	{
		
	}
	else if (game_level == 3)
	{
		
	}
}

void findXY()
{
	for (short i = 0; i < levels[GData.level - 1].rows; i++)
	{
		for (short j = 0; j < levels[GData.level - 1].cols; j++)
		{
			if (GData.ButAr[i][j] == GData.pedBut)
			{
				GData.curX = i;
				GData.curY = j;
			}
		}
	}
}

void ButPressed(Fl_Widget* w, void* data)
{
	GData.pedBut = (PGBut*)w;

	findXY();
	if (moves != 0)
	{
		moves ++;
		open(GData.curX, GData.curY);
	}
	else
	{
		moves++;
		initialize_field();

		place_mines(GData.field, levels[GData.level - 1].rows, levels[GData.level - 1].cols, levels[GData.level - 1].mines_count, GData.ButAr[0][0]);
		calculate_numbers(GData.field, levels[GData.level - 1].rows, levels[GData.level - 1].cols);

		open(GData.curX, GData.curY);
	}
}


void ShowSign(void* data)
{
	menuBut* but = (menuBut*)data;
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
		else if (!but->inFocus and box->y() > -32)
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
	//нормально
	else if (strcmp(but->label(), "Нормально") == 0)
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
	//сложно
	else if (strcmp(but->label(), "Сложно") == 0)
	{
		Fl_Widget* box = but->parent()->child(7);
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

//функционал
void initialize_field()
{
	short rows = levels[GData.level-1].rows;
	short cols = levels[GData.level - 1].cols;

	char** field = new char* [rows];
	bool** opened = new bool* [rows];
	for (int i = 0; i < rows; i++) {
		field[i] = new char[cols];
		opened[i] = new bool[cols];
		for (int j = 0; j < cols; j++) {
			field[i][j] = '0'; // Временно заполняем нулями
			opened[i][j] = false;
		}
	}
	GData.field = field;
	GData.opened = opened;
}

void place_mines(char** _field, int rows, int cols, int mines_count, PGBut* ButAr)
{
	int mines_placed = 0;
	while (mines_placed < mines_count)
	{
		int x = rand() % rows;
		int y = rand() % cols;

		if (GData.ButAr[x][y] == GData.pedBut or abs(x - GData.curX) <= 1 and abs(y - GData.curY) <= 1)
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

bool inbounds(int row, int col, int rows, int cols)
{
	return row >= 0 && row < rows && col >= 0 && col < cols;
}

void calculate_numbers(char** field, int rows, int cols)
{
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

void open_empty(char** _field, bool** _opened, int rows, int cols, int _row, int _col)
{
	if (!inbounds(_row, _col, rows, cols)) return;
	if (_opened[_row][_col]) return;

	_opened[_row][_col] = true;
	if (_field[_row][_col] != '0') return;

	for (int k = 0; k < 8; k++)
	{
		open_empty(_field, _opened, rows, cols, _row + dx[k], _col + dy[k]);
	}
	
}

bool open(short i, short j)
{
	if (GData.field[i][j] == '0')
	{
		open_empty(GData.field, GData.opened, levels[GData.level-1].rows, levels[GData.level - 1].cols, i, j);
		redraw();
	}
	else if (GData.field[i][j] == '*')
	{
		GData.opened[i][j] = true;
		redraw();
	}
	else
	{
		GData.opened[i][j] = true;
		redraw();
	}

	return 1;
}

void redraw()
{
	for (short i = 0; i < levels[GData.level-1].rows; i++)
	{
		for (short j = 0; j < levels[GData.level-1].cols; j++)
		{
			if (GData.opened[i][j] == true)
			{
				if(GData.field[i][j] == '0')
				{
					GData.ButAr[i][j]->box(FL_DOWN_BOX);
				}
				else if (GData.field[i][j] == '*')
				{
					GData.ButAr[i][j]->box(FL_DOWN_BOX);
					GData.ButAr[i][j]->color(FL_RED);
					GData.ButAr[i][j]->label("*");
				}
				else
				{
					if (GData.field[i][j] == '1')
					{
						GData.ButAr[i][j]->label("1");
						GData.ButAr[i][j]->labelcolor(fl_rgb_color(89, 194, 255));
					}
					else if (GData.field[i][j] == '2')
					{
						GData.ButAr[i][j]->label("2");
						GData.ButAr[i][j]->labelcolor(fl_rgb_color(64, 105, 255));
					}
					else if (GData.field[i][j] == '3')
					{
						GData.ButAr[i][j]->label("3");
						GData.ButAr[i][j]->labelcolor(fl_rgb_color(82, 255, 119));
					}
					else if (GData.field[i][j] == '4')
					{
						GData.ButAr[i][j]->label("4");
						GData.ButAr[i][j]->labelcolor(fl_rgb_color(251, 38, 255));
					}
					else if (GData.field[i][j] == '5')
					{
						GData.ButAr[i][j]->label("5");
						GData.ButAr[i][j]->labelcolor(fl_rgb_color(177, 10, 255));
					}
					else if (GData.field[i][j] == '6')
					{
						GData.ButAr[i][j]->label("6");
						GData.ButAr[i][j]->labelcolor(fl_rgb_color(255, 195, 43));
					}
					else if (GData.field[i][j] == '7')
					{
						GData.ButAr[i][j]->label("7");
						GData.ButAr[i][j]->labelcolor(fl_rgb_color(255, 66, 66));
					}
					else if (GData.field[i][j] == '8')
					{
						GData.ButAr[i][j]->label("8");
						GData.ButAr[i][j]->labelcolor(fl_rgb_color(255, 0, 0));
					}
					else
					{
						GData.ButAr[i][j]->label("X");
					}
				}
				GData.ButAr[i][j]->redraw();
			}
		}
	}
	GData.win->redraw();
}

void again(Fl_Widget* w, void* data)
{
	moves = 0;

	for (short i = 0; i < levels[GData.level - 1].rows; i++)
	{
		for (short j = 0; j < levels[GData.level - 1].cols; j++)
		{
			GData.ButAr[i][j]->reset_state();
		}
	}
}


