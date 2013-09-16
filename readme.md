##Bit oriented stream editor

A bit oriented stream editor.

* read stream in bit(s)/byte(s).
* write stream in bit(s)/byte(s).

###NOTICE
1. The whole process is little-endian, no matter byte or bit, that is,
   read/write from low byte/bit to high byte/bit.

2. The program is tested only on little-endian machine.

3. Stream size can't exceed 512MB.
   If you want larger stream support, modify the type of `capb` and `pos_B`.

###License
BSD

