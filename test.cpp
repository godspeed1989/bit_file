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

void stdstr_test()
{
	bitfile sfin;
	bitfile sfout;
	static const u32 buf_size = 256;
	static u8 buffer[buf_size];
	srand((unsigned int)time(NULL));
	for(u32 i=0; i < buf_size; i++)
	{
		buffer[i] = rand() % 255;
	}
	sfin.open("str_in", buffer, buf_size);
	sfin.capb = (rand() % buf_size) * 8;
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
	if(sfin.ftellb() != sfout.ftellb())
		printf("Error: size not match.\n");
	for(u32 i=0; i<sfin.ftellB() && i<sfout.ftellB(); i++)
	{
		if(sfin.data[i] != sfout.data[i])
		{
			printf("Error: data [%d] not match.\n", i);
			break;
		}
	}
}

void test_write2read()
{
	bitfile bfile;
	static const u32 buf_size = 256;
	static u8 buffer1[buf_size], buffer2[buf_size];
	srand((unsigned int)time(NULL));
	for(u32 i=0; i < buf_size; i++)
	{
		buffer1[i] = rand() % 255;
	}
	bfile.open("write_read", WRITE);
	bfile.writeB(buffer1, buf_size);
	bfile.write_to_read();
	bfile.readB(buffer2, buf_size);
	for(u32 i=0; i < buf_size; i++)
	{
		if(buffer1[i] != buffer2[i])
			printf("Error: data [%d] not match.\n", i);
	}
}

int main(int argc, char* argv[])
{
	clock_t start;

	printf("***Stdstr read and write test......\n");
	start = clock();
	stdstr_test();
	printf("Finished. Time %1.2lf sec.\n", double(clock()-start)/CLOCKS_PER_SEC);

	printf("***Write to read test......\n");
	start = clock();
	test_write2read();
	printf("Finished. Time %1.2lf sec.\n", double(clock()-start)/CLOCKS_PER_SEC);

	return 0;
}

