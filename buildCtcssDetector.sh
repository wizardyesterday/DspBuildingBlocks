#!/bin/sh

g++ -I include -g -O0 -o ctcssDetector src/ctcssDetector.cc  src/CtcssDetector.cc src/Decimator_int16.cc -lm

exit 0

