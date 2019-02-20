#! /bin/bash                                                                                                  
 
PORT=$1
NUM_PLAYERS=$2
NUM_HOPS=$3
MACHINE=$4
./ringmaster $PORT $NUM_PLAYERS $NUM_HOPS &
 
sleep 2
for ((i=1; i<=$NUM_PLAYERS; i++))
do
./player $MACHINE $PORT &
done

wait










