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

typedef struct bitfile
{
	O_TYPE otype;
	char * name;// file name
	u8 * data;  // file data
	u32 capb;   // file capacity in BITS
	// file position in bits = pos_B<<3 + pos_b
	u32 pos_B;  // file pointer in Bytes
	u8 pos_b;   // file pointer in bits(0~7), 0 ~ 'pos_b-1' is reached

	bitfile()
	{
		name = (char*)malloc(1024 * sizeof(char));
		init();
	}
	~bitfile()
	{
		free(name);
		if(data)
			free(data);
	}
	// Open a bitfile, READ or WRITE
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
			u32 filesize = ftell(fin);
			capb = filesize << 3;
			data = (u8*)malloc(filesize * sizeof(u8));
			if(data == NULL)
				return -1;
			fseek(fin, 0, SEEK_SET);
			fread(data, filesize, 1, fin);
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
	// Open a bitfile from stdstr, READ only
	u32 open(const char *file, u8 *dat, u32 nbytes)
	{
		if(file==NULL || dat == NULL || nbytes == 0)
			return -1;
		clean_and_init();
		otype = READ;
		strcpy(name, file);
		capb = nbytes << 3;
		data = (u8*)malloc(nbytes * sizeof(u8));
		if(data)
			return -1;
		memcpy(data, dat, nbytes);
		return 0;
	}
	
	/* Read functions */
	/* Read nbytes byte(s), return num of bit(s) actual read. */
	u32 readB(void *ptr, u32 nbytes)
	{
		return readb(ptr, nbytes<<3);
	}
	/* Read nbits bit(s), return num of bit(s) actual read. */
	u32 readb(void *ptr, u32 nbits)
	{
		if(otype == WRITE || eof())
			return 0;
		if(sizeb() + nbits >= capb)// not enough to read
			nbits = capb - sizeb();
		u8 byte, tmpb;
		u8 bits = nbits % 8;
		u32 nbytes = nbits >> 3;
		u8 * dst = (u8*)ptr;
		// copy whole byte(s), pos_b will not be changed.
		if(pos_b != 0)
		{
			for(u32 i=0; i<nbytes; i++)
			{
				byte = data[pos_B];
				byte >>= pos_b; //move to lower bound
				pos_B++;
				tmpb = data[pos_B];
				tmpb <<= (8-pos_b); //move upper
				byte |= tmpb;
				dst[i] = byte;
			}
		}
		else
		{
			memcpy(dst, data+pos_B, nbytes);
			pos_B += nbytes;
		}
		if(eof())
			return nbytes<<3;
		// read left bit(s) if we have
		if(bits != 0)
		{
			byte = data[pos_B];
			byte >>= pos_b; //move to lower bound
			if(bits <= 8-pos_b) // enough in [pos_B]
			{
				byte &= (0xFF >> (8-bits)); //just need bits bit(s)
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
				tmpb &= (0xFF >> (8-left)); //get left bits at lower bound
				tmpb <<= (bits-left); //move left bits upper, or 8-pos_b
				byte |= tmpb;
				pos_b = left;
			}
			dst[nbytes] = byte;
		}
		return nbits;
	}
	/* Write functions */
	/* Write nbytes byte(s), return num of bit(s) actual written. */
	u32 writeB(void *ptr, u32 nbytes)
	{
		return writeb(ptr, nbytes << 3);
	}
	/* Write nbits bit(s), return num of bit(s) actual written. */
	u32 writeb(void *ptr, u32 nbits)
	{
		if(otype == READ)
			return 0;
		if(data == NULL)
		{
			const u32 size = 25*1024*1024; // 25MB
			data = (u8*)malloc(size * sizeof(u8));
			memset(data, 0, size * sizeof(u8));
			capb = size << 3;
			pos_B = pos_b = 0;
		}
		if(nbits + sizeb() > capb) //capacity not enough, double the size
		{
			u32 size = 2 * sizeB();
			u8* dat = (u8*)malloc(size * sizeof(u8));
			memset(dat, 0, size * sizeof(u8));
			memcpy(dat, data, sizeB());
			capb = size << 3;
			free(data);
			data = dat;
		}
		u8 byte;
		u8 bits = nbits % 8;
		u32 nbytes = nbits >> 3;
		u8* src = (u8*)ptr;
		// copy whole byte(s), pos_b will not be changed
		if(pos_b != 0)
		{
			for(u32 i=0; i<nbytes; i++)
			{
				byte = (src[i] << pos_b); //move upper
				data[pos_B] |= byte; //[pos_B]
				pos_B++;
				data[pos_B] = (src[i] >> (8-pos_b)); //[pos_B+1]
			}
		}
		else
		{
			memcpy(data+pos_B, src, nbytes);
			pos_B += nbytes;
		}
		// write left bit(s) if have
		if(bits)
		{
			byte = src[nbytes];
			byte <<= pos_b; //move upper
			data[pos_B] |= byte; //put to [pos_B]
			if(bits <= 8-pos_b) //enough in [pos_B]
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
				byte = src[nbytes];
				byte >>= (bits-left);//move left bits lower, or 8-pos_b
				data[pos_B] = byte;
				pos_b = left;
			}
		}
		return nbits;
	}
	/* from WRITE to READ mode, read from start */
	u32 write_to_read()
	{
		if(otype==READ || data==NULL || sizeb()==0 || capb==0)
			return -1;
		otype = READ;
		capb = sizeb();
		pos_B = pos_b = 0;
		return 0;
	}
	bool eof()
	{
		return sizeb() >= capb;
	}
	void close()
	{
		clean_and_init();
	}
	void clean_and_init()
	{
		if(data)
			free(data);
		data = NULL;
		init();
	}
	u32 sizeb()
	{
		return pos_B * 8 + pos_b;
	}
	u32 sizeB()
	{
		return pos_B + (pos_b!=0);
	}
	void writeout()
	{
		if(otype == READ || data == NULL)
			return;
		FILE* fout = fopen(name, "wb");
		if(fout == NULL)
			return;
		fwrite(data, sizeB(), 1, fout);
		fclose(fout);
	}
	void init()
	{
		data = NULL;
		strcpy(name, "");
		capb = pos_B = pos_b = 0;
	}
	void info()
	{
		if(otype == READ)
			printf("bitfile [%s] info: READ ", name);
		if(otype == WRITE)
			printf("bitfile [%s] info: WRITE ", name);
		printf("%db(%dB) %d.%d %dKB\n", sizeb(), sizeB(), pos_B, pos_b, capb>>13);
	}
}bitfile;

#endif

