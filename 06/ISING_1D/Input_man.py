#!/usr/bin/env python
# coding: utf-8

#serve a manipolare il file input.dat

import sys
import io

if len(sys.argv)  != 3 :
    sys.exit ("Usage: <selected_T*1000> <selected h>")

with open ("input.dat") as file :
    data = file.readlines()

data[0] = str(float(sys.argv[1])/1000)+'\n' #uso valori moltiplicati per 1000 per permettere allo shell script di usare solamente valori interi
data[3] = str(sys.argv[2])+'\n'

with open('input.dat', 'w') as file:
    file.writelines( data )
