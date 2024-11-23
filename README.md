# Operating-Systems
This repository contains tasks completed as part of the Operating Systems course at Warsaw University of Technology. The course labs were based on the `MINIX 2.0.3` operating system.
## Description
- `semaphores-synchronization` and `monitors-synchronization`: Standalone programs implementing and testing common synchronization methods.
- `process-scheduling`, `memory-management`, and `file-system`: These tasks involve modifications or extensions of existing `MINIX` features. The corresponding directories include only the modified `MINIX` source files.

Monitors synchronization was implemented in `C++`. All other tasks were implemented in `C`.
## Usage
- `semaphores-synchronization` and `monitors-synchronization` can be compiled and executed normally. Each directory contains descriptions of the available tests that can be run via command line arguments.
- To run the `process-scheduling`, `memory-management`, and `file-system` modifications, you need the QEMU emulator and a MINIX image. You can set these up using the script available at [minix-toolkit](https://github.com/peku33/minix-toolkit). After setup, manually replace the source files, recompile the OS, and follow the test instructions provided in each directory.
