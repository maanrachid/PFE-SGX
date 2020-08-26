#include <stdio.h>
#include<string.h>    //strlen
#include<unistd.h>    //write
#include<stdlib.h>
#include "garbledcircuit.h"
//#include "Encryptor.h"

void printkey(key kk);



//  To do:
//      implement the actual random generation in SGX
void readRand(unsigned char *var, int len)
{
    for (int i = 0; i < len; i++)
    {
      var[i] = 0;
    }
}

//  To do:
//      implement the actual AES encryption in SGX
key encAES(key k, char msg[], int msgLen)
{
  /*
    key c;
    for (int i = 0; i < 16; i++)
    {
        if (i < msgLen)
        {
            c.k[i] = k.k[i] ^ msg[i];
        }
        else
        {
            c.k[i] = k.k[i];
        }
    }
    return c; 
  
  */
    
  key c;
  memcpy(&c,msg,msgLen);
  Encrypt_Enclave2((char*)&c,msgLen,1,(char*)k.k);
  
  for(int i=msgLen;i<16;i++)
    c.k[i]=k.k[i];
  

  
  return c;
    
}

key genKey()
{
    key k;
    readRand(k.k, 16);
    return k;
}

void xorChars(char s1[], char s2[], int len, char res[])
{
    for (int i = 0; i < len; i++)
        res[i] = s1[i] ^ s2[i];
}

key xorKeys(key k0, key k1)
{
    key res;
    xorChars(k0.k, k1.k, 16, res.k);
    return res;
}

key fixedAESKeyEnc(key A, key B, int gindex, key X, key fixedKey)
{
    key K = xorKeys(A, B);
    char T[16];
    int *Tint = (int *)(&T[0]);
    *Tint = gindex;
    xorChars(K.k, T, 16, K.k);

    return xorKeys(encAES(fixedKey, K.k, 16), xorKeys(K, X));
}

key fixedAESKeyDec(key A, key B, int gindex, key C, key fixedKey)
{
    key K = xorKeys(A, B);
    char T[16];
    int *Tint = (int *)(&T[0]);
    *Tint = gindex;
    xorChars(K.k, T, 16, K.k);

    return xorKeys(encAES(fixedKey, K.k, 16), xorKeys(K, C));
}

bit readLastBit(key k)
{
    return k.k[0] & 1;
}

void writeLastBit(key *k, bit b)
{
    if (readLastBit(*k) != b)
        k->k[0] ^= 1;
}

GGate garbleGate(Gate g, int gindex, key zeroKey, key oneKey, key fixedKey)
{
    GGate gg;

   
    return gg;
}

GCircuit garbleCircuit(Circuit C, bit input[], key garbledInput[], key fixedKey, key outGarblings[])
{
  GCircuit GC;
  return GC;
}

key evalGarbledGate(GGate GG, int gindex, key A, key B, key fixedKey)
{
    // For point and permute to decide which cipher to decrypt
    bit abit = readLastBit(A);
    bit bbit = readLastBit(B);
    key ct = GG.GT.ciphers[2 * abit + bbit];

    //printkey(A);
    //printkey(B);
    //printkey(ct);
    //printkey(fixedKey);
   
      
    //printf("%d\n",gindex);
    return fixedAESKeyDec(A, B, gindex, ct, fixedKey);
}

void evalGCircuit(GCircuit GC, key input[], key output[], key fixedKey)
{
    // wire garbled values in the circuit
    key *wires = (key *)calloc(GC.numWires, sizeof(key));
    if (wires==NULL) printf("malloc problem");
    
    // adding the garbled input the circuit wires
    for (int i = 0; i < GC.inpSize; i++)
    {
        wires[i] = input[i];
    }

    // evaluating the garbled circuit gate by gate
    for (int i = 0; i < GC.numGates; i++)
    {
        GGate gg = GC.ggates[i];
        wires[gg.out] = evalGarbledGate(gg, i, wires[gg.a], wires[gg.b], fixedKey);
    }

    // writing output wires
    for (int i = 0; i < GC.outSize; i++)
    {
        output[i] = wires[GC.outWLabels[i]];
    }

    free(wires);
}

int compareKeys(key k1, key k2)
{
  return 0;
}

void decodeOutput(key goutput[], key outGarblings[], bit output[], int outSize)
{
  
}

void freeGCircuit(GCircuit GC)
{
    free(GC.ggates);
    free(GC.outWLabels);
}

void printkey(key kk){
  for (int i = 0; i < sizeof(key); i++)
    {
      printf("%d",kk.k[i]);
    }
  printf("\n");
  

}
