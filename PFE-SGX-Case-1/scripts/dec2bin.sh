#!/bin/bash

# first input is the decimal to convert
# second input is the length of the binary to output

# echo "obase=2; $1" converts the input to binary
# printf "%0$2s\n" prefixes the binary with spaces till its length becomes $2
# tr ' ' '0' replaces spaces with zeroes
printf "%0$2s\n" $(echo "obase=2; $1" | bc) | tr ' ' '0' | rev