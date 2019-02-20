#include <iostream>
#include <fstream>
#include <string>

constexpr auto WIDTH = 10;
constexpr auto HEIGHT = 10;

int main(void)
{
	char var = 65;
	std::ofstream ofs;

	std::cout << "Initial code." << std::endl;

	ofs.open("picture.ppm");
	// don't use \n as ending white space, because of windows
	ofs << "P6\n" << WIDTH << " " << HEIGHT << "\n255 ";


	for (int i = 0; i < WIDTH * HEIGHT; ++i)
	{
		char r = 255 * ((i%3) == 0 ? 1 : 0);
		char g = 255 * ((i%3) == 1 ? 1 : 0);
		char b = 255 * ((i%3) == 2 ? 1 : 0);

		ofs << r <<  g  << b;
	}

	ofs.close();

	std::cout << "Done creating image." << std::endl;

	//std::cout << (int)var << std::endl;
	//getchar();
	
	return 0;
}