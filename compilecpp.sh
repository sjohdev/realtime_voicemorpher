#!/bin/bash
# This script compiles a .cpp file into an executable
# Usage: ./compilecpp.sh filename


# Prompt the user for the cpp filenames:
#echo "Gimme a cpp filename (skip the file extension):"
#read filename
filename=$1
echo "Compiling file: $filename.cpp" 

# compile the file using g++: 
# cpp code to Preprocessed cpp code
g++ -E $filename.cpp -o $filename.i
# Preprocessed code to Assembly code
g++ -S $filename.i -o $filename.s
# Assembly code to Binary code
g++ -c $filename.cpp -o $filename.o
#Linking
g++ $filename.o -o $filename
