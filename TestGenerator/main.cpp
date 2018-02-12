#include <string>

//#include "stdafx.h"

// -----
#include <iostream>
// -----

#include "GiftWrappingTest.h"


int main(int argc, char ** argv)   
{
// -----
	if (argc != 6)
	{
		std::cerr << "Usage: TestGenerator.exe <b_random_seed> <dimension> <num_of_interior_points> <test_id> <num_of_vertices>" << std::endl;
		return EXIT_FAILURE;
	}

	TestConfig cfg(	atoi(argv[1]), 
					atoi(argv[2]), 
					atoi(argv[3]), 
					atoi(argv[4]), 
					atoi(argv[5]));

	if (cfg.is_correct)
	{
		// -----
		GiftWrappingTest gw_test(cfg);
		gw_test.generate_test();
		// -----

		std::cout << "correct!!!" << std::endl;
		return EXIT_SUCCESS;
	}
	else
	{
		std::cerr << "Usage: TestGenerator.exe <b_random_seed> <dimension> <num_of_interior_points> <test_id> <num_of_vertices>" << std::endl;
		return EXIT_FAILURE;
	}
// -----

/*
	if ((argc == 1) || (argc == 3))
	{
		Parser p(argv[0], argv[1]);
		p.read();
		if (p.open_correct && p.parse_correct)
		{
			GiftWrappingTest gw_test;
			gw_test.generate_test(p.in_info);
			p.out_info = gw_test.result;
			p.write();

			return EXIT_SUCCESS;
		}
		else
			return EXIT_FAILURE;
	}
	else
	{
		std::cerr << "Usage: TestGenerator.exe in_file.txt out_file.txt" << std::endl;
		return EXIT_FAILURE;
	}
*/
}