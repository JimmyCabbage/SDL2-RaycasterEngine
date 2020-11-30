#include "window.h"
#include <iostream>

int main(int argc, char** argv)
{
	try
	{
		RaycastingWindow window;

		window.mainLoop();
	}
	catch (std::runtime_error& exp)
	{
		std::cerr << exp.what() << '\n';
		return 1;
	}

	return 0;
}