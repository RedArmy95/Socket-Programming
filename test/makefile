all: openssl.cpp
	g++ openssl.cpp -L /Users/redarmy95/Desktop/NTU/Computer_Networks/Programming/Part3/openssl-1.1.1d -l ssl -l crypto -o a.out
	openssl genrsa -out pri.pem 1024
	openssl rsa -in pri.pem -pubout -out pub.pem
	openssl genrsa -out pri2.pem 1024
	openssl rsa -in pri2.pem -pubout -out pub2.pem
clean:
	rm -f openssl
	rm -f pri.pem
	rm -f pub.pem
