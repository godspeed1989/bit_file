##Bit oriented file/memory reader/writer

A bit oriented file/memory reader/writer.

* read file/memory in bit(s)/byte(s).
* write file/memory in bit(s)/byte(s).

###NOTICE
1. The file can be 'just READ' or 'just WRITE'.
   You can convert from WRITE to READ mode using `write_to_read()` func.

2. The whole process is little-endian, no matter byte or bit, that is,
   read/write from low byte/bit to high byte/bit.

3. The program run on little-endian machine only.

4. File size can't exceed 512MB.
   If you want larger file support, modify the type of `capb` and `pos_B`.

###License
BSD

