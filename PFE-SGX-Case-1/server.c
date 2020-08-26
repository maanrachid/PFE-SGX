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


//void checkContents(Block* Arr1,char *message);
void show_usage();
int senttimes=0;


int main(int argc , char *argv[])
{
  if (argc!=4){
    show_usage();
    exit(0);
  }
  
  int socket_desc ,  c , read_size, sock;
  int NUM_OF_CLIENTS = atoi(argv[3]);
  int client_sock[NUM_OF_CLIENTS];
  struct sockaddr_in middle , client[NUM_OF_CLIENTS], server;

  int updated = 0;
  


  sgx_enclave_id_t   eid;
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
  // read circuit and pass it to the enclave
  printf("Read the circuit from file %s ....\n",argv[2]);
  Circuit C = readBRISCircuit(argv[2]);
  passCircuit(eid,&C,sizeof(Circuit));
  printf("Input size:%d\n",C.inpSize);
  printf("Output size:%d\n",C.outSize);

  char client_message[2+C.inpSize];  
  char enclave_message[C.inpSize+1];
  char output_message[C.outSize+1];
  
   
  //Create socket to let Client connect to it -------------------------
  socket_desc = socket(AF_INET , SOCK_STREAM , 0);
  if (socket_desc == -1)
    {
      printf("Could not create socket");
    }
  puts("Socket created to receive from Client");
     
  //Prepare the sockaddr_in structure
  middle.sin_family = AF_INET;
  middle.sin_addr.s_addr = INADDR_ANY;
  middle.sin_port = htons( atoi(argv[1]) );  //8887
     
  //Bind
  if( bind(socket_desc,(struct sockaddr *)&middle , sizeof(middle)) < 0)
    {
      //print the error message
      perror("bind failed. Error");
      return 1;
    }
  puts("bind done");
     
  //Listen
  listen(socket_desc , 3);
     
  //Accept and incoming connection
  puts("Waiting for incoming connections from Client...");
  c = sizeof(struct sockaddr_in);

  
  //accept connection from an incoming clients
  for(int i=0;i<NUM_OF_CLIENTS;i++){  
    client_sock[i] = accept(socket_desc, (struct sockaddr *)&client[i], (socklen_t*)&c);
    
    if (client_sock[i] < 0)
      {
	perror("accept failed");
	return 1;
      }
    puts("Connection accepted");
  }

  puts("All connections are ready");
  // end connecting to client ----------------------------------------------------------

  int msgsize=  1+C.inpSize; 
  enclave_message[C.inpSize]='\0'; // for printing

  int sock_client_map[NUM_OF_CLIENTS];
  int inpSizesAcc[NUM_OF_CLIENTS];

  
  
  inpSizesAcc[0]=0;
  for(int i=1;i<NUM_OF_CLIENTS;i++)
    inpSizesAcc[i]=C.inpSizes[i-1];

   
  for(int i=1;i<NUM_OF_CLIENTS;i++)
    inpSizesAcc[i]+=inpSizesAcc[i-1];
  

  for(int i=0;i<NUM_OF_CLIENTS;i++)
    printf("Size Acc:%d\n",inpSizesAcc[i]);
  
  
  while (1){
    // receiving data from clients
     C = readBRISCircuit(argv[2]);
    bool read=false;
    for (int i=0;i<NUM_OF_CLIENTS;i++){
    
      
      if  ( (read_size = recv(client_sock[i] , client_message ,
			      C.inpSize+1, 0))	    > 0 ){
	sock_client_map[i] = client_message[0]-'a';
	int index = 	sock_client_map[i];
	printf("index %d\n",index);
	memcpy(&enclave_message[inpSizesAcc[index]],
	       &client_message[1], C.inpSizes[index]);
	printf("received from to client %c\n", client_message[0]);
	read=true;
	//printf("received from to client %c\n", client_message[0]);
	//printf("received from to client %c\n", client_message[0]);
	//printf("%d",C.inpSizes[index]);
	//client_message[C.inpSizes[index]+1]='\0';
	//printf("%s\n",client_message);
	//for(int i=0;i< C.inpSize+1;i++)
	//printf("char %d %c\n",i,client_message[0]);
      }
    }
    
    // pass input
    if (read) passInput(eid,enclave_message,C.inpSize+1);


    /*
    for (int i = 0; i < OUTPUTSIZE; i++)
    {
      printf("%d", output_message[i]);
    }
    printf("\n");
    */
    for (int i=0;i<NUM_OF_CLIENTS;i++){
      int index = sock_client_map[i];
      getOutputFromEnclave(eid,output_message,index,C.outSize);
      write(client_sock[i],output_message,C.outSize );
    }
    free(C.gates);
    free(C.inpSizes);
    free(C.outWLabels);
  }
  close(sock); // disconnect from server




  /*
  if(SGX_SUCCESS != sgx_destroy_enclave(eid))
    printf("\nApp: error, failed to destroy enclave.\n");
  */
  
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
  printf("server <server_port> <filename> <no_of_clients>  \n");
}


/* OCall functions */
void ocall_print_string(const char *str)
{
    /* Proxy/Bridge will check the length and null-terminate 
     * the input string to prevent buffer overflow. 
     */
    printf("%s", str);
}
