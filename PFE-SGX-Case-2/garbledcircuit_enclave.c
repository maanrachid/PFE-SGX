#include "garbledcircuit.h"
#include "sgx_trts.h"
#include "sgx_tcrypto.h"
//#include "Encryptor.h"

void random_array(int arr[],int size);
int random_item(int size);


sgx_aes_ctr_128bit_key_t aes_key;
uint8_t IV[16];


//  To do:
//      implement the actual random generation in SGX

void readRand( char *var, int len)
{
  for (int i = 0; i < len; i++)
    {
      var[i] = random_item(255);
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


    //SGX Encryption
  /*
  key c;

  for(int i=0;i<16;i++){
    IV[i]= 1;
  }
  
  const char *p_src = msg;
  const int src_len = msgLen;
  uint8_t *p_ctr=IV;   // initialization vector
  const int ctr_inc_bits=8;
  key *p_dst = &c;

  sgx_aes_ctr_encrypt((sgx_aes_ctr_128bit_key_t *)&k,(uint8_t*)p_src,src_len,
		      p_ctr,ctr_inc_bits,(uint8_t*)p_dst);

  
  for(int i=msgLen;i<16;i++)
    c.k[i]=k.k[i];
  
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

  // copy meta data
  gg.a = g.a;
  gg.b = g.b;
  gg.out = g.out;

  // generate 0 and 1 garblings\keys for wire "a"
  key a0 = encAES(zeroKey, (char*)(&gg.a), sizeof(wlabel));
  key a1 = encAES(oneKey, (char*)(&gg.a), sizeof(wlabel));

  // generate 0 and 1 garblings\keys for wire "b"
  key b0 = encAES(zeroKey, (char*)(&gg.b), sizeof(wlabel));
  key b1 = encAES(oneKey, (char*)(&gg.b), sizeof(wlabel));

  // generate 0 and 1 garblings\keys for wire "out"
  key out0 = encAES(zeroKey, (char*)(&gg.out), sizeof(wlabel));
  key out1 = encAES(oneKey, (char*)(&gg.out), sizeof(wlabel));

  // For point and permute
  bit a0bit = readLastBit(a0);
  bit a1bit = !a0bit;
  writeLastBit(&a1, a1bit);
  key as[2];
  as[a0bit] = a0;
  as[a1bit] = a1;

  bit b0bit = readLastBit(b0);
  bit b1bit = !b0bit;
  writeLastBit(&b1, b1bit);
  key bs[2];
  bs[b0bit] = b0;
  bs[b1bit] = b1;

  bit out0bit = readLastBit(out0);
  bit out1bit = !out0bit;
  writeLastBit(&out1, out1bit);
  key outs[2] = {out0, out1};

  gg.GT.ciphers[2 * a0bit + b0bit] = fixedAESKeyEnc(as[a0bit], bs[b0bit], gindex, outs[evalGate(g, 0, 0)], fixedKey);
  gg.GT.ciphers[2 * a0bit + b1bit] = fixedAESKeyEnc(as[a0bit], bs[b1bit], gindex, outs[evalGate(g, 0, 1)], fixedKey);
  gg.GT.ciphers[2 * a1bit + b0bit] = fixedAESKeyEnc(as[a1bit], bs[b0bit], gindex, outs[evalGate(g, 1, 0)], fixedKey);
  gg.GT.ciphers[2 * a1bit + b1bit] = fixedAESKeyEnc(as[a1bit], bs[b1bit], gindex, outs[evalGate(g, 1, 1)], fixedKey);
  return gg;
}

GCircuit garbleCircuit(Circuit C, bit input[], key garbledInput[], key fixedKey, key outGarblings[])
{
  GCircuit GC;

  // copy meta data
  GC.numGates = C.numGates;
  GC.numWires = C.numWires;
  GC.inpSize = C.inpSize;
  GC.outSize = C.outSize;
  GC.ggates = (GGate *)calloc(GC.numGates, sizeof(GGate));
  GC.outWLabels = (wlabel *)calloc(GC.outSize, sizeof(wlabel));
  for (int i = 0; i < GC.outSize; i++)
    {
      GC.outWLabels[i] = C.outWLabels[i];
    }

  // generate the two keys used to generate the 0 and 1 wire labels
  key zk = genKey(); // used in AES to generate 0 wire labels
  key ok = genKey(); // used in AES to generate 1 wire labels

  // garble the gates one by one
  for (int i = 0; i < GC.numGates; i++)
    {
      GC.ggates[i] = garbleGate(C.gates[i], i, zk, ok, fixedKey);
    }

  // garble input
  for (int i = 0; i < GC.inpSize; i++)
    {
      char *msg = (char *)(&i);
      key zeroGarbling = encAES(zk, msg, sizeof(wlabel));

      if (input[i] == 0)
        {
	  garbledInput[i] = zeroGarbling;
        }
      else if (input[i] == 1)
        {
	  // if input bit is 1 then we may need to change the first bit
	  // for point and permute
	  garbledInput[i] = encAES(ok, (char*)(&i), sizeof(wlabel));
	  writeLastBit(&(garbledInput[i]), !readLastBit(zeroGarbling));
        }
      else
        {
	  //printf("invalid input %d\n", input[i]);
	  //exit(1);
	  return GC;
        }
    }

  // get the garbling representations of the circuit's output wires
  for (int i = 0; i < GC.outSize; i++)
    {
      // Zero garbling
      outGarblings[2 * i] = encAES(zk, (char*)(&GC.outWLabels[i]), sizeof(wlabel));

      // One garbling
      outGarblings[2 * i + 1] = encAES(ok, (char*)(&GC.outWLabels[i]), sizeof(wlabel));

      // Fix one garbling for point and permute
      writeLastBit(&(outGarblings[2 * i + 1]), !readLastBit(outGarblings[2 * i]));
    }

  GC.test = -1;
  return GC;
}

key evalGarbledGate(GGate GG, int gindex, key A, key B, key fixedKey)
{
  // For point and permute to decide which cipher to decrypt
  bit abit = readLastBit(A);
  bit bbit = readLastBit(B);
  key ct = GG.GT.ciphers[2 * abit + bbit];
  return fixedAESKeyDec(A, B, gindex, ct, fixedKey);
}

void evalGCircuit(GCircuit GC, key input[], key output[], key fixedKey)
{
  // wire garbled values in the circuit
  key *wires = (key *)calloc(GC.numWires, sizeof(key));

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


void random_array(int arr[],int size){
  for(int i=0;i<size;i++){
    uint32_t rand;
    sgx_read_rand((unsigned char *)&rand,4);
    arr[i]= rand%size;
  }
}

int random_item(int size){
  uint32_t rand;
  sgx_read_rand((unsigned char *)&rand,4);
  return rand%size;
}





int compareKeys(key k1, key k2)
{
  int res = 0;
  for (int i = 0; i < 16; i++)
    {
      if (k1.k[i] != k2.k[i])
	res = 1;
    }
  return res;
}

void decodeOutput(key goutput[], key outGarblings[], bit output[], int outSize)
{
  for (int i = 0; i < outSize; i++)
    {
      if (compareKeys(goutput[i], outGarblings[2 * i]) == 0)
        {
	  output[i] = 0;
        }
      else if (compareKeys(goutput[i], outGarblings[2 * i + 1]) == 0)
        {
	  output[i] = 1;
        }
      else
        {
	  //printf("Invalid garbled output given\n");
	  //exit(1);
	  goutput[0].k[0]='A';
	  return;
        }
    }
}

void freeGCircuit(GCircuit GC)
{
  free(GC.ggates);
  free(GC.outWLabels);
}


