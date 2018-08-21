# ConsistenHash
C语言实现的一致性哈希，借用MurmurHash3算法

借用了
https://github.com/aappleby/smhasher/tree/master/src
实现的MurmurHash3算法

stlc_list.h 是内核的双向链表，比较好用

DJB算法总体来说会比MurmurHash3优秀，MurmurHash3对有规律性的key更具有效率，同时对长度较长的key更有效率。如果key较短，还是用djb比较合适
