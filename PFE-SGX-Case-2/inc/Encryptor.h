#ifndef _Encryptor_h
#define _Encryptor_h


#include "ippcp.h"
#include "ippcore.h"
#include <string.h>
#include <stdlib.h>



void encrypt_ipp(char message[], int size,int op,char * IV_counter);



void encrypt_ipp2(char message[], int size,int op,char * IV_counter);

void Encrypt_Com(char message[],int start, int size,int op,long long IV_counter);

void Encrypt_Enclave(char message[], int size,int op,char* IV_counter);

void Encrypt_Enclave2(char message[], int size,int op,char* IV_counter);


#endif 
