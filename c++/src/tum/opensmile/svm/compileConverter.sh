#!/bin/sh

echo g++ -g -DNOSMILE -c svm.cpp -o svm.o
g++ -g -DNOSMILE -c svm.cpp -o svm.o
echo g++ -g -DNOSMILE modelconverter.cpp svm.o -o modelconverter
g++ -g -DNOSMILE modelconverter.cpp svm.o -o modelconverter

