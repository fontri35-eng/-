#!/bin/bash
set -e
cd "$(dirname "$0")"
g++ -std=c++17 -Wall -Wextra -pedantic main.cpp -o fizkat
./fizkat
