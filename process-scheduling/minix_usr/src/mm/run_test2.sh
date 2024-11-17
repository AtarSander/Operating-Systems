#!/bin/sh
# script commands
cat "$0"
echo "================================="
echo "        TEST SCHEDULE ALGO       "
echo "================================="
./num_sim 10000000 & ./int_sim 12 & ./io_sim 100000 3 & ./int_sim 6 & ./num_sim 10000000 &
sleep 1
./printqueues
