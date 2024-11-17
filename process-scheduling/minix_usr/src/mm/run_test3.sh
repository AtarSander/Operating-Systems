#!/bin/sh
# script commands
cat "$0"
echo "================================="
echo "        TEST FCFS QUEUE          "
echo "================================="
./num_sim 10000 & ./num_sim 10000000 & ./num_sim 10000000000 &
sleep 1
./printqueues
