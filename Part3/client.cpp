#include <iostream>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <cstdio>
#include <openssl/rsa.h>
#include <openssl/pem.h>
#include <iomanip>
#include <fstream>
#include <string>

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
    char receiveMessage[1000]= {};
    recv(sockfd,receiveMessage,sizeof(receiveMessage),0);

    bool online = true;
    bool signedin = false;

    cout << receiveMessage;

    while(online){
        char input;

        cout << "Enter r for register, s for sign in, l for online list, q for quit, t for transaction:" ;
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



//**************************************



        else if(input == 't')
        {

 



            int status;
            if(fork() == 0)
            {
                char request_user[100];
                memset(receiveMessage, '\0', sizeof(receiveMessage));
                strcpy(message, "TRANS#");
                cout << "Please enter the user you want to transact with:";
                cin >> request_user;
                strcat(message, request_user);
                strcat(message, "\n"); //end of message
                send(sockfd, message, sizeof(message), 0);
                recv(sockfd, receiveMessage, sizeof(receiveMessage), 0);
                
                if(strstr(receiveMessage, "230") != NULL)
                    cout << "No such user!" << endl;
                else
                {
                    strcpy(ip_address, strtok(receiveMessage, "#"));
                    portNumber = atoi(strtok(NULL, "#"));                
                }
                cout << "Please enter the amount you want to transact:";
                char money[10];
                cin >> money;

                int sockfd2 = 0;
                sockfd2 = socket(AF_INET , SOCK_STREAM , 0);

                if (sockfd2 == -1){
                    cout << "Fail to create a socket.";
                    return 0;
                }

                struct sockaddr_in info2;
                bzero(&info,sizeof(info2));
                info2.sin_family = PF_INET;
                info2.sin_addr.s_addr = inet_addr(ip_address);
                info2.sin_port = htons(portNumber);


                int err = connect(sockfd2,(struct sockaddr *)&info2,sizeof(info2));
                if(err==-1){
                    cerr << "Connection error";
                    return 0;
                }

//**************Encryption***************


                FILE *pri;
                RSA *privateRSA = nullptr;
                if((pri = fopen("payer_pri.pem","r")) == NULL) {
                    cout << "pri Error" << endl;
                    exit(-1);
                }
                // 初始化算法庫
                OpenSSL_add_all_algorithms();
                // 從 .pem 格式讀取公私鑰
                if((privateRSA = PEM_read_RSAPrivateKey(pri, NULL,NULL,NULL)) == NULL) { 
                    cout << "Read pri error" << endl;
                }
                fclose(pri);
                int rsa_len = RSA_size(privateRSA); // 幫你算可以加密 block 大小，字數超過要分開加密
                
                const unsigned char * src = (const unsigned char *)money; //  測試的明文
                // 要開空間來存放加解密結果，型態要改成 unsigned char *

                unsigned char * enc = (unsigned char *)malloc(rsa_len);
                // 加密時因為 RSA_PKCS1_PADDING 的關係，加密空間要減 11，回傳小於零出錯
                if(RSA_private_encrypt(rsa_len-11, src, enc, privateRSA, RSA_PKCS1_PADDING) < 0) {
                    cout << "enc error" << endl;
                }
                cerr << "enc: " <<(const char*) enc << ":" << endl;
                RSA_free(privateRSA);

                cout << "len:" << strlen( (const char*) enc) << endl;

//***************Encryption********************

                cout << send(sockfd2, (const char*) enc, strlen((const char*) enc), 0) << "%" << strlen((const char*)enc) << endl;
            }
            else
                wait(&status);
            

        }





//*********************************************






        else if(input == 'a')
        {
            int status, fd[2];
            pipe(fd);
            char enc_message[500];

            if(fork() == 0)
            {
                cout << "Please enter your port again:";
                int socket_desc2 , new_socket2 , c2, port_temp;
                cin >> port_temp;
                struct sockaddr_in server2 , client2;
                char *message2;
                
                //Create socket
                socket_desc2 = socket(AF_INET , SOCK_STREAM , 0);
                if (socket_desc2 == -1)
                {
                    printf("Could not create socket");
                }
                
                //Prepare the sockaddr_in structure
                server2.sin_family = AF_INET;
                server2.sin_addr.s_addr = INADDR_ANY;
                server2.sin_port = htons( port_temp );
                
                //Bind
                if( bind(socket_desc2,(struct sockaddr *)&server2 , sizeof(server2)) < 0)
                {
                    puts("bind failed");
                    return 1;
                }
                puts("bind done");
                
                //Listen
                listen(socket_desc2 , 3);
                
                //Accept and incoming connection
                puts("Waiting for incoming connections...");
                c2 = sizeof(struct sockaddr_in);
                new_socket2 = accept(socket_desc2, (struct sockaddr *)&client2, (socklen_t*)&c2);
                if (new_socket2<0)
                {
                    perror("accept failed");
                    return 1;
                }
                
                puts("Connection accepted");


//**************Encryption***************


                FILE *pri, *pub;
                RSA *privateRSA = nullptr, *publicRSA = nullptr;


                if((pub = fopen("payer_pub.pem","r")) == NULL) {
                    cout << "pub Error" << endl;
                    exit(-1);
                }

                OpenSSL_add_all_algorithms();
                if((publicRSA = PEM_read_RSA_PUBKEY(pub, NULL,NULL,NULL)) == NULL) { 
                    cout << "Read pub error" << endl;
                }
                fclose(pub);

                int rsa_len_dec = RSA_size(publicRSA); 

                char recv_enc[1000];
                memset(recv_enc, '\0', sizeof(recv_enc));
                cout << recv(new_socket2, recv_enc, sizeof(recv_enc), 0);
                cerr << "Received message: " << recv_enc << ":" << endl;
                cout << "len:" << strlen(recv_enc) << endl;

                const unsigned char * src = (const unsigned char *)recv_enc;
                unsigned char * dec = (unsigned char *)malloc(rsa_len_dec);

                cout << src << endl;

                if(RSA_public_decrypt(rsa_len_dec, src, dec, publicRSA, RSA_PKCS1_PADDING) < 0) {
                    cout << "dec error" << endl;
                }
                cerr << dec << ":" << endl;

                RSA_free(publicRSA);

//**********************************************************



//**********************************************************

                if((pri = fopen("payee_pri.pem","r")) == NULL) {
                    cout << "pri Error" << endl;
                    exit(-1);
                }


                if((privateRSA = PEM_read_RSAPrivateKey(pri, NULL,NULL,NULL)) == NULL) { 
                    cout << "Read pri error" << endl;
                }

                fclose(pri);
                int rsa_len_enc = RSA_size(privateRSA); 


                unsigned char * enc = (unsigned char *)malloc(rsa_len_enc);

                if(RSA_private_encrypt(rsa_len_enc-11, dec, enc, privateRSA, RSA_PKCS1_PADDING) < 0) {
                    cout << "enc error" << endl;
                }
                RSA_free(privateRSA);
                cerr << endl << "enc: " << enc << ":" << endl;
                cerr << "len:" << strlen((const char*)enc) << endl;


//***************Encryption********************

                close(fd[0]);
                write(fd[1], (const char*)enc, rsa_len_enc);
                close(fd[1]);

                close(new_socket2);

                exit(0);
            }
            else
            {
                wait(&status);
                close(fd[1]);
                read(fd[0], enc_message, sizeof(enc_message));
                close(fd[0]);
            }

            cerr << endl << "encoding message:" << enc_message << ":" << endl;
            cerr << "len:" << strlen(enc_message) << endl;

            cout << send(sockfd, enc_message, strlen(enc_message), 0);

        }







//************************************



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