#include "Encryptor.h"




void encrypt_ipp(char message[], int size,int op,char * IV_counter){
  // op=1 is encryption, op=2 is decryption

  Ipp8u key[] = "\x00\x01\x02\x03\x04\x05\x06\x07"
    "\x08\x09\x10\x11\x12\x13\x14\x15";
  // define and setup AES cipher
  int ctxSize;
  ippsAESGetSize(&ctxSize);
  IppsAESSpec* pCtx = (IppsAESSpec*)( new Ipp8u [ctxSize] );  
  ippsAESInit(key, sizeof(key)-1, pCtx, ctxSize);  

  
  Ipp8u pIV[] = "\xff\xee\xdd\xcc\xbb\xaa\x99\x88"
    "\x77\x66\x55\x44\x33\x22\x11\x00"; 

  memcpy(pIV,IV_counter,16);

  
  Ipp8u* pSrc= (Ipp8u*)message;
  int msgsize = size;
  Ipp8u* pDst = (Ipp8u*) malloc(msgsize);
  
  
  if (op==1){
    if (ippsAESEncryptCBC(pSrc,pDst,msgsize,pCtx,pIV)!=ippStsNoErr){
      //printf("Error Encrypting client\n");
      //exit(0);
    }
  }else{
    if (ippsAESDecryptCBC(pSrc,pDst,msgsize,pCtx,pIV)!=ippStsNoErr){
      //printf("Error Decrypting client\n");
      //exit(0);
    }
  }
  
  memcpy(message,pDst,msgsize);
  free(pDst);
  delete [] (Ipp8u*) pCtx;
  
}

void encrypt_ipp2(char message[], int size,int op,char * IV_counter){
  // op=1 is encryption, op=2 is decryption

  Ipp8u key[] = "\x00\x01\x02\x03\x04\x05\x06\x07"
    "\x08\x09\x10\x11\x12\x13\x14\x15";
  // define and setup AES cipher
  int ctxSize;
  ippsAESGetSize(&ctxSize);
  IppsAESSpec* pCtx = (IppsAESSpec*)( new Ipp8u [ctxSize] );  
  ippsAESInit(key, sizeof(key)-1, pCtx, ctxSize);  

  
  Ipp8u pIV[] = "\xff\xee\xdd\xcc\xbb\xaa\x99\x88"
    "\x77\x66\x55\x44\x33\x22\x11\x00"; 

  memcpy(pIV,IV_counter,16);

  
  Ipp8u* pSrc= (Ipp8u*)message;
  int msgsize = size;
  Ipp8u* pDst = (Ipp8u*) malloc(msgsize);
  
  
  if (op==1){
    if (ippsAESEncryptCTR(pSrc,pDst,msgsize,pCtx,pIV,64)!=ippStsNoErr){
      //printf("Error Encrypting client\n");
      //exit(0);
    }
  }else{
    if (ippsAESDecryptCTR(pSrc,pDst,msgsize,pCtx,pIV,64)!=ippStsNoErr){
      //printf("Error Decrypting client\n");
      //exit(0);
    }
  }
  
  memcpy(message,pDst,msgsize);
  free(pDst);
  delete [] (Ipp8u*) pCtx;
}






void Encrypt_Com(char message[],int start, int size,int op,long long IV_counter){
  char buffer[16];
  memset(buffer,0,16);
  memcpy(buffer, &IV_counter,sizeof(long long));
  encrypt_ipp2(&message[start],size,op,buffer);
}


void Encrypt_Enclave(char message[], int size,int op,char* IV_counter){
  encrypt_ipp(message,size,op,IV_counter);
}

void Encrypt_Enclave2(char message[], int size,int op,char* IV_counter){
  encrypt_ipp2(message,size,op,IV_counter);
}


