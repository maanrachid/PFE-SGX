#include "reader.h"

Circuit readBRISCircuit(char filename[])
{

    FILE *f;
    Circuit C;
    int linpSize, rinpSize;

    // open file
    f = fopen(filename, "r");
    if (f == NULL)
    {
        fprintf(stderr, "Can't open input file %s\n", filename);
        exit(1);
    }

    // Read first line for meta data
    fscanf(f, "%d %d", &C.numGates, &C.numWires);

    // Read number of inputs
    int numInputs;
    fscanf(f, " %d ", &numInputs);


    // Read all inputs
    C.inpSize=0;
    C.num_of_clients = numInputs;
    C.inpSizes = (int*)malloc(numInputs*sizeof(int));
    for(int i=0;i<numInputs;i++){
      fscanf(f, " %d ", &C.inpSizes[i]);
      C.inpSize += C.inpSizes[i];
    }
      
    // Read second line for meta data
    fscanf(f, "%d", &C.outSize);
    //C.inpSize = linpSize + rinpSize;

    // Read the gates one by one
    C.gates = (Gate *)malloc(sizeof(Gate) * C.numGates);
    int gateInpSize, gateOutSize;
    for (int i = 0; i < C.numGates; i++)
    {
        char gtype[4];
        fscanf(f, "%d", &gateInpSize);

        if (gateInpSize == 1)
        { // not gate, replace it with a Nand
            fscanf(f, "%d %d %d %s3", &gateOutSize, &C.gates[i].a, &C.gates[i].out, gtype);
            C.gates[i].b = C.gates[i].a;
            C.gates[i].T = 7;
        }
        else if (gateInpSize == 2)
        { // And or Xor gate
            fscanf(f, "%d %d %d %d %s3", &gateOutSize, &C.gates[i].a, &C.gates[i].b, &C.gates[i].out, gtype);
            if (strcmp(gtype, "XOR") == 0)
            {
                C.gates[i].T = 6;
            }
            else if (strcmp(gtype, "AND") == 0)
            {
                C.gates[i].T = 8;
            }
            else
            {
                fprintf(stderr, "Invalid gate type %s, only (XOR, AND, INV)\n", gtype);
                exit(1);
            }
        }
        else
        {
            fprintf(stderr, "Invalid gate input size %d\n", gateInpSize);
            exit(1);
        }
    }

    // Prepare output wire labels, they are just the last wires in the circuit
    C.outWLabels = (wlabel *)malloc(sizeof(wlabel) * C.outSize);
    for (int i = 0; i < C.outSize; i++)
    {
        C.outWLabels[i] = C.numWires - C.outSize + i;
    }

    fclose(f);

    return C;
}
