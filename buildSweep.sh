#!/bin/sh
#*****************************************************************************
# File name: buildSweep.sh
#*****************************************************************************
# This build script creates the sweeper app.
# Chris G. 07/23/2021
#*****************************************************************************
g++ -I include -g -O0 -o sweep src/sweep.cc src/Nco.cc src/PhaseAccumulator.cc

