
#include <iostream>
#include "ClauText.h"



int main(void)
{
	{
		wiz::UserType ut;

		wiz::LoadDataFromString("x=1 y= 2", ut);

		std::cout << ut.ToString() << "\n";
	}
	{
		wiz::UserType ut;

		wiz::LoadDataFromFile("input.txt", ut);

		std::cout << ut.ToString() << "\n";
	}

	return 0;
}

