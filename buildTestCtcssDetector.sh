#!/bin/sh

g++ -I include -g -O0 -o testCtcssDetector src/testCtcssDetector.cc  src/CtcssDetector.cc src/Decimator_int16.cc -lm

exit 0

