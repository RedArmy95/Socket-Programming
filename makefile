all: client.cpp
	g++ client.cpp -o client
clean:
	rm -f client