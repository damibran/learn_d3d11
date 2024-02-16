#include <iostream>

#include "Game.h"

int main()
{
	try
	{
		dmbrn::Game app{};
		app.run();
	}
	catch (const std::exception& e)
	{
		std::cerr << e.what() << std::endl;
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}
