//*************************************************************************
// File name: testNco.cc
//*************************************************************************

//*************************************************************************
// This program tests both the numerically controlled oscillator (NCO).
// The NCO data is written to an output file with the format described
// below.
//
// To run this program type,
// 
//     ./testNco > {frequency] ncoFileName
//
// where, frequency is the frequency in Hz (either positive or negative),
// and ncoFileName is an IQ file represented as (I0,Q0), (I1,Q1),...
//
// If frequency is not specified, a frequency of 200Hz is used.
//*************************************************************************

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

#include "Nco.h"

int main(int argc,char **argv)
{
  int i;
  float iValue, qValue;
  float frequency;
  Nco *myNcoPtr;

  // Default to 200Hz.
  frequency = 200;

  if (argc == 2)
  {
    frequency = atof(argv[1]);
  } // if

  // Create an NCO with a sample rate of 24000S/s and the appropriate frequency.
  myNcoPtr = new Nco(24000,frequency);

  for (i = 0; i < 32768; i++)
  {
    // Get the next sample pair.
    myNcoPtr->run(&iValue,&qValue);

   // Write the samples to stdout
   fwrite(&iValue,sizeof(float),1,stdout);
   fwrite(&qValue,sizeof(float),1,stdout);
  } // for

  // Release resources.
  if (myNcoPtr != NULL)
  {
    delete myNcoPtr;
  } // if

  return (0);

} // main
