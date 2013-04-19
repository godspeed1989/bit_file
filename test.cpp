#include "bitfile.hpp"
#include <cstdio>
#include <cstring>
#include <ctime>
//#define DEBUG
#ifdef DEBUG
void printByte(const u8 byte)
{
	printf("%02x ", byte);
	for(int j=7; j>=0; j--)
	{
		if(byte & (1<<j))
			printf("1");
		else
			printf("0");
		if(j==4)
			printf("_");
	}
}
void printBits(const u8* ptr, u32 nbits)
{
	u32 bytes, bits;
	bytes = nbits >> 3;
	bits = nbits % 8;
	for(u32 i=0; i<bytes; i++)
	{
		printByte(ptr[i]);
		if((i+1)%4 == 0 && i != 0)
			printf("\n");
		else
			printf("\t");
	}
	if(bits != 0)
	{
		u8 byte;
		byte = ptr[bytes];
		byte &= (0xFF >> (8-bits));
		printByte(byte);
	}
	printf("\n%d bits = %d * 8 + %d\n", nbits, nbits >> 3, nbits % 8);
}
#endif

void file_test(const char * file)
{
	bitfile bfin;
	bitfile bfout;
	static const u32 buf_size = 256;
	static u8 buffer[buf_size];
	static char filename[128];
	if(bfin.open(file, READ)) // open read file bfin
	{
		printf("File [%s] open error\n", file);
		return;
	}
	strcpy(filename, file);
	strcat(filename, ".out");
	bfout.open(filename, WRITE); // open write file bfout
	bfin.info();
	bfout.info();
	u32 sum = 0;
	srand((unsigned int)time(NULL));
	while(!bfin.eof())
	{
		u32 rbits, read, write;
		rbits = rand() % 16;
		read = bfin.readb(buffer, rbits);
#ifdef DEBUG
		printBits(buffer, read);
		getchar();
#endif
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
}

void stdstr_test()
{
	bitfile sfin;
	bitfile sfout;
	static const u32 buf_size = 256;
	static u8 buffer[buf_size];
	srand((unsigned int)time(NULL));
	for(u32 i=0; i < buf_size; i++)
	{
		buffer[i] = rand()%255;
	}
	sfin.open("str_in", buffer, buf_size);
	sfin.capb = (rand() % buf_size)*8 + rand() % 8;
	sfout.open("str_out", WRITE);
	sfin.info();
	sfout.info();
	u32 sum = 0;
	srand((unsigned int)time(NULL));
	while(!sfin.eof())
	{
		u32 rbits, read, write;
		rbits = rand() % 16;
		read = sfin.readb(buffer, rbits);
#ifdef DEBUG
		printBits(buffer, read);
		getchar();
#endif
		write = sfout.writeb(buffer, read);
		if(read != write)
		{
			printf("Error: Read %d and Write %d\n", read, write);
			break;
		}
		sum += read;
	}
	printf("Total %d bits\n", sum);
	sfin.info();
	sfout.info();
	if(sfin.sizeb() != sfout.sizeb())
		printf("Error: size not match.\n");
	for(u32 i=0; i<sfin.sizeB() && i<sfout.sizeB(); i++)
	{
		if(sfin.data[i] != sfout.data[i])
		{
			printf("Error: data [%d] not match.\n", i);
			break;
		}
	}
}

int main(int argc, char* argv[])
{
	clock_t start;

	printf("***Stdstr read and write test......\n");
	start = clock();
	stdstr_test();
	printf("Finish. Time %1.2lf sec.\n", double(clock()-start)/CLOCKS_PER_SEC);

	if(argc < 2)
		return 1;
	printf("***File read and write test......\n");
	start = clock();
	file_test(argv[1]);
	printf("Finish. Time %1.2lf sec.\n", double(clock()-start)/CLOCKS_PER_SEC);

	return 0;
}

