須先下載openssl並安裝

Compile:

先將makefile中的

g++ client.cpp -L <路徑> -l ssl -l crypto -o client

g++ server.cpp -L <路徑> -l ssl -l crypto -o server

兩行的<路徑>改成openssl的folder位置

然後輸入makefile編譯

Run:

./server 然後輸入port

./client 然後輸入然後輸入server ip, port

轉帳時轉帳方使用"t"指令

收帳方須先使用"a"指令