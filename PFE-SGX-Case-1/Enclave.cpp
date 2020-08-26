#include "Enclave_t.h"

#include "sgx_trts.h"
#include "sgx_tcrypto.h"


#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "Encryptor.h"
//#define ARRSIZE 1000




void Encrypt(char message[], int size,int client_id,int op);
void printf(const char *fmt, ...);



sgx_aes_ctr_128bit_key_t key;
int *com_to_client_key;
char clientmessage[3];
int ARRSIZE;
int Mid_Enc_Com,SizeSoFarEnc,SizeSoFarDec,initcounter,currclientmessageindex;
char *A;
Circuit cur;
int NUM_OF_CLIENTS;
bit *input=NULL;
bit *output=NULL;


void init(int no_of_clients){
  
  NUM_OF_CLIENTS = no_of_clients;
  com_to_client_key = (int*) malloc(no_of_clients*sizeof(int));
  for (int i=0;i<NUM_OF_CLIENTS;i++)
    com_to_client_key[i]= 14324654+i;
  
}

void passCircuit(Circuit *c,size_t len){
  cur= *c;
  if (input==NULL){
    input = (bit*) malloc(cur.inpSize);
    output = (bit*)malloc(cur.outSize+1);
  }
  printf("number of gates:%d\n",cur.numGates);
}



void passInput(char *inputs,size_t len){
  //printf("From Server %s\n",inputs);
  int counter = 0;
  
  for (int i=0;i<NUM_OF_CLIENTS;i++){
    Encrypt(&inputs[counter],cur.inpSizes[i],i,2); // decrypt message
    com_to_client_key[i]++;
    counter+=cur.inpSizes[i];
  }
     
  
  printf("Evaluating...\n");
  for (int i = 0; i < cur.inpSize; i++)
    {
        int b = inputs[i] - '0';
        if (b != 0 && b != 1)
        {
	  printf("index %d Input must be in binary. Unkown value %d\n",i, b);
           
        }
        input[i] = b;
    }
  
 
  evalCircuit(cur, input, output);


  printf("From Enclave: ");
  for (int i = 0; i < cur.outSize+1; i++)
    {
      printf("%d", output[i]);
    }
  printf("\n");
  
  
}


void getOutputFromEnclave(char *output_message,int client_id,size_t len){
  memcpy(output_message,output,cur.outSize);
  Encrypt(output_message,cur.outSize,client_id,1); // encrypt message
  com_to_client_key[client_id]++;
}
     



void Encrypt(char message[], int size,int client_id,int op){
  Encrypt_Com(message,0,size,op,com_to_client_key[client_id]);
  
  /*
  for(int i=0;i<size;i++){
    char b = (message[i] ^ com_to_client_key[client_id]);
    message[i]=b;
    //message[i]= (message[i] ^ com_to_client_key);
    }*/
}


/* 
 * printf: 
 *   Invokes OCALL to display the enclave buffer to the terminal.
 */
void printf(const char *fmt, ...)
{
    char buf[BUFSIZ] = {'\0'};
    va_list ap;
    va_start(ap, fmt);
    vsnprintf(buf, BUFSIZ, fmt, ap);
    va_end(ap);
    ocall_print_string(buf);
}

