#include <iostream>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>

using namespace std;
//sudo chmod +x server
//gcc -c .c => gcc -o <output_file_name> .o

enum Action{REGISTER, LOGIN, LIST, QUIT, EXCEPT};
Action keyboardListening();




int main()
{
 int socket_fd = 0; //socket file descriptor
 int connect_rt = -1; //connection return value
 struct sockaddr_in server;


//socket creating
 socket_fd = socket(AF_INET, SOCK_STREAM, 0); //create a socket using adress from internet and TCP protocol
 if(socket_fd == -1) //error detection
 {
  cout << "Fail to create a socket.";
  return 0;
 }
 
//server info. setting
 char local_host[] = "127.0.0.1";
 server.sin_addr.s_addr = inet_addr(local_host);
 server.sin_family = AF_INET;
 server.sin_port = htons(9487);

//initial connection
 //int connect(int sd, struct sockaddr* server, int addr_len);
 connect_rt = connect(socket_fd, (struct sockaddr*) &server, sizeof(server)); //with structure casting 'sockaddr_in => sockaddr'
 if(connect_rt == -1) //error detection
 {
  cout << "connection error.";
  return 0;
 }
 char return_message[100] = {};
 recv(socket_fd, return_message, sizeof(return_message), 0);
 cout << "return_message: " << return_message;
 memset(return_message, '\0', sizeof(return_message));



 char input[100];
 char message[120];
 bool logged_in = false;
 Action flag;
//login page
 while(!logged_in)
 {
  cout << "Enter \'r\' to register, \'l\' to login.\n";
  memset(return_message, '\0', sizeof(return_message));
  memset(input, '\0', sizeof(input));
  memset(message, '\0', sizeof(message));
  flag = keyboardListening();

 //case: register
  if(flag == REGISTER)
  {
   cout << "Please enter register username: ";
   cin >> input;
   strcpy(message, "REGISTER#");
   strcat(message, input);
   strcat(message, "\n"); //end of message
   send(socket_fd, message, sizeof(message), 0);
   recv(socket_fd, return_message, sizeof(return_message), 0);
   cout << "return_message: " << return_message;
  }

 //case: login
  else if(flag == LOGIN)
  {
   cout << "Please enter login username: ";
   cin >> input;
   strcpy(message, input);
   cout << "Enter port number: ";
   cin >> input;
   strcat(message, "#");
   strcat(message, input); //concate port #
   strcat(message, "\n"); //end of message
   send(socket_fd, message, sizeof(message), 0);
   recv(socket_fd, return_message, sizeof(return_message), 0);
   cout << "return_message:\n" << return_message << endl;
   if(strstr(return_message, "AUTH_FAIL") == NULL) //logged in
    logged_in = true;
  }
  else
   cout << "Please enter a valid option.";
  cout << "------------------------------------------------------------\n";
 }

//user page
 while(logged_in)
 {
  cout << "Enter \'s\' to list, 'q' to quit.\n";
  memset(return_message, '\0', sizeof(return_message));
  memset(message, '\0', sizeof(message));
  flag = keyboardListening();

 //case: list
  if(flag == LIST)
  {
   strcpy(message, "List\n");
   send(socket_fd, message, sizeof(message), 0);
   recv(socket_fd, return_message, sizeof(return_message), 0);
   cout << "return_message:\n" << return_message;
   //cout << send(socket_fd, "still", sizeof(message), 0);//
  }
 //case: quit
  else if(flag == QUIT)
  {
   strcpy(message, "Exit\n");
   send(socket_fd, message, sizeof(message), 0);
   recv(socket_fd, return_message, sizeof(return_message), 0);
   cout << "return_message:\n" << return_message << endl;
   if(strstr(return_message, "Bye") != NULL) //logged out
    logged_in = false;  
  }
  else
   cout << "Please enter a valid option.";
  cout << "------------------------------------------------------------\n";
 }
 return 0;
}



Action keyboardListening()
{
 char key_in;
 cin >> key_in;
 if(key_in == 'q')
  return QUIT;
 else if(key_in == 'r')
  return REGISTER;
 else if(key_in == 'l')
  return LOGIN;
 else if(key_in == 's')
  return LIST; 
 else
  return EXCEPT;
}