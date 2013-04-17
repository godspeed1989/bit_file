#ifndef __BITFILE_H__
#define __BITFILE_H__

#include <cstdio>
#include <cstring>
#include <cstdlib>

typedef unsigned char      u8;
typedef unsigned short     u16;
typedef unsigned int       u32;
typedef unsigned long long u64;

enum O_TYPE {READ, WRITE};
// 1. The bitfile only can be 'just READ' or 'just WRITE'
// 2. The whole class is little-endian, not matter byte or bit, that is,
//    read/write from low byte/bit to high byte/bit
// 3. file size can't exceed 512MB
typedef struct bitfile
{
	O_TYPE otype;
	char * name;// file name
	u8 * data;  // file data
	u32 sizeb;  // file size in BITS
	// file position in bits = pos_B<<3 + pos_b
	u32 pos_B;  // file pointer in Bytes
	u8 pos_b;   // file pointer in bits(0~7), 0 ~ 'pos_b' is reached

	bitfile()
	{
		name = (char*)malloc(1024*sizeof(char));
		init();
	}
	~bitfile()
	{
		free(name);
		if(data)
			free(data);
	}
	// Open a bitfile
	u32 open(const char *file, O_TYPE ot)
	{
		if(ot == READ) // read in from existing file
		{
			FILE * fin = fopen(file, "rb");
			if(fin == NULL)
				return -1;
			clean_and_init();
			otype = ot;
			strcpy(name, file);
			fseek(fin, 0, SEEK_END);
			u32 sizeB = ftell(fin);
			sizeb = sizeB << 3;
			data = (u8*)malloc(sizeB*sizeof(u8));
			if(data)
				return -1;
			fread(data, sizeB, 1, fin);
			fclose(fin);
			return 0;
		}
		else if(ot == WRITE)
		{
			clean_and_init();
			otype = ot;
			strcpy(name, file);
			return 0; // do nothing, wait for first write
		}
		else
			return -1;
	}
	u32 open(const char *file, u8 *dat, u32 nbytes) //read in from str
	{
		if(file==NULL || dat == NULL || otype != READ)
			return -1;
		clean_and_init();
		otype = READ;
		strcpy(name, file);
		sizeb = nbytes << 3;
		data = (u8*)malloc(nbytes*sizeof(u8));
		if(data)
			return -1;
		memcpy(data, dat, nbytes);
		return 0;
	}
	
	// In the last byte, from 'high' to 'pos_b' is not reached yet
	// Read functions
	u32 readB(void *ptr, u32 nbytes)
	{
		return readb(ptr, nbytes<<3) >> 3;
	}
	u32 readb(void *ptr, u32 nbits)
	{
		if(otype == WRITE || eof())
			return 0;
		if(pos_B*8 + pos_b + nbits >= sizeb)
			nbits = sizeb - pos_B*8 - pos_b;
		u8 byte, tmpb;
		u8 bits = nbits % 8;
		u32 nbytes = nbits >> 3;
		u8 * dst = (u8*)ptr;
		// copy whole byte(s), pos_b will not changed.
		u32 i;
		for(i=0; i<nbytes && !eof(); i++)
		{
			byte = data[pos_B];
			pos_B++;
			if(pos_b != 0)
			{
				byte >>= pos_b;
				tmpb = data[pos_B];
				tmpb <<= (8-pos_b);
				byte |= tmpb;
			}
			dst[i] = byte;
		}
		if(eof())
			return i<<3;
		// copy left bit(s) if we have
		if(bits)
		{
			byte = data[pos_B];
			byte >>= pos_b; //move to lower bound
			if(bits <= 8-pos_b) // enough in [pos_B]
			{
				byte &= (0xFF >> (8-bits)); //just need bits bit(s)
				byte <<= (8-bits); //move to upper bound
				pos_b = pos_b + bits;
				if(pos_b == 8)
				{
					pos_b = 0;
					pos_B++;
				}
			}
			else // bits>8-pos_b, not enough, need [pos_B] and [pos_B+1]
			{
				pos_B++;
				tmpb = data[pos_B];
				u8 left = bits - (8-pos_b); //left bits
				tmpb &= (0xFF >> (8-left)); //get left at lower bound
				tmpb <<= (bits-left); //move left upper, or 8-pos_b
				byte |= tmpb;
				pos_b = left;
			}
			dst[i] = byte;
		}
		return nbits;
	}
	
	// Write functions
	u32 writeB(void *ptr, u32 nbytes)
	{
		return writeb(ptr, nbytes << 3) >> 3;
	}
	u32 writeb(void *ptr, u32 nbits)
	{
		if(otype == READ)
			return 0;
		if(data == NULL)
		{
			const u32 size = 20*1024*1024;
			data = (u8*)malloc(size*sizeof(u8));
			memset(data, 0, size*sizeof(u8));
			sizeb = size << 3;
			pos_B = pos_b = 0;
		}
		if(nbits + pos_b > sizeb)
		{
			u32 size = 2*sizeB();
			u8* dat = (u8*)malloc(size*sizeof(u8));
			memset(data, 0, size*sizeof(u8));
			memcpy(dat, data, sizeB());
			sizeb = size << 3;
			free(data);
			data = dat;
		}
		u8 byte;
		u8 bits = nbits % 8;
		u32 nbytes = nbits >> 3;
		u8* src = (u8*)ptr;
		// copy whole byte(s), pos_b will not changed
		u32 i;
		for(i=0; i<nbytes; i++)
		{
			if(pos_b)
			{
				data[pos_B] |= (src[i] << pos_b); // [pos_B]
				pos_B++;
				data[pos_B] = (src[i] >> (8-pos_b)); // [pos_B+1]
			}
			else
				data[pos_B++] = src[i];
		}
		// copy left bit(s) if have
		if(bits)
		{
			byte = src[i] >> (8-bits);//move to lower bound
			byte <<= pos_b; // move upper
			data[pos_B] |= byte; // put to [pos_B]
			if(bits <= 8-pos_b) // enough in [pos_B]
			{
				pos_b = pos_b + bits;
				if(pos_b == 8)
				{
					pos_b = 0;
					pos_B++;
				}
			}
			else // bits>8-pos_b, not enough, need [pos_B+1]
			{
				pos_B++;
				u32 left = bits - (8-pos_b); //left bits
				byte = src[i] >> (8-bits);//move to lower bound
				byte >>= (bits-left);//move left lower, or 8-pos_b
				data[pos_B] = byte;
				pos_b = left;
			}
		}
		return nbits;
	}
	bool eof()
	{
		return (pos_B*8 + pos_b)>=sizeb;
	}
	void close()
	{
		clean_and_init();
	}
	void clean_and_init()
	{
		if(data)
			free(data);
		init();
	}
	u32 sizeB()
	{
		return sizeb/8 + (sizeb%8)?1:0;
	}
	void writeout()
	{
		if(otype == READ || data == NULL)
			return;
		FILE* fout = fopen(name, "wb");
		fwrite(data, sizeB(), 1, fout);
		fclose(fout);
	}
	void init()
	{
		data = NULL;
		strcpy(name, "");
		sizeb = pos_B = pos_b = 0;
	}
	void info()
	{
		if(otype == READ)
			printf("bitfile info: READ  ");
		if(otype == WRITE)
			printf("bitfile info: WRITE ");
		printf("%db(%dB) %d.%d\n", sizeb, sizeB(), pos_B, pos_b);
	}
}bitfile;

#endif

