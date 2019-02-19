#include <iostream>
#include <fstream>

constexpr auto WIDTH = 10;
constexpr auto HEIGHT = 10;

int main(void)
{
	char var;
	std::ofstream ofs;

	std::cout << "Initial code." << std::endl;

	ofs.open("picture.ppm");
	ofs << "P6\n" << WIDTH << "\n" << HEIGHT << "\n255\n";

	for (int i = 0; i < WIDTH * HEIGHT; ++i)
	{
		char r = 255 * ((i%3) == 0 ? 1 : 0);
		char g = 255 * ((i%3) == 1 ? 1 : 0);
		char b = 255 * ((i%3) == 2 ? 1 : 0);

		ofs << r << g << b;
	}

	ofs.close();

	std::cout << "Done creating image.";
	
	return 0;
}