#include "circuit.h"
#include <stdio.h>

/*
    Description:
        Returns the evaluation of gate G on input bits a and b
    Requires:
        G: a valid Gate
        a, b: valid input bits i.e. 0 or 1 only
*/
bit evalGate(Gate G, bit a, bit b)
{
    return (G.T >> a + a + b) & 1;
}

void evalCircuit(Circuit C, bit input[], bit output[])
{
    bit wires[C.numWires]; // wire values in the circuit

    // adding input the circuit wires
    for (int i = 0; i < C.inpSize; i++)
    {
        wires[i] = input[i];
    }

    // evaluating the circuit gate by gate
    for (int i = 0; i < C.numGates; i++)
    {
        Gate g = C.gates[i];
        wires[g.out] = evalGate(g, wires[g.a], wires[g.b]);
    }

    // writing output wires
    for (int i = 0; i < C.outSize; i++)
    {
        output[i] = wires[C.outWLabels[i]];
    }
}

void freeCircuit(Circuit C)
{
    free(C.gates);
    free(C.outWLabels);
}
