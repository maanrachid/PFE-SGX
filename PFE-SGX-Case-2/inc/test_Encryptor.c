#include <string.h>
#include "ippcp.h"
#include "Encryptor.h"
#include <stdio.h>

int main(){

  long long i=10;
  char buffer[16] = {'a','s','d','E',0};
  char buffer2[25] = {'a','s','d','E',
		     'a','s','d','E',
		     'a','s','d','E',
		     'a','s','d','E',
		     'a','s','d','E',0};
  
  char key[16]= {1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16};
  printf("%s\n",buffer);
  Encrypt_Enclave(buffer,16,1,key);
  printf("%s\n",buffer);
  Encrypt_Enclave(buffer,16,2,key);
  printf("%s\n",buffer);
  printf("---------\n");
  Encrypt_Enclave2(buffer,4,1,key);
  printf("%s\n",buffer);
  Encrypt_Enclave2(buffer,4,2,key);
  printf("%s\n",buffer);
  printf("---------\n");
  long long v =10001;
  long  long f = 10001;
  Encrypt_Com(buffer2,1,19,1,v);
  printf("%s\n",buffer2);
  Encrypt_Com(buffer2,1,19,2,f);
  printf("%s\n",buffer2);


  
  return 0;
}
