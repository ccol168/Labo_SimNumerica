#!/bin/bash 

#shell script che lancia la simulazione per STEPS diverse temperature 

COUNT=0
STEPS=40
while [ $COUNT -lt $STEPS ]; do
    TEMP=$(( 500+(COUNT)*1500/(STEPS-1) ))
    ./Input_man.py $TEMP 0.00
    ./Monte_Carlo_ISING_1D.exe
    ./Input_man.py $TEMP 0.02
    ./Monte_Carlo_ISING_1D.exe #per ogni T lancio la sim due volte, una a h=0.00 e una ad h=0.02 per calcolare M
    let COUNT=COUNT+1
done