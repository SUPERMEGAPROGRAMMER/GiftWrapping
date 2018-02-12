#include "stdafx.h"
#include "Parser.h"
#include "GiftWrapping.h"

int main(int argc, char ** argv)
{
	if ((argc == 2) || (argc == 5))
	{
		if (atoi(argv[1]) < 0)
		{
			std::cerr << "Usage: GiftWrapping.exe b_random_seed [in_file.txt out_file.txt]" << std::endl;
			return EXIT_FAILURE;
		}

		bool b_random_seed = (atoi(argv[1]) > 0);

		Parser p(argv[2], argv[3], argv[4]);
		p.read();
		if (p.open_correct && p.parse_correct)
		{
			GiftWrapping gw(b_random_seed);
			gw.input_points = p.in_info;
			gw.run_algorithm();

			p.out_info.points = gw.scatter_points;
			p.out_info.faces = gw.convex_hull;

			p.write();

			return EXIT_SUCCESS;
		}
		else
			return EXIT_FAILURE;
	}
	else
	{
		std::cerr << "Usage: GiftWrapping.exe b_random_seed [in_file.txt out_file.txt]" << std::endl;
		return EXIT_FAILURE;
	}
}