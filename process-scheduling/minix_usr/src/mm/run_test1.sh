#!/bin/sh
# script commands
cat "$0"
cc io_sim.c -o io_sim
cc int_sim.c -o int_sim
cc num_sim.c -o num_sim
echo "================================="
echo "        TEST QUEUES PRIORITY     "
echo "================================="
./num_sim 100000000 5 & ./num_sim 100000000 4 & ./num_sim 100000000 3 &
sleep 1
./printqueues
