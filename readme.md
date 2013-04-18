##file bit reader/writer

A bit oriented reader/writer.    

* read file in bit(s)/byte(s).
* write file in bit(s)/byte(s).

###NOTICE
1. The bitfile only can be 'just READ' or 'just WRITE'
2. The whole class is little-endian, not matter byte or bit, that is,
   read/write from low byte/bit to high byte/bit
3. file size can't exceed 512MB

