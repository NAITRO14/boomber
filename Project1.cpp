#include <FL/Fl.H>

#include <FL/Fl_Window.H>

#include <FL/Fl_Box.H>

#include <FL/Fl_Button.H>

using namespace std;

int main(int argc, char** argv)
{
	SetConsoleOutputCP(CP_UTF8); SetConsoleCP(CP_UTF8); // для русского текста в консоли|для работы текста в окне приложения, в свойствах проекта указать /utf-8 и сохранить в кодировке юникод

	Fl_Window win(800, 600, "Главное окно");
	win.end();

	win.show(argc, argv);
	return Fl::run();



}