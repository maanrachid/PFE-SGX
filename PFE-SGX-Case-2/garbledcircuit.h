#ifndef GARBLEDCIRCUIT_H
#define GARBLEDCIRCUIT_H

#include "circuit.h"
#include <stdio.h>
#include "Encryptor.h"

// a structure that defines a 128 bit key
// this is also used to define ciphertexts as they have the same size
// the structure wrapping is to return it from functions
typedef struct key
{
    char k[16];
} key;

// a Garbled Trurth Table is 4 ciphertexts
typedef struct GTTable
{
    key ciphers[4];
} GTTable;

/*
    Description:
        A structure that defines a Garbled Gate
    Elements:
        a, b: are input wire labels of the gate
        out: is the output wire label of the gate
        GT: is the garbled truth table of the gate
*/
typedef struct GGate
{
    wlabel a, b, out;
    GTTable GT;
} GGate;

/* 
    Description:
        A structure that defines a Garbled Circuit.
    Elements:
        ggates: a topoligcally sorted array of garbled gates
        numGates: the number of gates in the circuit
        numWires: number of wires used by the circuit
        inpSize: the number of input bits; these have to be the first wire labels in the circuit
        outSize: the number of output bits; these don't have to be the last wire labels
        outWLabels: the list of this circuit's final output wire labels
*/
typedef struct GCircuit
{
    GGate *ggates;
    int numGates;
    int numWires;
    int inpSize;
    int outSize;
    wlabel *outWLabels;
  int test;
} GCircuit;

/*
    Description:
        Returns a garbled version of the input circuit.
    Elements:
        C: a valid circuit
        input: is the input to the circuit
        garbledInput: is an array to fill the garbled input representation with
        fixedKey: is the fixed key used in the encryption of truth tables
        outGarblings: is an empty array of size 2 * C.outSize to be filled by this function with
            the garbling representation of the output wires of the circuit. The output wire label
            C.outWLabel[i] will have its zero garbling in outGarblings[2*i] and its one garblings in
            outGarblings[2*i + 1]
*/
GCircuit garbleCircuit(Circuit C, bit input[], key garbledInput[], key fixedKey, key outGarblings[]);

/*
    Description:
        Writes to output the garbled result of evaluating the garbled circuit GC on the
        garbled input input.
    Elements:
        GC: a valid garbled circuit
        input: an array of garbled input
        output: an empty array of the output's length to be filled by the output
        fixedKey: is the key used to encrypt in AES when garbling the circuit
*/
void evalGCircuit(GCircuit GC, key input[], key output[], key fixedKey);

/*
    Description:
        Decodes the garbled output using the output garblings created during the garbling of the circuit
    Elements:
        goutput: the garbled output produced by running evalGarbledCircuit
        outGarblings: the circuit output garbling representations produced by garlbeCircuit
        output: an empty array of size ouotSize to fill with the decoded output
        outSize: the number of output bits of the circuit
*/
void decodeOutput(key goutput[], key outGarblings[], bit output[], int outSize);

/*
    Description:
        Generates a random 16 byte key
*/
key genKey();

/*
    Description:
        frees memory that was allocated by garbleCircuit function to create GC
*/
void freeGCircuit(GCircuit GC);

#endif
