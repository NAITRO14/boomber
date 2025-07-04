#include <FL/Fl.H>

#include <FL/Fl_Window.H>

#include <FL/Fl_Box.H>

#include <FL/Fl_Button.H>

using namespace std;

int main(int argc, char** argv)
{
	Fl_Window win(800, 600, /*u8*/"Главное окно");
	win.end();

	win.show(argc, argv);
	return Fl::run();



}