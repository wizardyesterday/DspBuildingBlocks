#!/bin/sh
#*****************************************************************************
# File name: buildTest.sh
#*****************************************************************************
# This build script creates the testNco app.
# Chris G. 07/23/2021
#*****************************************************************************
g++ -I include -g -O0 -o testNco src/testNco.cc src/Nco.cc src/PhaseAccumulator.cc

