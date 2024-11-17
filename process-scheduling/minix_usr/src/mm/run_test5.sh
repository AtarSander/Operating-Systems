#!/bin/sh
# script commands
cat "$0"
echo "================================="
echo "        TEST UPGRADE PRIORITY    "
echo "================================="
./num_sim 100000000000000000000 4 & 
./int_sim 10 5 &
sleep 1
./printqueues
