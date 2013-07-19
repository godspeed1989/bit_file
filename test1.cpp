#include "bitfile.hpp"
#include <cstdio>
#include <cstring>
#include <ctime>

/*
 * profile the efficency
 */
void file_test(const char * file)
{
	bitfile bfin;
	bitfile bfout;
	static const u32 buf_size = 256;
	static u8 buffer[buf_size];
	static char filename[128];
	// open input file
	if(bfin.open(file, READ))
	{
		printf("File [%s] open error\n", file);
		return;
	}
	strcpy(filename, file);
	strcat(filename, ".out");
	// open output file
	bfout.open(filename, WRITE);
	bfin.info();
	bfout.info();
	// write from input file to output file
	u32 sum = 0;
	srand((unsigned int)time(NULL));
	while(!bfin.eof())
	{
		u32 rbits, read, write;
		rbits = rand() % 16;
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
	bfout.write_out();
}

int main(int argc, char* argv[])
{
	char * fname;
	if(argc < 2)
		fname = argv[0];
	else
		fname = argv[1];

	clock_t start;
	start = clock();
	file_test(fname);
	printf("Finished. Time %1.2lf sec.\n", double(clock()-start)/CLOCKS_PER_SEC);
	
	return 0;
}

