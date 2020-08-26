#include<stdio.h> //printf
#include<string.h>    //strlen
#include<sys/socket.h>    //socket
#include<arpa/inet.h> //inet_addr
#include <unistd.h>   //close
#include <stdlib.h>
#include <time.h>
#include "Encryptor.h"

#include <iostream>
#include <fstream>
#define ITR 333333333



//#define ARRSIZE 1000

void show_usage();
void Encrypt(char message[], int start, int s,int op);
void ReadFile(char *filename, char *msg, int size);
void DecToBin(unsigned long long i,char *arr,int len);
unsigned long long BinToDec(char *arr,int len);


int com_key;

int main(int argc , char *argv[])
{
  
  int sock;
  struct sockaddr_in middle;

  if (argc!=7 && argc!=8) {show_usage(); exit(0);}

  int iterations;
  int clientinputsize = atoi(argv[4]);
  int outputsize = atoi(argv[5]);
  int inputsize = atoi(argv[6]);
  char message[2+inputsize];
  char server_reply[outputsize+1];

  if (argc==7)
    iterations = ITR;
  else
    iterations = atoi(argv[7]);

  
  //const int ARRSIZE = atoi(argv[3]);
  //int requests = atoi(argv[4]);
  
  com_key=14324654+atoi(argv[3]);  // initialize key 
  int msgsize= 1+inputsize; // 1 for the client_id, rest for input

  printf("Size of client input: %d\n",clientinputsize);
  printf("Size of output:%d\n",outputsize);
     
  //Create socket
  sock = socket(AF_INET , SOCK_STREAM , 0);
  if (sock == -1)
    {
      printf("Could not create socket");
    }
  puts("Socket created. Trying to connect to server");
     
  middle.sin_addr.s_addr = inet_addr(argv[1]); //127.0.0.1
  middle.sin_family = AF_INET;
  middle.sin_port = htons( atoi(argv[2]) ); //8887
  
  
  //Connect to Middle 
  while (connect(sock , (struct sockaddr *)&middle , sizeof(middle)) < 0)
    {
      //perror("connect failed. Error");
      //return 1;
    }
     
  puts("Connected\n");


  char *BUF;
  
  message[0]='a'+ atoi(argv[3]);
  char fname[10] = "input";
  if (iterations!=ITR){
    memcpy(&fname[5],argv[3],1);
    BUF =(char*)malloc(iterations*clientinputsize);
    ReadFile(fname,BUF,clientinputsize*iterations);
    printf("Filename: %s\n",fname);
  }


  
  for (int i=0;i<iterations;i++){
    int choice;
    printf("Request %d:\n",i);
    memset(&message[clientinputsize+1],'v',inputsize-clientinputsize);

    if (iterations==ITR){
      printf("Way of input:\n");
      printf("0 for direct binary input\n");
      printf("1 for a binary file\n");
      printf("2 for direct decimal input\n");
      printf("Choice:");
      scanf("%d",&choice);
    
      if (choice==0){
	printf("Enter binary input:");
	scanf("%s", &message[1]);
      }else if (choice==1) {
	char z[100];
	printf("Enter binary input file name:");
	scanf("%s", z);
	ReadFile(z,&message[1],clientinputsize);
      }else if (choice==2){
	printf("Enter decimal input:");
	unsigned long long y;
	scanf("%llu",&y);
	DecToBin(y,&message[1],clientinputsize);
      }
    }else
      memcpy(&message[1],&BUF[clientinputsize*i],clientinputsize);

    
    if (message[1]!='0' && message[1]!='1') break;
    message[msgsize]='\0';
    printf("Sent Message:%s\n",message);
    Encrypt(message,1,msgsize,1);   // encrypt before sending
    com_key++;


    //Send the message to server
    printf("Sending the input to the server \n");
    if( send(sock , message , msgsize , 0) < 0)
      {
	puts("Send failed");
	
      }
    
    //Receive a reply from the server
    if( recv(sock , server_reply , outputsize , 0) < 0)
      {
	puts("recv failed");
	break;
      }

    
    server_reply[outputsize]='\0';
    Encrypt(server_reply,0,outputsize,2);   //decrypt message
    com_key++;
    
    
    for (int i = 0; i < outputsize; i++)
    {
      printf("%d", server_reply[i]);
    }
    printf("\n");
    if (choice==2)
      printf("Decimal: %ld\n",BinToDec(server_reply,outputsize));

  }

  free(BUF);
  close(sock);
  return 0;
}



void Encrypt(char message[],int start, int s,int op){
  Encrypt_Com(message,start,s-start,op,com_key);
  /*
  for(int i=start;i<s;i++){
    message[i]= (message[i] ^ com_key);
    }*/
  
}

  
void show_usage(){
  printf("client <server_ip> <server_port> <client_id> <input_size> <output_size> <full_input_size> [num_of_iterations]\n");  
}

  
void ReadFile(char *filename,char * msg, int size)
{
    std::ifstream::pos_type posSize;
    std::ifstream file ((char *)filename, std::ios::in|std::ios::binary|std::ios::ate);
    if (file.is_open())
    {
      
      file.seekg (0, std::ios::beg);
      file.read (msg, size);
      msg[size] = '\0';
      file.close();
    }
    else
      return;
}

void DecToBin(unsigned long long v,char *arr,int len){
  unsigned long long g=1;
  for(int i=0;i<len-1;i++)
    g=g*2;
  
  for(int i=len-1;i>=0;i--){
    unsigned long long  z = v/g;
    if (z>0)
      arr[i]= '1';
    else
      arr[i]='0';
    v = v % g;
    g/=2;
  }
}

unsigned long long BinToDec(char *arr,int len){
  unsigned long long val=0;
  unsigned long long power=1;
  for(int i=0;i<len;i++){
    if (arr[i]=='\1') // this is important since it is comming as a character
      val+=power;
    power*=2;
  }
  return val;
}
