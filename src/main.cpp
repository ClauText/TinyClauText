
#include <iostream>
#include "ClauText.h"



int main(void)
{
	{ // test1
		wiz::UserType ut;

		wiz::LoadDataFromString("x=1 y= 2", ut);

		std::cout << ut.ToString() << "\n";
	}
	{ //test2 
		wiz::UserType ut;

		wiz::LoadDataFromFile("input.txt", ut);

		std::cout << ut.ToString() << "\n";
	}

	{ // test3
		wiz::ClauText clautext;

		wiz::UserType global;

		wiz::LoadDataFromString("Main = { $call = { id = 0 } } Event = { id = 0 } ", global);

		wiz::Option opt;
		{
			int a = clock();
			std::string result = clauText.excute_module("", &global, wiz::ExcuteData(), opt, 0);
			int b = clock();
			std::cout << "excute result is " << result << "\n";
			std::cout << b - a << "ms" << "\n"; //
		}
	}

	return 0;
}

