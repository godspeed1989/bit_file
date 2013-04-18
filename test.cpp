#include "bitfile.hpp"
#include <cstdio>
#include <cstring>
#include <ctime>

int main(int argc, char* argv[])
{
	if(argc < 2)
		return 1;
	static const int buf_size = 256;
	static char buffer[buf_size];
	bitfile bfin;
	bitfile bfout;
	if(bfin.open(argv[1], READ))
		printf("File [%s] open error\n", argv[1]);
	strcpy(buffer, argv[1]);
	strcat(buffer, ".out");
	bfout.open(buffer, WRITE);
	bfin.info();
	bfout.info();
	u32 sum = 0;
	while(!bfin.eof())
	{
		u32 rbits, read, write;
		rbits = rand()%50;
		read = bfin.readb(buffer, rbits);
		write = bfout.writeb(buffer, read);
		if(read != write)
		{
			printf("Error: Read %d and Write %d", read, write);
			break;
		}
		sum += read;
	}
	printf("Total %d bits\n", sum);
	bfin.info();
	bfout.info();
	bfout.writeout();
	return 0;
}

