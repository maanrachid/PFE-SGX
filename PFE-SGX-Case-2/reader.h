#ifndef READER_H
#define READER_H

#include "circuit.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

/*
    Description:
        Returns a circuit representing the circuit read from filename.
        Check http://www.cs.bris.ac.uk/Research/CryptographySecurity/MPC/
        for circuit file format
    Requires:
        filename: a valid path to a file that has a valid circuit with the above format
*/
Circuit readBRISCircuit(char filename[]);

#endif