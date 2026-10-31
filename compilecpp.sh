#!/bin/bash
# This script compiles a .cpp file into an executable
# Usage: ./compilecpp.sh filename


# Get the filename from input argument 1:
filename=$1
echo "Compiling file: $filename.cpp" 

# remove old executable:
#rm $filename
# Compiling and linking with included libraries: 
g++ $filename.cpp -lfftw3 -lportaudio -o $filename

