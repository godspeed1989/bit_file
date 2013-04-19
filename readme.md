##file/stdstr BIT reader/writer

A bit oriented file/stdstr reader/writer.    

* read file/stdstr in bit(s)/byte(s).
* write file/stdstr in bit(s)/byte(s).

###NOTICE
1. The bitfile only can be 'just READ' or 'just WRITE'
2. The whole process is little-endian, not matter byte or bit, that is,
   read/write from low byte/bit to high byte/bit
3. The program only run on little-endian mach.
4. File size couldn't exceed 512MB.

