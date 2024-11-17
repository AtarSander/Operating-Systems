#!/bin/sh
# script commands
cat "$0"
echo "================================="
echo "        TEST RR QUEUE            "
echo "================================="
./num_sim 100000000 4 & ./num_sim 1000 4 & ./num_sim 10000000 4 &
./printqueues
