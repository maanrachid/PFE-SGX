#include "Enclave_t.h"

#include "sgx_trts.h"
#include "sgx_tcrypto.h"
#include "garbledcircuit.h"

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "ext.h"

//#define ARRSIZE 1000




void Encrypt_Client(char message[], int start, int size,int client_id,int op);
void Encrypt_Circuit(char message[], int start,int size);
void printf(const char *fmt, ...);
void decryptProgram(char buffer[],int size);
void EncapsulateGProgram(GCircuit GC, char buffer[],int *size,
			 key *ginput,key fixedkey);

void printkey(key kk);


//sgx_aes_ctr_128bit_key_t key;
long long *com_to_client_key;
long long com_key_program;
char clientmessage[3];
int ARRSIZE;
int Mid_Enc_Com,SizeSoFarEnc,SizeSoFarDec,initcounter,currclientmessageindex;
char *A;
Circuit cur;
int NUM_OF_CLIENTS;
bit *datainput=NULL;
bit *output;
Circuit C;
key *outGarblings;







void init(int no_of_clients){
  
  NUM_OF_CLIENTS = no_of_clients;
  com_to_client_key = (long long*) malloc(no_of_clients*sizeof(long long));
  for (int i=0;i<NUM_OF_CLIENTS;i++)
    com_to_client_key[i]= 10000+i;
  com_key_program=10000;
}


void passGoutput(char *inputs,size_t len){  

  /*
  int counter=0;
  for(int j=0;j<C.outSize;j++){
    for (int i = 0; i < sizeof(key); i++)
      {
	printf("%d", inputs[counter++]);
      }
    printf("\n");
    }*/

  
  printf("Decoding .....\n");  
  output = (bit *)calloc(C.outSize, sizeof(bit));
  decodeOutput((key*)inputs, outGarblings, output, C.outSize);

  
  printf("Result received with garbling:    ");
  for (int i = 0; i < C.outSize; i++)
    {
      printf("%d", output[i]);
    }
  printf("\n");

  free(datainput);
  free(outGarblings);
  datainput=NULL;
}


void passInput(char *inputs,size_t len){

  int counter = 0;

  int client_id = inputs[0]-'a';
  //printf("From Server %s\n",inputs);
  //Encrypt_Com(inputs, 1 , 16 , 1, 10000);
  Encrypt_Client(inputs,1,len-1, client_id,2); // decrypt message
  printf("From Server %s\n",inputs);
  com_to_client_key[client_id]++;
  
  if (datainput==NULL)
    datainput = (bit*)malloc((len-1)*2);

  memcpy(&datainput[client_id*(len-1)],&inputs[1],len-1);

  
}




void passProgram(char* input, size_t len){

  decryptProgram(input+4,len-4);
  //printf("The whole input %s\n",datainput);
  
  com_key_program++;

  
 

  /*
  printf("%d,%d,%d\n",C.numGates,C.numWires,C.outSize);
  for(int i=0;i<C.numGates;i++)
    printf("Wire %d %d %d\n",C.gates[i].a,C.gates[i].b,C.gates[i].out);
  */ 

  // Garble the Circuit

  //for (int i=0;i<3;i++){
 
  
  // Evaluate garbled circuit

 
  //free(goutput);
  //free(output1);
  //free(temp);
  //}
 
  
  /*

  
  printf("End Evaluating\n");
  for(int j=0;j<C.outSize;j++){
    for (int i = 0; i < sizeof(key); i++)
      {
	printf("%d", goutput[j].k[i]);
      }
    printf("\n");
    }

  
  */
 
  
}

void getGCircuit(char *input,size_t len){

  // prepare input
  bit* temp = (bit*)malloc(C.inpSize);
  
  int counter=0;
  //printf("%d %d %d\n",C.inpSizes[0],C.inpSizes[1],C.gates[0].a);
  for(int i=0;i<NUM_OF_CLIENTS;i++){
    for(int j=0;j<C.inpSizes[i];j++){
      temp[counter++] =(int)(datainput[C.inpSize*i+j]-'0');
      if (temp[counter-1] != 0 && temp[counter-1] != 1)
	{
	  printf("Input must be in binary. Unkown value %d\n", temp[counter-1]);
	  return;
	}
    }
  }

  
  //printf("The whole input after cleaning %d %d\n",temp[1],counter);

  
  for(int i=0;i<C.inpSize;i++)
    printf("%d",temp[i]);
  printf("\n");



  
  key *ginput = (key *)calloc(C.inpSize, sizeof(key));
  key fixedKey;
  fixedKey= genKey();
  printf("Key generated\n");
  printkey(fixedKey);
  outGarblings = (key *)calloc(C.outSize * 2, sizeof(key));
  if (outGarblings==NULL || ginput==NULL) printf("Malloc porblem \n");
  GCircuit GC = garbleCircuit(C, temp, ginput, fixedKey, outGarblings);
  printkey(*ginput);
  printf("End Garbeling. GC test:%d\n",GC.test);
  //char *GProgram=(char*)malloc(PROGLIMIT);
  //if (GProgram==NULL) printf("Gprogram Malloc porblem \n");
  int prog_size;


  
  EncapsulateGProgram(GC,input,&prog_size,ginput,fixedKey);
  com_key_program++;
  
  printf("Garbled Circuit is Encapsulated size %d\n",prog_size);


  // replace ocall with ecall
  //memcpy(input,GProgram,prog_size);
  //passGProgram(GProgram,prog_size);


  /*
   // --------------- for checking only---- Evaluating in the enclave
  key *goutput = (key *)calloc(C.outSize, sizeof(key));
  bit *output1 = (bit *)calloc(C.outSize, sizeof(bit));
  
  evalGCircuit(GC, ginput, goutput, fixedKey);
  printkey(*goutput);
  
  
  decodeOutput(goutput, outGarblings, output1, C.outSize);
  printkey(*goutput);
  printf("Result received with garbling (test):    ");
  for (int i = 0; i < C.outSize; i++)
    {
      printf("%d", output1[i]);
    }
  printf("\n");
  free(goutput);
  free(output1);
  //---------------------------------
  */

  free(ginput);
  free(temp);
  //free(GProgram);
  free(GC.ggates);
  free(GC.outWLabels);
  free(C.gates);
  free(C.outWLabels);

}



void getOutputFromEnclave(char *output_message,int client_id,size_t len){
  memcpy(output_message,output,C.outSize);
  Encrypt_Client(output_message,0,C.outSize,client_id,1); // encrypt message
  com_to_client_key[client_id]++;
}
     



void Encrypt_Client(char message[],int start, int size,int client_id,int op){
  
  Encrypt_Com(message,start,size,op,com_to_client_key[client_id]);
  
  /*
  for(int i=start;i<size;i++){
    char b = (message[i] ^ com_to_client_key[client_id]);
    message[i]=b;
    //message[i]= (message[i] ^ com_to_client_key);
    }*/
}

void Encrypt_Circuit(char message[], int start,int size,int op){
  Encrypt_Com(message,start,size,op,com_key_program);

  /*
  for(int i=start;i<size;i++){
    char b = (message[i] ^ com_key_program);
    message[i]=b;
    }*/
}

void decryptProgram(char buffer[],int size){

  Encrypt_Circuit(buffer,0,size,2); //decrypt
  /*  for(int i=0;i<size;i++){
    char b = (buffer[i] ^ com_key_program);
    buffer[i]=b;
    }*/

  
  int counter=0;
  int numGates = *((int*)(buffer+counter));
  printf("Number of Gates: %d\n",numGates);
  counter+=4;

  int numWires = *((int*)(buffer+counter));
  printf("Number of Wires: %d\n",numWires);
  counter+=4;

  int num_of_clients = *((int*)(buffer+counter));
  printf("Number of clients: %d\n",num_of_clients);
  counter+=4;

  int inputsize = *((int*)(buffer+counter));
  printf("Input Size: %d\n",inputsize);
  counter+=4;

  int outputsize = *((int*)(buffer+counter));
  printf("Output Size: %d\n",outputsize);
  counter+=4;

  C.numGates= numGates;
  C.numWires = numWires;
  C.num_of_clients = num_of_clients;
  C.gates = (Gate*)malloc(numGates*sizeof(Gate));
  C.inpSizes = (int*)malloc(num_of_clients*sizeof(int));
  C.outWLabels = (int *)malloc(outputsize * sizeof(int));
  if (C.gates==NULL || C.inpSizes==NULL || C.outWLabels==NULL) printf("Malloc Problem\n");
  C.inpSize = inputsize;
  C.outSize = outputsize;
  
  
  for(int i=0;i<numGates;i++){
    Gate g = *((Gate*)(buffer+counter));
    C.gates[i] = g;
    //printf("%d %d %d %d\n",C.gates[i].a,C.gates[i].b,C.gates[i].out,(int)(g.T));
    counter+=sizeof(Gate);
  }

  
  for(int i=0;i< num_of_clients;i++){
    int g =  *((int*)(buffer+counter));
    C.inpSizes[i]=g;
    //printf("inputsize  %d\n",C.inpSizes[i]);
    counter+=sizeof(int);
  }
  
  for(int i=0;i< outputsize;i++){
    int g = *((int*)(buffer+counter));
    C.outWLabels[i]= g;
    //printf("Wire %d\n",C.outWLabels[i]);
    counter+=sizeof(int);
  }


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


void EncapsulateGProgram(GCircuit GC, char buffer[],int *size,
			 key *ginput,key fixedkey){
  int counter=4;
  int numGate = GC.numGates;
  memcpy(&buffer[counter],&numGate,sizeof(int));
  counter+=4;

  int numWires = GC.numWires;
  memcpy(&buffer[counter],&numWires,sizeof(int));
  counter+=4;
  
  int inputsize = GC.inpSize;
  memcpy(&buffer[counter],&inputsize,sizeof(int));
  counter+=4;

  int outputsize = GC.outSize;
  memcpy(&buffer[counter],&outputsize,sizeof(int));
  counter+=4;

  for(int i=0;i<C.numGates;i++){
    GGate g = GC.ggates[i];
    //printf("%d %d %d\n",g.a,g.b,g.out);
    memcpy(&buffer[counter],&g,sizeof(GGate));
    counter+=sizeof(GGate);
  }

  for(int i=0;i< outputsize;i++){
    int g = GC.outWLabels[i];
    //printf("Wire %d\n",g);
    memcpy(&buffer[counter],&g,sizeof(int));
    counter+=sizeof(int);
  }

  memcpy(&buffer[counter],ginput,sizeof(key)*inputsize);
  counter+=sizeof(key)*inputsize;
  memcpy(&buffer[counter],&fixedkey,sizeof(key));
  counter+=sizeof(key);

  memcpy(buffer,&counter,sizeof(int));
  *size = counter;

  //  printf("Encap is done\n");
  Encrypt_Circuit(buffer,4,counter-4,1); //encrypt
  //printf("Encryption is done\n");
  /*
  for(int i=4;i<counter;i++){ //encrypt
    char b = (buffer[i] ^ com_key_program);
    buffer[i]=b;
    }*/


  
}



void printkey(key kk){
  for (int i = 0; i < sizeof(key); i++)
    {
      printf("%d",kk.k[i]);
    }
  printf("\n");
}

void freeEnc(){  
  free(output);
  free(C.inpSizes);
}
