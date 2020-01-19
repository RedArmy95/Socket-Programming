#include <iostream>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/types.h>

using namespace std;

int main()
{
    char ip_address[] = {};
    int portNumber = 0;
	int sockfd = 0;
    sockfd = socket(AF_INET , SOCK_STREAM , 0);

    if (sockfd == -1){
        cout << "Fail to create a socket.";
        return 0;
    }

    struct sockaddr_in info;
    bzero(&info,sizeof(info));
    info.sin_family = PF_INET;

    cout << "Please enter the IP address of server:";
    cin >> ip_address;
    cout << "Please enter the port number of server:";
    cin >> portNumber;

    //localhost test
    info.sin_addr.s_addr = inet_addr(ip_address);
    info.sin_port = htons(portNumber);


	int err = connect(sockfd,(struct sockaddr *)&info,sizeof(info));
    if(err==-1){
        cerr << "Connection error";
        return 0;
    }   

    char message[100]= {};
    char receiveMessage[100]= {};
    recv(sockfd,receiveMessage,sizeof(receiveMessage),0);

    bool online = true;
    bool signedin = false;

    cout << receiveMessage;

    while(online){
        char input;

        cout << "Enter r for register, s for sign in, l for online list, q for quit:";
        cin >> input;

        if(input == 'r')
        {
            char username[100];
            memset(receiveMessage, '\0', sizeof(receiveMessage));
            cout << "Please enter your username:";
            cin >> username;
            strcpy(message, "REGISTER#");
            strcat(message, username);
            strcat(message, "\n"); //end of message
            send(sockfd, message, sizeof(message), 0);
            recv(sockfd, receiveMessage, sizeof(receiveMessage), 0);
            cout << receiveMessage;
        }
        else if(input == 's')
        {
            char username[100];
            char port[100];
            memset(receiveMessage, '\0', sizeof(receiveMessage));
            cout << "Please enter your username and port number:";
            cin >> username;
            strcpy(message, username);
            cin >> port;
            strcat(message, "#");
            strcat(message, port); 
            strcat(message, "\n"); 
            send(sockfd, message, sizeof(message), 0);
            recv(sockfd, receiveMessage, sizeof(receiveMessage), 0);

            cout << receiveMessage;

            if(strstr(receiveMessage, "220 AUTH_FAIL") == NULL) 
            {
                signedin = true;
            }
        }
        else if(input == 'l')
        {
            memset(receiveMessage, '\0', sizeof(receiveMessage));
            if(signedin == true)
            {
                strcpy(message, "List\n");
                send(sockfd, message, sizeof(message), 0);
                recv(sockfd, receiveMessage, sizeof(receiveMessage), 0);
                cout  << receiveMessage;
            }
            else
            {
                cerr << "Failed to acquire online list." << endl;
            }

        }
        else if(input == 'q')
        {
            memset(receiveMessage, '\0', sizeof(receiveMessage));
            strcpy(message, "Exit\n");
            send(sockfd, message, sizeof(message), 0);
            recv(sockfd, receiveMessage, sizeof(receiveMessage), 0);
            cout << receiveMessage << endl;
            if(strstr(receiveMessage, "Bye") != NULL) 
            {
//                online = false;
                signedin = false;
            }
            else
                cerr << "Failed to disconnect!" << endl;
        }
        else
        {
            cerr << "Error" << endl;
        }
    }

	return 0;
}