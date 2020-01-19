#include <cstdio>
#include <openssl/rsa.h>
#include <openssl/pem.h>
#include <iostream>
#include <iomanip>
#include <fstream>
#include <string>
using namespace std;

int main() {
    FILE *pub, *pri, *pub2, *pri2;
    RSA *publicRSA = nullptr, *privateRSA = nullptr, *publicRSA2 = nullptr, *privateRSA2 = nullptr;  // 因為回傳的是指標
    if((pub = fopen("pub.pem","r")) == NULL) {
        cout << "pub Error" << endl;
        exit(-1);
    }
    if((pri = fopen("pri.pem","r")) == NULL) {
        cout << "pri Error" << endl;
        exit(-1);
    }
    if((pub2 = fopen("pub2.pem","r")) == NULL) {
        cout << "pub2 Error" << endl;
        exit(-1);
    }
    if((pri2 = fopen("pri2.pem","r")) == NULL) {
        cout << "pri2 Error" << endl;
        exit(-1);
    }
    // 初始化算法庫
    OpenSSL_add_all_algorithms();
    // 從 .pem 格式讀取公私鑰
    if((privateRSA = PEM_read_RSAPrivateKey(pri, NULL,NULL,NULL)) == NULL) { 
        cout << "Read pri error" << endl;
    }
    if((publicRSA = PEM_read_RSA_PUBKEY(pub,NULL,NULL,NULL)) == NULL) {
        cout << "REad pub erro " << endl;
    }
    if((privateRSA2 = PEM_read_RSAPrivateKey(pri2, NULL,NULL,NULL)) == NULL) { 
        cout << "Read pri2 error" << endl;
    }
    if((publicRSA2 = PEM_read_RSA_PUBKEY(pub2,NULL,NULL,NULL)) == NULL) {
        cout << "REad pub2 erro " << endl;
    }
    fclose(pri2), fclose(pub2), fclose(pri), fclose(pub);
    cout << "pub: " << publicRSA << endl;
    cout << "pri: " << privateRSA << endl;
    cout << "pub2: " << publicRSA2 << endl;
    cout << "pri2: " << privateRSA2 << endl;
    int rsa_len = RSA_size(publicRSA); // 幫你算可以加密 block 大小，字數超過要分開加密
    int rsa_len2 = RSA_size(publicRSA2);    
    const unsigned char * src = (const unsigned char *)"struture"; //  測試的明文
    // 要開空間來存放加解密結果，型態要改成 unsigned char *
    unsigned char * enc = (unsigned char *)malloc(rsa_len); 
    unsigned char * dec = (unsigned char *)malloc(rsa_len2); 
    unsigned char * enc2 = (unsigned char *)malloc(rsa_len2); 
    unsigned char * dec2 = (unsigned char *)malloc(rsa_len); 
    // 加密時因為 RSA_PKCS1_PADDING 的關係，加密空間要減 11，回傳小於零出錯
    if(RSA_private_encrypt(rsa_len-11, src, enc, privateRSA, RSA_PKCS1_PADDING) < 0) {
        cout << "enc error" << endl;
    }
    cerr << "enc: " << enc << endl;
    if(RSA_private_encrypt(rsa_len2-11, enc, enc2, privateRSA2, RSA_PKCS1_PADDING) < 0) {
        cout << "enc2 error" << endl;
    }
    // 加密後就會變成一堆奇怪字元
    cerr << "enc2: " << enc2 << endl;
    // 解密時不用減 11，回傳小於零出錯
    if(RSA_public_decrypt(rsa_len2, enc2,dec,publicRSA2, RSA_PKCS1_PADDING) < 0){
        cout << "dec error" << endl;
    }
    cerr << "dec: " << dec << endl;
    if(RSA_public_decrypt(rsa_len, dec,dec2,publicRSA, RSA_PKCS1_PADDING) < 0){
        cout << "dec2 error" << endl;
    }
    cerr << "dec2: " << dec2 << endl;
    cerr << strcmp((const char*)enc, (const char*)dec) << endl;
    // 因為是它的函式 new 出來的東東，需要用他的函式釋放記憶體
    RSA_free(publicRSA);
    RSA_free(privateRSA);
    RSA_free(publicRSA2);
    RSA_free(privateRSA2);
    }