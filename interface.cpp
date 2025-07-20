#include <FL/Fl.H>
#include <FL/Fl_Window.H>
#include <FL/Fl_Box.H>
#include <FL/Fl_Button.H>

void ShowConsole() {
	::ShowWindow(::GetConsoleWindow(), SW_SHOW);
}

void HideConsole() {
	::ShowWindow(::GetConsoleWindow(), SW_HIDE);
}

void toGameMenu(Fl_Widget* w, void* data);
void exitf(Fl_Widget* w, void* data);

int main(int argc, char** argv)
{
	HideConsole();
	SetConsoleOutputCP(CP_UTF8); SetConsoleCP(CP_UTF8);
	Fl_Window win(1000, 600, "игра");

	//группа приветственного меню (0)
	Fl_Group* helloWin = new Fl_Group(0, 0, 1000, 600);
	Fl_Button begin(435.5, 250, 125, 60, "Начать");
	begin.callback(toGameMenu, helloWin->parent());
	helloWin->end();

	//группа игрового меню (1)
	Fl_Group* mainMenu = new Fl_Group(0, 0, 1000, 600);
	Fl_Button start(100, 150, 150, 75, "Начать игру");
	Fl_Button rules(100, 250, 150, 75, "Правила");
	Fl_Button exit(100, 350, 150, 75, "Выход");
	exit.callback(exitf, nullptr);

	mainMenu->hide();
	mainMenu->end();

	win.end();
	win.show(argc, argv);
	return Fl::run();
}

void toGameMenu(Fl_Widget* w, void* data)
{
	Fl_Group* win = (Fl_Group*)data;
	win->child(0)->hide();
	
	win->child(1)->show();
	


}

void exitf(Fl_Widget* w, void* data)
{
	exit(0);
}
