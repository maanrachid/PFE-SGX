#include<stdio.h>
#include<string.h>    //strlen
#include<sys/socket.h>
#include<arpa/inet.h> //inet_addr
#include<unistd.h>    //write
#include<stdlib.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/ioctl.h>
#include "ext.h"
#include "reader.h"
#include "garbledcircuit.h"
#include <time.h>



#include <iostream>
#include <fstream>
#define ITR 333333333


int com_key;

void Encrypt(char* message, int start, int s,int op);
void show_usage();
void EncapsulateProgram(Circuit C, char buffer[],int *size);
void decryptGProgram(char buffer[],int size,GCircuit *GC,key *ginput,key *fixedkey);
void printkey(key kk);


int main(int argc , char *argv[])
{
  struct sockaddr_in middle;
  int iterations;int sock;
  clock_t begin_time;
  
  if (argc!=4 && argc!=5){
    show_usage();
    exit(0);
  }

  if (argc==4)
    iterations = ITR;
  else
    iterations = atoi(argv[4]);
  
   // read circuit and pass it to the enclave
  printf("Read the circuit from file %s ....\n",argv[3]);
  Circuit C = readBRISCircuit(argv[3]);
  //passCircuit(eid,&C,sizeof(Circuit));
  printf("Input size:%d\n",C.inpSize);
  printf("Output size:%d\n",C.outSize);
  printf("Wires :%d\n",C.numWires);
  printf("Gates:%d\n",C.numGates);
  printf("GGate size %d\n",sizeof(GGate));

  

  char client_message[2+C.inpSize];  
  char enclave_message[C.inpSize+1];
  char output_message[C.outSize+1];

  sock = socket(AF_INET , SOCK_STREAM , 0);
  if (sock == -1)
    {
      printf("Could not create socket");
    }
  puts("Socket created. Trying to connect to server");
     
  middle.sin_addr.s_addr = inet_addr(argv[1]); //127.0.0.1
  middle.sin_family = AF_INET;
  middle.sin_port = htons( atoi(argv[2]) ); //8887
   //Connect to Enclave Parent

  com_key=10000;
  
  while (connect(sock , (struct sockaddr *)&middle , sizeof(middle)) < 0)
    {
      //perror("connect failed. Error");
      //return 1;
    }
  
  puts("Connected\n");
  
  for (int i=0;i<iterations;i++){
    char *program=(char *)malloc(PROGLIMIT);
    int prog_size;
    begin_time=clock();
    C = readBRISCircuit(argv[3]);
    EncapsulateProgram(C,program,&prog_size);
    Encrypt(program,4, prog_size-4,1);   // encrypt before sending
    com_key++;
     printf("Time for reading and serializing circuit :%f\n",float( clock () - begin_time ) /  CLOCKS_PER_SEC);
    printf("size of program %d\n",prog_size);
    //exit(0);
  
     //Send the message to server
    printf("Sending the circuit to the server \n");
    for(int i=0;i<PROGLIMIT/BUFSIZE;i++){
      if ( send(sock , &program[i*BUFSIZE] ,BUFSIZE , 0) < 0)
	{
	  puts("Send failed");	
	}

      int vv;
      recv(sock,&vv,4, 0);
    }
    //Receive a reply from the server

    int ans;
    //recv(sock,&ans,4,0);

    
    int bufcount=0;int buftot=0;
    printf("Recieving garbled circuit from the server\n");
    for(int i=0;i<PROGLIMIT/BUFSIZE;i++){
     if  (recv(sock , &program[i*BUFSIZE] ,BUFSIZE , 0) <0 )
      {
	puts("Error recv");
      }

     int vv;
     send(sock,&vv,4, 0);
    }


    
    printf("Garbled Program received %d\n",*((int *)program));
    int size = *((int *)program);
    GCircuit GC;key *ginput;key fixedkey;
    ginput = (key *)malloc(sizeof(key)* C.inpSize);
    if (ginput==NULL)printf("malloc problem\n");
    
    Encrypt(program,4,size-4,2); // decrypt

    begin_time=clock();
    decryptGProgram(program,size,&GC,ginput,&fixedkey);
    com_key++;

    printf("Time for deserializing garbeled circuit:%f\n",float( clock () - begin_time ) /  CLOCKS_PER_SEC);
    //evaluate
    if (GC.ggates==NULL || GC.outWLabels==NULL)
      printf("Error Malloc\n");
    begin_time=clock();
    printf("Evaluating garbled circuit..\n");
    key *goutput = (key *)calloc(C.outSize, sizeof(key));
    bit *output1 = (bit *)calloc(C.outSize, sizeof(bit));
    evalGCircuit(GC, ginput, goutput, fixedkey);
    printf("Time for evaluating garbled circuit:%f\n",float( clock () - begin_time ) /  CLOCKS_PER_SEC);
    printkey(*goutput);
    send(sock,goutput,C.outSize*sizeof(key),0);

    int u;
    recv(sock,&u,4,0);
    
    /*
    
    printf("%d\n",GC.numGates);
    printf("%d\n",GC.numWires);
    printf("%d\n",GC.inpSize);
    printf("%d\n",GC.outSize);
    for(int i=0;i<GC.numGates;i++)
      printf("%d %d %d %d\n",GC.ggates[i].a,GC.ggates[i].b,GC.ggates[i].out
	     , GC.ggates[i].GT.ciphers[3]);
    for(int i=0;i<GC.outSize;i++)
      printf("%d\n",GC.outWLabels[i]);
    */

    // check output

    /*
    for(int j=0;j<C.outSize;j++){
      for (int i = 0; i < sizeof(key); i++)
	{
	  printf("%d", goutput[j].k[i]);
	}
      printf("\n");
      }*/

     //check ginput
    /*
   for(int j=0;j<C.inpSize;j++){
      for (int i = 0; i < sizeof(key); i++)
      {
	printf("%d", ginput[j].k[i]);
      }
	printf("\n");
	}*/
  

    
    /*
      for (int i = 0; i < sizeof(key); i++)
      {
      printf("%d",fixedkey.k[i]);
      }
      printf("\n");
    */

    
    
    printf("Sending garbled output to Server %d...\n",C.outSize*sizeof(key));

    free(program);
    free(GC.ggates);
    free(GC.outWLabels);
    free(C.inpSizes);
    free(C.gates);
    free(C.outWLabels);
    //recv(sock,&ans,4,0);
    free(ginput);
    free(goutput);
    free(output1);
    
    /*
    for(int j=0;j<C.inpSize;j++){
      for (int i = 0; i < sizeof(key); i++)
	{
	  printf("%d", ginput[j].k[i]);
	}
      printf("\n");
    }
    */
  }


  close(sock);
  return 0;
}

void show_usage(){
  printf("program_owner <server_ip> <server_port> <program_file> [num_of_iterations]\n");  
}




void Encrypt(char *message,int start, int size,int op){
  Encrypt_Com(message,start,size,op,com_key);
  /*
  for(int i=start;i<s;i++){
    message[i] = message[i] ^ com_key;
    //memcpy(message+i*8,&r,8);
    }*/
}


void EncapsulateProgram(Circuit C, char buffer[],int *size){
  int counter=4;
  int numGate = C.numGates;
  memcpy(&buffer[counter],&numGate,sizeof(int));
  counter+=4;

  int numWires = C.numWires;
  memcpy(&buffer[counter],&numWires,sizeof(int));
  counter+=4;
  
  int num_of_clients = C.num_of_clients;
  memcpy(&buffer[counter],&num_of_clients,sizeof(int));
  counter+=4;

  int inputsize = C.inpSize;
  memcpy(&buffer[counter],&inputsize,sizeof(int));
  counter+=4;

  int outputsize = C.outSize;
  memcpy(&buffer[counter],&outputsize,sizeof(int));
  counter+=4;

  
  for(int i=0;i<C.numGates;i++){
    Gate g = C.gates[i];
    memcpy(&buffer[counter],&g,sizeof(Gate));
    counter+=sizeof(Gate);
  }

  
  for(int i=0;i< C.num_of_clients;i++){
    int g = C.inpSizes[i];
    memcpy(&buffer[counter],&g,sizeof(int));
    counter+=sizeof(int);
  }
  
  for(int i=0;i< outputsize;i++){
    int g = C.outWLabels[i];
    memcpy(&buffer[counter],&g,sizeof(int));
    counter+=sizeof(int);
  }

  memcpy(buffer,&counter,sizeof(int));
  *size = counter;
}


void decryptGProgram(char buffer[],int size, GCircuit* GC, key* ginput, key * fixedkey){
 


  
  int counter=4;
  int numGates = *((int*)(buffer+counter));
  printf("Number of Gates: %d\n",numGates);
  counter+=4;

  int numWires = *((int*)(buffer+counter));
  printf("Number of Wires: %d\n",numWires);
  counter+=4;


  int inputsize = *((int*)(buffer+counter));
  printf("Input Size: %d\n",inputsize);
  counter+=4;

  int outputsize = *((int*)(buffer+counter));
  printf("Output Size: %d\n",outputsize);
  counter+=4;

  GC->numGates= numGates;
  GC->numWires = numWires;
  GC->ggates = (GGate*)malloc(numGates*sizeof(GGate));
  GC->outWLabels = (int *)malloc(outputsize * sizeof(int));
  GC->inpSize = inputsize;
  GC->outSize = outputsize;
  
  
  for(int i=0;i<numGates;i++){
    GGate g = *((GGate*)(buffer+counter));
    GC->ggates[i] = g;
    //printf("%d %d %d\n",g.a,g.b,g.out);
    counter+=sizeof(GGate);
  }
  
  for(int i=0;i< outputsize;i++){
    int g = *((int*)(buffer+counter));
    GC->outWLabels[i]= g;
    //printf("Wire %d\n",g);
    counter+=sizeof(int);
  }

  memcpy(ginput, buffer+counter, inputsize*sizeof(key));
  counter+= sizeof(key)*inputsize;
  memcpy(fixedkey ,buffer+counter,sizeof(key));
  counter+= sizeof(key);
  //printf("done\n");
}

