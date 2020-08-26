#include<stdio.h>
#include<string.h>    //strlen
#include<sys/socket.h>
#include<arpa/inet.h> //inet_addr
#include<unistd.h>    //write
#include<stdlib.h>
#define ENCLAVE_FILE "enclave.signed.so"

#include "sgx_urts.h"
#include "Enclave_u.h"
#include "reader.h"
//#include "garbledcircuit.h"
#include <sys/time.h>
#include <sys/types.h>
#include <sys/ioctl.h>
#include "ext.h"
#include <time.h>

//void checkContents(Block* Arr1,char *message);
void show_usage();
void sendGProgram(char *input,int len);




int senttimes=0;
int progowner_sock;
int inputsize,outputsize;
sgx_enclave_id_t   eid;

int main(int argc , char *argv[])
{
  if (argc!=6){
    show_usage();
    exit(0);
  }
  
  int socket_desc ,socket_desc2,  c , read_size, sock;
  clock_t begin_time, end_time;
  
 
  int NUM_OF_CLIENTS = atoi(argv[3]);

  struct sockaddr_in  client[NUM_OF_CLIENTS], middle, server, progowner;
  int client_sock[NUM_OF_CLIENTS];
  int updated = 0;
  
  sgx_status_t       ret   = SGX_SUCCESS;
  sgx_launch_token_t token = {0};
  
  
 
  
  //Create an Enclave
  ret = sgx_create_enclave(ENCLAVE_FILE, SGX_DEBUG_FLAG, &token, &updated, &eid, NULL);


  
  printf("Creating Enclave.....\n");
  if (ret != SGX_SUCCESS) {
    printf("\nApp: error %#x, failed to create enclave.\n", ret);
  }else
    printf("Enclave is created successfully\n");
  
  init(eid,NUM_OF_CLIENTS);
 
  
   
  //Create socket to let Client connect to it -------------------------
  socket_desc = socket(AF_INET , SOCK_STREAM , 0);
  if (socket_desc == -1)
    {
      printf("Could not create socket");
    }
  puts("Socket created to receive from Clients");
  //Create socket to let Client connect to it -------------------------
  socket_desc2 = socket(AF_INET , SOCK_STREAM , 0);
  if (socket_desc == -1)
    {
      printf("Could not create socket");
    }
  puts("Socket created to receive from Program Owner");


  
     
  //Prepare the sockaddr_in structure for clients
  middle.sin_family = AF_INET;
  middle.sin_addr.s_addr = INADDR_ANY;
  middle.sin_port = htons( atoi(argv[1]) );  //8887

  //Prepare the sockaddr_in structure for program owner
  server.sin_family = AF_INET;
  server.sin_addr.s_addr = INADDR_ANY;
  server.sin_port = htons( atoi(argv[2]) );  //8887
  

  
  //Bind
  if( bind(socket_desc,(struct sockaddr *)&middle , sizeof(middle)) < 0)
    {
      //print the error message
      perror("bind failed. Error");
      return 1;
    }
  puts("bind done to clients port");


   //Bind
  if( bind(socket_desc2,(struct sockaddr *)&server , sizeof(middle)) < 0)
    {
      //print the error message
      perror("bind failed. Error");
      return 1;
    }
  puts("bind done to program owner port");


  
  //Listen
  listen(socket_desc , 3);
  listen(socket_desc2 , 3);

  
  //Accept and incoming connection
  puts("Waiting for incoming connections from Client...");
  c = sizeof(struct sockaddr_in);

  
  //accept connection from an incoming clients
  for(int i=0;i<NUM_OF_CLIENTS;i++){  
    client_sock[i] = accept(socket_desc, (struct sockaddr *)&client[i],
			    (socklen_t*)&c);
    
    if (client_sock[i] < 0)
      {
	perror("accept failed");
	return 1;
      }
    puts("Connection accepted");
  }

  puts("All connections are ready");
  // end connecting to client ----------------------------------------------------------

  progowner_sock = accept(socket_desc2, (struct sockaddr *)&progowner,
			    (socklen_t*)&c);

  if (progowner_sock < 0)
    {
      perror("accept failed");
      return 1;
    }
  puts("Connection To program owner accepted");

  inputsize=  atoi(argv[4]); //input size
  outputsize = atoi(argv[5]);
  char client_message[2+inputsize];  
  char output_message[inputsize+1];


  //enclave_message[inputsize]='\0'; // for printing

  int sock_client_map[NUM_OF_CLIENTS];
  //int inpSizesAcc[NUM_OF_CLIENTS];
  
  //inpSizesAcc[0]=0;
 
  bool ex=false;
  
  while (1){
    // receiving data from clients
    bool read=false;
    for (int i=0;i<NUM_OF_CLIENTS;i++){     
      if  ( (read_size = recv(client_sock[i] , client_message ,
			      inputsize+1, 0))	    > 0 ){
	sock_client_map[i] = client_message[0]-'a';
	int index = 	sock_client_map[i];
	printf("index %d\n",index);
	//memcpy(&enclave_message[inpSizesAcc[index]],
	//     &client_message[1], C.inpSizes[index]);
	printf("received from to client %c\n", client_message[0]);
	read=true;
	passInput(eid,client_message,inputsize+1);
	
	//printf("received from to client %c\n", client_message[0]);
	//printf("received from to client %c\n", client_message[0]);
	//printf("%d",C.inpSizes[index]);
	//client_message[C.inpSizes[index]+1]='\0';
	//printf("%s\n",client_message);
	//for(int i=0;i< inputsize+1;i++)
	//  printf("char %d %c\n",i,client_message[i]);
      }else {
	break;
      }
    }


    char *buffer = (char*) malloc(PROGLIMIT);
    int bufsize;
    for(int i=0;i<PROGLIMIT/BUFSIZE;i++){
      if  ( (read_size = recv(progowner_sock,&buffer[i*BUFSIZE],BUFSIZE, 0))  < 0 ){
	puts("Error recv");
      }

      int vv;
      send(progowner_sock,&vv,4, 0);
    }
    //if (ex==true) break;
    
    printf("size of program: %d \n",*((int*)buffer));
    bufsize=*((int*)buffer);

    begin_time = clock();
    passProgram(eid,buffer,bufsize);    
    char *prog=(char*)malloc(PROGLIMIT);
    printf("Time for deserializing circuit: %f\n",
	   float( clock () - begin_time ) /  CLOCKS_PER_SEC);
    begin_time = clock();
    getGCircuit(eid,prog,PROGLIMIT);
    printf("Time for garbeling circuit and input: %f\n",
	   float( clock () - begin_time ) /  CLOCKS_PER_SEC);
   
    int ss = *((int*)prog);
    sendGProgram(prog,ss); 
    free(prog);
    
    char goutput[outputsize*16];
    int read_size;

   
    
    
    if  ((read_size= recv(progowner_sock,&goutput,outputsize*16, 0))> 0 ){
      printf("Received evaluated data from program owner %d\n",read_size);
      
      /*
	int counter=0;
      for(int j=0;j<outputsize;j++){
	for (int i = 0; i < sizeof(key); i++)
	  {
	    printf("%d", goutput[counter++]);
	  }
	printf("\n");
      }
      */
      begin_time=clock();
      passGoutput(eid,goutput,outputsize*16);
      printf("Time for Decoding:%f\n",float( clock () - begin_time ) /  CLOCKS_PER_SEC);
    }
    
    send(progowner_sock,&read_size,4,0);

    // send output to client
    char output[outputsize];
    for(int i=0;i<NUM_OF_CLIENTS;i++){
      int index = sock_client_map[i];
      getOutputFromEnclave(eid,output,index,outputsize);
      write(client_sock[i],output,outputsize);
    }

    freeEnc(eid);
    free(buffer);
  }


  for(int i=0;i<NUM_OF_CLIENTS;i++)
    close(client_sock[i]); // disconnect from server
  close(progowner_sock);



  
  if(SGX_SUCCESS != sgx_destroy_enclave(eid))
    printf("\nApp: error, failed to destroy enclave.\n");
  
  
  if(read_size == 0)
    {
      puts("Client disconnected");
      fflush(stdout);
    }
  else if(read_size == -1)
    {
      perror("recv failed");
    }
     
  return 0;
}





/*
void checkContents(Block* Arr1,char * message){
  puts(message);
  for(int i=0;i<10;i++)
    printf("%d\n",(int)Arr1[i]);
    }*/


void show_usage(){
  printf("server <server_port_clients> <server_port_program_owner> <no_of_clients> <inputsize> <outputsize> \n");
}


/* OCall functions */
void ocall_print_string(const char *str)
{
    /* Proxy/Bridge will check the length and null-terminate 
     * the input string to prevent buffer overflow. 
     */
    printf("%s", str);
}

void sendGProgram(char *input,int len){
  char *buffer=(char*)malloc(PROGLIMIT);
  memcpy(buffer,input,len);
  printf("Sending garbled circuit to program owner %d\n",*((int *)buffer));

  for(int i=0;i<PROGLIMIT/BUFSIZE;i++){
    send(progowner_sock,&buffer[i*BUFSIZE],BUFSIZE,0);

    int vv;
    recv(progowner_sock,&vv,4, 0);
  }

  free(buffer);
}



