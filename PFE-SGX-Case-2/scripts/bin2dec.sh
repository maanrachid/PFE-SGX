#!/bin/bash

# input is the binary from most significant to least

# converts $1 to decimal from base 2
echo "ibase=2; $(echo $1 | rev)" | bc