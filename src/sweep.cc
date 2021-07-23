//*************************************************************************
// File name: testNco.cc
//*************************************************************************

//*************************************************************************
// This program tests both the numerically controlled oscillator (NCO).
// The NCO data is written to an output file with the format described
// below.  Note that only the in-phase component of the NCO output is
// used, therefore the output file represents a cosine waveform.
//
// To run this program type,
// 
//     ./sweep > {startFrequency] {endFrequency} {frequencyStep}
//               {duration} > ncoFileName
//
// where, frequency is the frequency in Hz (either positive or negative),
// and ncoFileName is a sample file represented as S0, S1,...
//
// If frequency is not specified, a frequency of 200Hz is used.
//*************************************************************************

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

#include "Nco.h"

int main(int argc,char **argv)
{
  int i, j;
  float iValue, qValue;
  int16_t cosineValue;
  float sampleRate;
  float startFrequency, endFrequency;
  float currentFrequency;
  float duration;
  float frequencyStep;
  int numberOfSamples, samplesPerDwell, numberOfDwells;
  Nco *myNcoPtr;

  // Set system sample rate.
  sampleRate = 24000;

  // Default to 200Hz.
  startFrequency = 100;

  // Default to 500Hz.
  endFrequency = 600;

  // Default for a 1 second sweep.
  duration = 1;

//  if (argc == 2)
//  {
//    frequency = atof(argv[1]);
//  } // if

  // We derive this.
  numberOfSamples = (int)(sampleRate * duration);

  // Let's discretize the sweep.
  frequencyStep = 10;

  // Number of dwells is a dependent variable.
  numberOfDwells = (int)((endFrequency - startFrequency) / frequencyStep);

  // This is derived.
  samplesPerDwell = numberOfSamples / numberOfDwells;

  // Create an NCO with the appropriate sample rate and frequency.
  myNcoPtr = new Nco(sampleRate,200);

  // Initial value of the frequency.
  currentFrequency = startFrequency;

  //_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
  // The outer loop is responsible for stepping
  // through all of the frequencies of the sweep.
  //_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
  for (i = 0; i < numberOfDwells; i++)
  {
    // Update the frequency.
    myNcoPtr->setFrequency(currentFrequency);

    // Update to the next increment.
    currentFrequency += frequencyStep;

    //_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
    // The inner loop generates all of the samples
    // at the current frequency.
    //_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
    for (j = 0; j < samplesPerDwell; j++)
    {
      // Get the next sample pair.
      myNcoPtr->run(&iValue,&qValue);

      // Convert to integer and scale.
      cosineValue = (int16_t)(iValue * 32767);

     // Write the samples to stdout
     fwrite(&cosineValue,sizeof(int16_t),1,stdout);
    } // for
  } // for

  // Release resources.
  if (myNcoPtr != NULL)
  {
    delete myNcoPtr;
  } // if

  return (0);

} // main
