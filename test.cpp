#include "bitfile.hpp"
#include <cstdio>
#include <cstring>
#include <ctime>

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
	printf("\n");
}
void printBits(const u8* ptr, u32 nbits)
{
	u32 bytes, bits;
	bytes = nbits >> 3;
	bits = nbits % 8;
	for(u32 i=0; i<bytes; i++)
	{
		printByte(ptr[i]);
	}
	if(bits != 0)
	{
		u8 byte;
		byte = ptr[bytes];
		byte &= (0xFF >> (8-bits));
		printByte(byte);
	}
}

int main(int argc, char* argv[])
{
	if(argc < 2)
		return 1;
	static const int buf_size = 256;
	static u8 buffer[buf_size];
	static char filename[128];
	bitfile bfin;
	bitfile bfout;
	if(bfin.open(argv[1], READ))
		printf("File [%s] open error\n", argv[1]);
	strcpy(filename, argv[1]);
	strcat(filename, ".out");
	bfout.open(filename, WRITE);
	bfin.info();
	bfout.info();
	u32 sum = 0;
	while(!bfin.eof())
	{
		u32 rbits, read, write;
		rbits = rand()%128;
		read = bfin.readb(buffer, rbits);
		//printBits(buffer, read);
		//getchar();
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

