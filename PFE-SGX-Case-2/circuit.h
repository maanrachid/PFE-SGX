#ifndef CIRCUIT_H
#define CIRCUIT_H

#include <stdlib.h>

typedef int wlabel;     // wire label
typedef char ttabel;    // truth tabel (e.g. And = 0001 = 8, Or = 0111 = 14)
typedef char bit;       // 0 or 1 only

/*
    Description:
        A structure that defines a Gate
    Elements:
        a, b: are input wire labels of the gate
        out: is the output wire label of the gate
        T: is the truth table of the gate
*/
typedef struct Gate {
    wlabel a, b, out;
    ttabel T;
} Gate;

/* 
    Description:
        A structure that defines a Circuit.
    Elements:
        gates: a topoligcally sorted array of gates
        numGates: the number of gates in the circuit
        numWires: number of wires used by the circuit
        inpSize: the number of input bits; these have to be the first wire labels in the circuit
        outSize: the number of output bits; these don't have to be the last wire labels
        outWLabels: the list of this circuit's final output wire labels
*/
typedef struct Circuit {
  Gate *gates;
  int numGates;
  int numWires;
  int num_of_clients;
  int inpSize;
  int *inpSizes;
  int outSize;
  int *outWLabels;
} Circuit;

bit evalGate(Gate G, bit a, bit b);


/*
    Description:
        Evaluates C on the given input, writing the result to output
    Requires:
        C: a valid and correct Circuit
        input: are the appropriate input bits to the circuit
        output: is predefined array of the appropriate output size to fill
*/
void evalCircuit(Circuit C, bit input[], bit output[]);

/*
    Description:
        Frees outWLabels and gates inside of C but not C itself
*/
void freeCircuit(Circuit C);


#endif
