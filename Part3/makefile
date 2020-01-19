all: client.cpp
	g++ client.cpp -L /Users/redarmy95/Desktop/NTU/Computer_Networks/Programming/Part3/openssl-1.1.1d -l ssl -l crypto -o client
	g++ server.cpp -L /Users/redarmy95/Desktop/NTU/Computer_Networks/Programming/Part3/openssl-1.1.1d -l ssl -l crypto -o server
	openssl genrsa -out payer_pri.pem 1024
	openssl rsa -in payer_pri.pem -pubout -out payer_pub.pem
	openssl genrsa -out payee_pri.pem 1024
	openssl rsa -in payee_pri.pem -pubout -out payee_pub.pem
clean:
	rm -f client
	rm -f server
	rm -f payer_pri.pem
	rm -f payer_pub.pem
	rm -f payee_pri.pem
	rm -f payee_pub.pem
	