#define _CRT_SECURE_NO_WARNINGS
#define SDL_MAIN_HANDLED
//функцонал
#include <iostream>
#include <windows.h>
#include <iomanip>
#include <stdlib.h>
#include <string.h>
#include <string>
#include "resource.h"

//графика
#include <FL/Fl.H>
#include <FL/Fl_Double_Window.H> // использование двойной буферизации
#include <FL/Fl_Box.H>
#include <FL/Fl_Button.H>
#include <FL/fl_draw.H>

//звуки
#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>

using namespace std;

//функционал
void initialize_field();
void place_mines(char** _field, int rows, int cols, int mines_count);
void save_field_to_file(char** _field, int rows, int cols, const char* _filename);
bool open(short i, short j);
bool inbounds(int row, int col, int rows, int cols);
void open_empty(char** _field, bool** _opened, int rows, int cols, int _row, int _col);
void calculate_numbers(char** field, int rows, int cols);
void dock(Fl_Widget* w, void* data);
void timer(void* data);
void counts_redraw();
short flags_count();
void winOrFail();
void findXY();

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
void ShowSign(void* data);
void toGameRule(Fl_Widget* w, void* data);
void alertf(void* data);
void drowField();
void showField();
void redraw();

//информация об уровнях
struct GameLevel
{
	int rows;
	int cols;
	int mines_count;
	string name;
};

//основная информация об игре
struct GameData
{
	Fl_Double_Window* win;
	Fl_Widget* pedBut;
	short level;
	char** field;
	bool** opened;
	short curX;
	short curY;
	short zCount;
	short GTime;
	bool isHackAct;	
};

//хранилище меню
struct menu
{
	Fl_Group* hello;
	Fl_Group* main;
	Fl_Group* settings;
	Fl_Group* easy;
	Fl_Group* normal;
	Fl_Group* hard;
	Fl_Group* rules;
};

//хранилище экранов победы/поражения
struct screen
{
	Fl_Group* gl1;
	Fl_Group* gl2;
	Fl_Group* gl3;

	Fl_Group* gw1;
	Fl_Group* gw2;
	Fl_Group* gw3;
};

//данные об уровне
const GameLevel levels[] =
{
	{10, 10, 15, "легкий"},
	{15, 15, 35, "средний"},
	{19, 36, 100, "сложный"}
};

GameData GData;
menu menues;
screen screens;


//переключаемые кнопки
class TogButton : public Fl_Button
{
	int Y = y(); //чтобы вернуть кнопку на место

	Mix_Chunk* click = nullptr;
	Mix_Chunk* hover = nullptr;
public:
	bool inFocus = false;

	TogButton(int X, int Y, int W, int H, const char* L = 0)
		:Fl_Button(X, Y, W, H, L)
	{
		color(fl_rgb_color(169, 169, 169));
		selection_color(fl_rgb_color(143, 175, 255));
		clear_visible_focus();
		labelsize(24);

		click = Mix_LoadWAV("sounds/selection.wav");
		hover = Mix_LoadWAV("sounds/on_aim.wav");
		if (!click or !hover)
		{
			cerr << "Звук не загружен" << Mix_GetError() << endl;
		}
	}

	static void delay(void* data)
	{
		TogButton* btn = static_cast<TogButton*>(data);
		btn->position(btn->x(), btn->Y - 5);
		btn->redraw();
		if (btn->parent()) btn->parent()->parent()->redraw();
	}

	int handle(int event) override
	{
		switch (event)
		{
		case FL_PUSH:
		{
			if (click)
			{
				Mix_PlayChannel(-1, click, 0);
			}
			return Fl_Button::handle(event);
		}
		case FL_ENTER:
		{
			if (hover)
			{
				Mix_PlayChannel(-1, hover, 0);
			}
			inFocus = true;
			ShowSign(this);
			Fl::add_timeout(0.05, delay, this);
			redraw();
			if (parent()) { parent()->parent()->redraw(); }
			return 1;
		}
		case FL_LEAVE:
		{
			inFocus = false;
			ShowSign(this);
			Fl::remove_timeout(delay, this);
			position(x(), Y);
			redraw();
			if (parent()) { parent()->parent()->redraw(); }
			return 1;
		}
		}

		return Fl_Button::handle(event);
	}

	~TogButton()
	{
		if (click)
		{
			Mix_FreeChunk(click);
		}
		if (hover)
		{
			Mix_FreeChunk(hover);
		}
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
private:
	Mix_Chunk* click = nullptr;
	Mix_Chunk* hover = nullptr;

public:
	menuBut(int X, int Y, int W, int H, const char* L = 0)
		: MyButton(X, Y, W, H, L) 
	{
		click = Mix_LoadWAV("sounds/on_click.wav");
		if (!click) {
			cerr << "Звук не найден: " << Mix_GetError() << endl;
		}

		hover = Mix_LoadWAV("sounds/on_aim.wav");
		if (!hover) {
			cerr << "Звук не найден: " << Mix_GetError() << endl;
		}
	}

	//деструктор - здесь для того, чтобы освободить место от звука
	~menuBut()
	{
		if (click) {
			Mix_FreeChunk(click);
		}
		if (hover) {
			Mix_FreeChunk(hover);
		}
	}

	int handle(int event)
	{ 
		switch (event)
		{
		case FL_PUSH:
		{
			if (click) {
				Mix_PlayChannel(-1, click, 0);
			}
			
		}break;
		case FL_ENTER:
		{
			if (hover) {
				Mix_PlayChannel(-1, hover, 0);
			}
		}break;
		}
		return MyButton::handle(event);
	}
};

//выезжающие тексты
class BoxForBut : public Fl_Box
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
private:
	Fl_Color startColor = fl_rgb_color(169, 169, 169);
	Fl_Color endColor = fl_rgb_color(237, 55, 55);

	Mix_Chunk* sound = nullptr;
public:
	PlayBut(int X, int Y, int W, int H, const char* L = 0)
		: MyButton(X, Y, W, H, L) 
	{
		sound = Mix_LoadWAV("sounds/pbip.wav");
		if (!sound)
		{
			cerr << "Звук не найден: " << Mix_GetError() << endl;
		}
	}
	~PlayBut()
	{
		if (sound)
		{
			Mix_FreeChunk(sound);
		}
	}

	float Progress = 0.0f;
	bool anim = false;
	bool icreasing = false;

	int handle(int event)
	{
		switch (event)
		{
		case FL_PUSH:
		{
			alertf(this);

			return MyButton::handle(event);
		}break;
		case FL_ENTER:
		{
			startAnim(true);

			if (sound)
			{
				Mix_PlayChannel(-1, sound, 0);
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

//кнопка для текста
class ChangedT : public Fl_Box
{
public:
	ChangedT(int X, int Y, int W, int H, const char* L = 0)
		:Fl_Box(X, Y, W, H, L)
	{
		labelsize(24);
		align(FL_ALIGN_INSIDE | FL_ALIGN_LEFT);
	}
};

class hackBut : public TogButton
{
public:
	hackBut(int X, int Y, int W, int H, const char* L = 0)
		:TogButton(X, Y, W, H, L)
	{

	}

	int handle(int event)
	{
		switch (event)
		{
		case FL_PUSH:
		{
			if (GData.isHackAct == 0)
			{
				GData.isHackAct = 1;
				color(fl_rgb_color(255, 128, 64));
			}
			else
			{
				GData.isHackAct = 0;
				reset_state();
			}
		}break;
		}
		return TogButton::handle(event);
	}
};

//кнопка игрового поля
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
	
	int handle(int event)
	{
		switch (event)
		{
		case FL_PUSH:
		{
			if (Fl::event_button() == 3)
			{
				if (box() != FL_DOWN_BOX && (label() == "" || label() == "*"))
				{
					if (color() == fl_rgb_color(247, 132, 25))//цвет для смены
					{
						color(fl_rgb_color(233, 240, 234));
					}
					else if (flags_count() < levels[GData.level - 1].mines_count)
					{
						color(fl_rgb_color(247, 132, 25));
					}
					counts_redraw();
					winOrFail();

				}
				
			}
		}break;
		}

		return Fl_Button::handle(event);
	}

	void reset_state()
	{
		color(fl_rgb_color(233, 240, 234));
		box(FL_UP_BOX);
		labelcolor(FL_BLACK);
		label("");
	}
};

//общие элементы сложностей
struct levels_w
{
	BoxForBut* UnG1;
	BoxForBut* UnG2;

	menuBut* toMenu;
	menuBut* again;

	ChangedT* cur_t;
	ChangedT* cur_m;

	BoxForBut* alert;
};

struct widgets
{
	hackBut* hBut;
};

widgets widget;

//глобальные переменные для игры
PGBut*** BField;

levels_w l_widget;
short moves = 0;
bool loose = false;
int game_level = 0;

//окошко с информацией(глоб. перемнные нужны потому, что они стираются при переходе между функциями)
string GTf, GMf;
string Mfound; string Mleft; string complT; string buf;

ChangedT* foundB; ChangedT* leftB;
ChangedT* complexity;

//массивы для работы функций проверки
short dx[] = { -1, -1, -1, 0, 0, 1, 1, 1 };
short dy[] = { -1, 0, 1, -1, 1, -1, 0, 1 };

short dx1[] = { -1, 0, 0, 1 };
short dy1[] = { 0,  -1, 1, 0 };


int main(int argc, char** argv)
{
	//настройка звука приложения
	/*WORD vl = (WORD)(0xFFFF);
	DWORD volume = MAKELONG(vl, vl);
	waveOutSetVolume(NULL, volume);*/

	//ЕСЛИ НОВЫЕ ВИДЖЕТЫ НЕ ОТРИСОВЫВАЮТСЯ, НУЖНО ПРОВЕРИТЬ ФУНКЦИИ. ГДЕ-ТО ОНИ МОГУТ ЯВНО ПРИВОДИТЬСЯ К НЕВЕРНОМУ КЛАССУ!!!

	 /*Инициализация SDL с поддержкой аудио*/
	if (SDL_Init(SDL_INIT_AUDIO) < 0) {
		std::cerr << "SDL_Init failed: " << SDL_GetError() << std::endl;
		return 1;
	}
	 //Инициализация SDL_mixer
	if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) {
		std::cerr << "Mix_OpenAudio failed: " << Mix_GetError() << std::endl;
		SDL_Quit();
		return 1;
	}
	Mix_Chunk* sound = Mix_LoadWAV("sounds/on_click.wav");
	if (!sound) {
		std::cerr << "Failed to load sound: " << Mix_GetError() << std::endl;
		Mix_CloseAudio();
		SDL_Quit();
		return 1;
	}

	HideConsole();
	SetConsoleOutputCP(CP_UTF8); SetConsoleCP(CP_UTF8); srand(time(NULL));
	Fl_Double_Window win(1000, 600, "Boomber");
	GData.win = &win; GData.GTime = 0;

	win.color(fl_rgb_color(192, 192, 192));

	//группа приветственного меню (0)
	Fl_Group* helloWin = new Fl_Group(0, 0, 1000, 600);
	menuBut begin(435.5, 250, 125, 60, "Начать");
	begin.callback(toGameMenu, helloWin->parent());
	helloWin->end();
	menues.hello = helloWin;

	//группа игрового меню (1)
	Fl_Group* mainMenu = new Fl_Group(0, 0, 1000, 600);

	menuBut start(100, 150, 150, 75, "Начать игру");
	menuBut rules(100, 250, 150, 75, "Правила");
	menuBut exit(100, 350, 150, 75, "Выход");

	BoxForBut Tstart(300, -32, 291, 32, "Перейти к настройке сложности");
	BoxForBut Trules(1000, 272, 291, 32, "Ознакомиться с правилами игры");
	BoxForBut Texit(300, 600, 291, 32, "Закрыть приложение");

	BoxForBut version(860, 580, 140, 20, "Версия: alpha0.3");
	version.box(FL_NO_BOX);

	rules.callback(toGameRule, &win);
	start.callback(toGameSettings, &win);
	exit.callback(exitf, nullptr);
	mainMenu->hide();
	mainMenu->end();
	menues.main = mainMenu;

	//меню правил (n)
	Fl_Group* rules_settings = new Fl_Group(0, 0, 1000, 600);
	rules_settings->begin();


	Fl_Box* background = new Fl_Box(0, 0, 1000, 600);
	background->color(fl_rgb_color(160, 160, 160));  // Цвет фона
	background->box(FL_FLAT_BOX);  // Стиль без рамки

	BoxForBut nigger(50, 80, 900, 600);
	BoxForBut nigger2(350, 15, 295, 55, "Правила");
	TogButton backk(75, 520, 140, 70, "Выход");
	TogButton next(785, 520, 140, 70, "->");

	backk.callback();
	backk.box(FL_FLAT_BOX);
	backk.color(fl_rgb_color(170, 170, 170));
	next.callback();
	next.box(FL_FLAT_BOX);
	next.color(fl_rgb_color(170, 170, 170));


	nigger.color(fl_rgb_color(192, 192, 192));
	nigger2.align(FL_ALIGN_CENTER);
	nigger2.labelsize(40);
	nigger2.color(fl_rgb_color(180, 180, 180));

	rules_settings->end();
	rules_settings->hide();

	menues.rules = rules_settings;


	//Группа настройки игры(2)
	Fl_Group* game_settings = new Fl_Group(0, 0, 1000, 600);

	TogButton easy(175, 150, 150, 70, "Легко");
	TogButton normal(425, 150, 150, 70, "Нормально");
	TogButton hard(675, 150, 150, 70, "Сложно");
	menuBut back(15, 525, 125, 60, "Назад");
	 
	PlayBut play(359, 353, 184, 76, "Играть");

	BoxForBut Teasy(166, -56, 166, 56, "Размер поля: 10х10\nКол-во мин: 15");
	BoxForBut Tnormal(416, -56, 166, 56, "Размер поля: 15х15\nКол-во мин: 35");
	BoxForBut Thard(666, -56, 166, 56, "Размер поля: 38х19\nКол-во мин:100");
	BoxForBut Thack(300, 600, 400, 56, "Включает отображение мин до их открытия");

	BoxForBut alert(200, -88, 600, 85, "Пожалуйста, выбирете сложность!");
	alert.box(FL_GLEAM_THIN_DOWN_BOX);
	alert.labelsize(36);
	alert.color(fl_rgb_color(237, 55, 55));
	l_widget.alert = &alert;

	hackBut hack(555, 353, 86, 76, "Чит");
	widget.hBut = &hack;

	play.callback(Game, nullptr);
	back.callback(toGameMenu, &win);
	easy.callback(choose_level, &win);
	normal.callback(choose_level, &win);
	hard.callback(choose_level, &win);
	game_settings->end();
	game_settings->hide();
	menues.settings = game_settings;


	BoxForBut* et1 = nullptr;/* (620, 37, 350, 60);*/
	l_widget.UnG1 = et1;

	BoxForBut* et2 = nullptr;/* (620, 131, 350, 246);*/
	l_widget.UnG1 = et2;

	menuBut* againLvl = nullptr;
	l_widget.again = againLvl;

	menuBut* leaveLvl = nullptr;
	l_widget.toMenu = leaveLvl;

	ChangedT* cur_time = nullptr;
	l_widget.cur_t = cur_time;

	ChangedT* cur_steps = nullptr;
	l_widget.cur_m = cur_steps;

	//группа легкой сложности (3)
	Fl_Group* es_g = new Fl_Group(0, 0, 1000, 600);

	es_g->end();
	es_g->hide();
	menues.easy = es_g;

	//группа нормальной сложности (4)
	Fl_Group* nor_g = new Fl_Group(0, 0, 1000, 600);

	nor_g->end();
	nor_g->hide();
	menues.normal = nor_g;

	//группа сложной сложности (5)
	Fl_Group* har_g = new Fl_Group(0, 0, 1000, 600);

	har_g->end();
	har_g->hide();
	menues.hard = har_g;

	//экран победы
	Fl_Group* wScreen1 = new Fl_Group(10, 213, 990, 174);

	Fl_Box bckgrW1(10, 213, 580, 174);
	bckgrW1.color(fl_rgb_color(192, 192, 192));
	bckgrW1.box(FL_FLAT_BOX);

	Fl_Box tgrW1(10, 220, 580, 159);
	tgrW1.color(fl_rgb_color(169, 169, 169));
	tgrW1.box(FL_FLAT_BOX);

	Fl_Box tgrTextW1(30, 254, 539, 58, "Все мины найдены");
	tgrTextW1.labelcolor(fl_rgb_color(21, 133, 4));
	tgrTextW1.labelsize(48);

	wScreen1->hide();
	wScreen1->end();
	screens.gw1 = wScreen1;	

	//экран поражения
	Fl_Group* lScreen1 = new Fl_Group(10, 213, 990, 174);

	Fl_Box bckgrL1(10, 213, 570, 174);
	bckgrL1.color(fl_rgb_color(192, 192, 192));
	bckgrL1.box(FL_FLAT_BOX);

	Fl_Box tgrL1(10, 220, 570, 159);
	tgrL1.color(fl_rgb_color(169, 169, 169));
	tgrL1.box(FL_FLAT_BOX);

	Fl_Box tgrTextL1(30, 254, 535, 58, "Открыта мина");
	tgrTextL1.labelcolor(fl_rgb_color(113, 0, 0));
	tgrTextL1.labelsize(48);

	Fl_Box tgrTextL2(30, 330, 535, 58, "Проигрыш");
	tgrTextL2.labelsize(24);

	lScreen1->hide();
	lScreen1->end();
	screens.gl1 = lScreen1;

	HICON hIcon = LoadIcon(GetModuleHandle(NULL), MAKEINTRESOURCE(101));
	win.icon((char*)hIcon);

	win.end();
	win.show(argc, argv);
	// Запуск событийного цикла FLTK
	int ret = Fl::run();

	// Освобождение ресурсов
	Mix_CloseAudio();
	SDL_Quit();

	return ret;
}

void toGameMenu(Fl_Widget* w, void* data)
{
	Fl_Double_Window* win = (Fl_Double_Window*)data;
	menuBut* self = (menuBut*)w;
	self->reset_state();

	//обновление переменных
	game_level = 0;

	if (menues.hello->visible())
	{
		menues.hello->hide();
		menues.main->show();
	}
	else if (menues.settings->visible())
	{
		menues.settings->hide();
		menues.main->show();
		choose_level(w, data);
	}
	else if (menues.easy->visible())
	{
		menues.easy->hide();
		menues.main->show();
		//again(nullptr, nullptr);
		choose_level(w, data);
	}
	else if (menues.normal->visible())
	{
		menues.normal->hide();
		menues.main->show();
		//again(nullptr, nullptr);
		choose_level(w, data);
	}
	else if (menues.hard->visible())
	{
		menues.hard->hide();
		menues.main->show();
		//again(nullptr, nullptr);
		choose_level(w, data);
	}
}

void toGameRule(Fl_Widget* w, void* data)
{
	Fl_Double_Window* win = (Fl_Double_Window*)data;

	menues.main->hide();
	menues.rules->show();
}

void exitf(Fl_Widget* w, void* data)
{
	exit(0);
}

void toGameSettings(Fl_Widget* w, void* data)
{
	Fl_Double_Window* win = (Fl_Double_Window*)data;
	TogButton* TogBut = NULL;
	menuBut* self = (menuBut*)w;
	self->reset_state();

	for (short i = 0; i < 3; i++)
	{
		TogBut = (TogButton*)menues.settings->child(i);
		TogBut->reset_state();
	}
	menues.main->hide();
	menues.settings->show();
}

void choose_level(Fl_Widget* w, void* data)
{
	Fl_Double_Window* win = (Fl_Double_Window*)data;

	for (short i = 0; i < 3; i++)
	{
		menues.settings->child(i)->color(fl_rgb_color(169, 169, 169));
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
	menues.settings->hide();
	PlayBut* self = (PlayBut*)w;
	self->icreasing = false;
	self->reset_state();

	if (game_level == 0)
	{
		menues.settings->show();
		return;
	}

	drowField();

	if (game_level == 1)
	{
		menues.easy->show();
		
	}
	else if (game_level == 2)
	{
		menues.normal->show();
	}
	else if (game_level == 3)
	{
		menues.hard->show();
	}
}

void findXY()
{
	for (short i = 0; i < levels[GData.level - 1].rows; i++)
	{
		for (short j = 0; j < levels[GData.level - 1].cols; j++)
		{
			if (BField[i][j] == GData.pedBut)
			{
				GData.curX = i;
				GData.curY = j;
			}
		}
	}
}

void ButPressed(Fl_Widget* w, void* data)
{
	if (Fl::event_button() != 1) return;
	GData.pedBut = (PGBut*)w;

	findXY();
	if (moves != 0)
	{
		if (GData.opened[GData.curX][GData.curY] == false)
		{
			moves++;
			open(GData.curX, GData.curY); 
		}
	}
	else
	{
		moves++;
		initialize_field();

		place_mines(GData.field, levels[GData.level - 1].rows, levels[GData.level - 1].cols, levels[GData.level - 1].mines_count);
		calculate_numbers(GData.field, levels[GData.level - 1].rows, levels[GData.level - 1].cols);
		Fl::add_timeout(1.0, timer, nullptr);

		open(GData.curX, GData.curY);
	}
	GMf = "Ходов: " + to_string(moves);
	l_widget.cur_m->label(GMf.c_str());
}

void counts_redraw()
{
	short f = flags_count();

	short l = levels[GData.level - 1].mines_count - f;

	Mfound = "Мин найдено: " + to_string(f);
	Mleft = "Мин осталось: " + to_string(l);

	foundB->label(Mfound.c_str());
	leftB->label(Mleft.c_str());

	foundB->redraw();
	leftB->redraw();
	
}

short flags_count()
{
	short count = 0;
	for (int i = 0; i < levels[GData.level - 1].rows; i++)
	{
		for (int j = 0; j < levels[GData.level - 1].cols; j++)
		{
			if (BField[i][j]->color() == fl_rgb_color(247, 132, 25))
			{
				count++;
			}
		}
	}
	return count;
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
	else if (strcmp(but->label(), "Чит") == 0)
	{
		Fl_Widget* box = but->parent()->child(8);
		TogButton* tBut = (TogButton*)data;

		if (tBut->inFocus and box->y() > 544)
		{
			box->resize(box->x(), box->y() - 4, box->w(), box->h());
			box->redraw();
			box->parent()->parent()->redraw();

			Fl::add_timeout(0.005, ShowSign, data);
		}
		else if (!tBut->inFocus and box->y() < 600)
		{
			box->resize(box->x(), box->y() + 4, box->w(), box->h());
			box->redraw();
			box->parent()->parent()->redraw();

			Fl::add_timeout(0.005, ShowSign, data);
		}
	}
	

}

void alertf(void* data)
{
	Fl_Widget* but = (Fl_Widget*)data;
	static bool omit = true;

	if (strcmp(but->label(), "Играть") == 0)
	{
		Fl_Widget* box = but->parent()->child(9);
		PlayBut* tBut = (PlayBut*)data;

		if (box->y() < -3 and omit and game_level == 0)
		{
			box->resize(box->x(), box->y() + 5, box->w(), box->h());
			box->redraw();
			box->parent()->parent()->redraw();
			if (box->y() == -3)
			{
				omit = false;
				Fl::add_timeout(1, alertf, data);
				return;
			}
			

			Fl::add_timeout(0.005, alertf, data);
		}
		else if (box->y() > -88)
		{
			box->resize(box->x(), box->y() - 5, box->w(), box->h());
			box->redraw();
			box->parent()->parent()->redraw();
			if (box->y() == -88) { omit = true; return; }

			Fl::add_timeout(0.005, alertf, data);
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

void place_mines(char** _field, int rows, int cols, int mines_count)
{
	int mines_placed = 0;
	while (mines_placed < mines_count)
	{
		int x = rand() % rows;
		int y = rand() % cols;

		if (BField[x][y] == GData.pedBut or abs(x - GData.curX) <= 1 and abs(y - GData.curY) <= 1)
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
	for (int i = 0; i < cols * 3; i++)
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
	for (int i = 0; i < cols * 3; i++)
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

	for (int k = 0; k < 4; k++)
	{
		open_empty(_field, _opened, rows, cols, _row + dx1[k], _col + dy1[k]);
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
		BField[i][j]->color(FL_RED);
		loose = true;
		redraw();
	}
	else
	{
		GData.opened[i][j] = true;
		redraw();
	}

	winOrFail();
	return 1;
}

void winOrFail()
{
	if (loose == true)
	{
		Fl::remove_timeout(timer);
		screens.gl1->show();
		showField();
		return;
	}

	short count = 0;
	for (int i = 0; i < levels[GData.level - 1].rows; i++)
	{
		for (int j = 0; j < levels[GData.level - 1].cols; j++)
		{
			if (!GData.field[i][j])return;
			if (BField[i][j]->color() == fl_rgb_color(247, 132, 25) and  GData.field[i][j] == '*')
			{
				count++;
			}
		}
	}

	if (count >= levels[GData.level - 1].mines_count)
	{
		screens.gw1->show();
		Fl::remove_timeout(timer);
	}
}

void showField()
{
	for (short i = 0; i < levels[GData.level - 1].rows; i++)
	{
		for (short j = 0; j < levels[GData.level - 1].cols; j++)
		{
			if (GData.curX == i and GData.curY == j) continue;
			GData.opened[i][j] = true;
		}
	}
	redraw();
}

//тут подключаются читы
void redraw()
{
	for (short i = 0; i < levels[GData.level-1].rows; i++)
	{
		for (short j = 0; j < levels[GData.level-1].cols; j++)
		{
			if (GData.isHackAct == 1)
			{
				if (GData.field[i][j] == '*') BField[i][j]->label("*");
			}

			if (GData.opened[i][j] == true)
			{
				if(GData.field[i][j] == '0')
				{
					BField[i][j]->box(FL_DOWN_BOX);
					BField[i][j]->color(fl_rgb_color(186, 186, 186));
				}
				else if (GData.field[i][j] == '*')
				{
					BField[i][j]->box(FL_DOWN_BOX);
					BField[i][j]->label("*");
				}
				else
				{
					if (GData.field[i][j] == '1')
					{
						BField[i][j]->label("1");
						BField[i][j]->labelcolor(fl_rgb_color(27, 23, 255));
					}
					else if (GData.field[i][j] == '2')
					{
						BField[i][j]->label("2");
						BField[i][j]->labelcolor(fl_rgb_color(27, 158, 49));
					}
					else if (GData.field[i][j] == '3')
					{
						BField[i][j]->label("3");
						BField[i][j]->labelcolor(fl_rgb_color(156, 76, 30));
					}
					else if (GData.field[i][j] == '4')
					{
						BField[i][j]->label("4");
						BField[i][j]->labelcolor(fl_rgb_color(191, 11, 11));
					}
					else if (GData.field[i][j] == '5')
					{
						BField[i][j]->label("5");
						BField[i][j]->labelcolor(fl_rgb_color(219, 2, 132));
					}
					else if (GData.field[i][j] == '6')
					{
						BField[i][j]->label("6");
						BField[i][j]->labelcolor(fl_rgb_color(255, 195, 43));
					}
					else if (GData.field[i][j] == '7')
					{
						BField[i][j]->label("7");
						BField[i][j]->labelcolor(fl_rgb_color(255, 66, 66));
					}
					else if (GData.field[i][j] == '8')
					{
						BField[i][j]->label("8");
						BField[i][j]->labelcolor(fl_rgb_color(255, 0, 0));
					}
					else
					{
						BField[i][j]->label("X");
					}
					BField[i][j]->color(fl_rgb_color(233, 240, 234));
				}
				BField[i][j]->redraw();
			}
		}
	}
	counts_redraw();
	GData.win->redraw();
}

void again(Fl_Widget* w, void* data)
{
	for (short i = 0; i < levels[GData.level - 1].rows; i++)
	{
		for (short j = 0; j < levels[GData.level - 1].cols; j++)
		{
			BField[i][j]->reset_state();
		}
	}
}

void drowField()
{
	//удалить, если уже объявлено
	if (foundB)
	{
		delete foundB;
		delete leftB;

		delete complexity;

		delete l_widget.cur_m;
		delete l_widget.cur_t;
	}

	short x, y, sx = NULL, sy = NULL, subx = NULL, suby = NULL;
	BField = new PGBut **[30];
	for (short i = 0; i < levels[GData.level - 1].rows; i++)
	{
		BField[i] = new PGBut * [levels[GData.level - 1].cols];
	}
	
	Fl_Group* gr = nullptr;

	if (game_level == 1)
	{
		gr = menues.easy;
		sx = 58; sy = 58;
		subx = 10; suby = 10;
	}
	else if (game_level == 2)
	{
		gr = menues.normal;
		sx = 38.1; sy = 38.6;
		subx = 10; suby = 10;
	}
	else if (game_level == 3)
	{
		gr = menues.hard;
		sx = sy = 27;
		subx = 14; suby = 7;
	}

	y = suby;
	for (short i = 0; i < levels[GData.level - 1].rows; i++)
	{
		x = subx;
		for (short j = 0; j < levels[GData.level - 1].cols; j++)
		{
			BField[i][j] = new PGBut(x, y, sx, sy, "");
			BField[i][j] = BField[i][j];
			BField[i][j]->callback(ButPressed, BField);
			gr->add(BField[i][j]);
			
			x += sx;
		}
		y += sy;
	}

	if (game_level == 1 || game_level == 3) x = 580, y = 549;
	else x = 570, y = 539;

	screens.gl1->child(0)->resize(10, 213, x, 174);
	screens.gl1->child(1)->resize(10, 220, x, 159);
	screens.gl1->child(2)->resize(30, 254, y, 58);
	screens.gl1->child(3)->resize(30, 330, y, 58);

	screens.gw1->child(0)->resize(10, 213, x, 174);
	screens.gw1->child(1)->resize(10, 220, x, 159);
	screens.gw1->child(2)->resize(30, 254, y, 58);

	if (game_level == 1)
	{

		foundB = new ChangedT(635, 190, 211, 29, "Мин найдено: 0");
		leftB = new ChangedT(635, 219, 211, 29, "Мин осталось: 15");

		l_widget.again = new menuBut(814, 510, 169, 63, "Заново");
		l_widget.toMenu = new menuBut(606, 510, 169, 63, "В меню");
		l_widget.cur_t = new ChangedT(650, 37, 175, 60, "Время: 0");
		l_widget.cur_m = new ChangedT(830, 37, 175, 60, "Ходов: 0");
		l_widget.UnG1 = new BoxForBut(620, 37, 350, 60);
		l_widget.UnG2 = new BoxForBut(620, 131, 350, 246);

		menues.easy->add(l_widget.UnG1);
		menues.easy->add(l_widget.UnG2);
		menues.easy->add(l_widget.again);
		menues.easy->add(l_widget.toMenu);
		menues.easy->add(l_widget.cur_t);
		menues.easy->add(l_widget.cur_m);

		menues.easy->add(foundB);
		menues.easy->add(leftB);

		complexity = new ChangedT(670, 141, 240, 29, "Сложность: легкая");
		complexity->align(FL_ALIGN_INSIDE);
		menues.easy->add(complexity);

		menues.easy->add(screens.gl1);
		menues.easy->add(screens.gw1);
	}
	else if (game_level == 2)
	{
		foundB = new ChangedT(635, 190, 211, 29, "Мин найдено: 0");
		leftB = new ChangedT(635, 219, 211, 29, "Мин осталось: 35");
		
		l_widget.again = new menuBut(814, 510, 169, 63, "Заново");
		l_widget.toMenu = new menuBut(606, 510, 169, 63, "В меню");
		l_widget.cur_t = new ChangedT(650, 37, 175, 60, "Время: 0");
		l_widget.cur_m = new ChangedT(830, 37, 175, 60, "Ходов: 0");
		l_widget.UnG1 = new BoxForBut(620, 37, 350, 60);
		l_widget.UnG2 = new BoxForBut(620, 131, 350, 246);

		menues.normal->add(l_widget.UnG1);
		menues.normal->add(l_widget.UnG2);
		menues.normal->add(l_widget.cur_t);
		menues.normal->add(l_widget.cur_m);
		menues.normal->add(l_widget.again);
		menues.normal->add(l_widget.toMenu);

		menues.normal->add(foundB);
		menues.normal->add(leftB);

		complexity = new ChangedT(670, 141, 240, 29, "Сложность: нормальная");
		complexity->align(FL_ALIGN_INSIDE);
		menues.normal->add(complexity);

		menues.normal->add(screens.gl1);
		menues.normal->add(screens.gw1);
	}
	else if (game_level == 3)
	{
		foundB = new ChangedT(157, 548, 169, 26, "Мин найдено: 0");
		foundB->labelsize(20);
		leftB = new ChangedT(658, 548, 178, 26, "Мин осталось: 100");
		leftB->labelsize(20);

		l_widget.again = new menuBut(864, 531, 127, 57, "Заново");
		l_widget.again->labelsize(20);

		l_widget.toMenu = new menuBut(9, 531, 127, 57, "В меню");
		l_widget.toMenu->labelsize(20);

		l_widget.cur_t = new ChangedT(361, 548, 159, 26, "Время: 0");
		l_widget.cur_t->labelsize(20);

		l_widget.cur_m = new ChangedT(505, 548, 123, 26, "Ходов: 0");
		l_widget.cur_m->labelsize(20);

		l_widget.UnG1 = new BoxForBut(149, 533, 702, 53);

		screens.gl1->child(0)->resize(10, 213, 980, 174); 
		screens.gl1->child(1)->resize(10, 220, 975, 159);
		screens.gl1->child(2)->resize(30, 254, 975, 58);
		screens.gl1->child(3)->resize(30, 330, 980, 58);

		screens.gw1->child(0)->resize(10, 213, 980, 174);
		screens.gw1->child(1)->resize(10, 220, 975, 159);
		screens.gw1->child(2)->resize(30, 254, 975, 58);

		menues.hard->add(l_widget.UnG1);
		menues.hard->add(l_widget.cur_t);
		menues.hard->add(l_widget.cur_m);
		menues.hard->add(l_widget.again);
		menues.hard->add(l_widget.toMenu);

		menues.hard->add(foundB);
		menues.hard->add(leftB);

		complexity = new ChangedT(670, 141, 240, 29, "Сложность: сложно");

		menues.hard->add(screens.gl1);
		menues.hard->add(screens.gw1);
	}
	l_widget.again->callback(dock, GData.win);
	l_widget.toMenu->callback(dock, nullptr);
}

void timer(void* data)
{
	GData.GTime++;

	GTf = "Время: " + to_string(GData.GTime);
	l_widget.cur_t->label(GTf.c_str());

	Fl::repeat_timeout(1.0, timer, nullptr);
}

//функция, распределяющая кнопки(нужна, чтобы удобно обновлять переменные между играми)
void dock(Fl_Widget* w, void* data)
{
	//сбросить переменные
	moves = 0; loose = false;
	GData.GTime = 0;
	GData.isHackAct = 0;

	//обновить окна
	screens.gl1->hide();
	screens.gw1->hide();
	widget.hBut->reset_state();

	l_widget.cur_t->redraw(); //таймер
	l_widget.cur_m->label("Ходов: 0");
	foundB->label("Мин найдено: 0");
	Mleft = "Мин осталось: " + to_string(levels[GData.level - 1].mines_count);
	leftB->label(Mleft.c_str());

	//обновить время
	Fl::remove_timeout(timer);
	GTf = "Время: 0";

	//В другие функции
	if (w == nullptr) return;
	if (strcmp(w->label(), "Заново") == 0)
	{
		again(w, nullptr);
	}
	else if ((strcmp(w->label(), "В меню") == 0))
	{
		//перед выходом в меню, удалить старое игровое поле
		if (game_level != 0)
		{
			//удалить, если уже объявлено
			if (BField)
			{
				for (short i = 0; i < levels[GData.level - 1].rows; i++)
				{
					for (short j = 0; j < levels[GData.level - 1].cols; j++)
					{
						delete BField[i][j];
					}
					delete[] BField[i];
				}
				delete[] BField;
			}
		}

		toGameMenu(w, GData.win);
	}
}
