#!/bin/sh
#*****************************************************************************
# This build script creates the cosine app.
# Chris G. 07/23/2021
#*****************************************************************************
g++ -I include -g -O0 -o cosine src/cosine.cc src/Nco.cc src/PhaseAccumulator.cc

