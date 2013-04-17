#include "bitfile.hpp"
#include <cstdio>

int main()
{
	bitfile bfin;
	bitfile bfout;
	bfin.open("", READ);
	bfout.open("", WRITE);
	bfin.info();
	bfout.info();
	return 0;
}

