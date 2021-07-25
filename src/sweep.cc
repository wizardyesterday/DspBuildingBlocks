//*************************************************************************
// File name: sweep.cc
//*************************************************************************

//*************************************************************************
// This program generates an audio frequency sweep as specified by the
// as specified by the user.  The output is sent to stdout, and the
// waveform is a cosine wave represented as 16-bit little endian PCM
// samples such that -32768 < sampleValue <= 32767.
//
// To run this program type,
// 
//     ./sweep  -S startFrequency -E endFrequency -s frequencyStep
//              -r sampleRate -d duration
//
// where,
//
//    startFrequency - The start frequency in Hz.
//    endFrequency - The end frequency in Hz.
//    frequencyStep - The frequency increment in Hz.
//    sampleRate - The sample rate in samples/second.
//    duration - The duration in seconds.
//*************************************************************************

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <unistd.h>
#include <math.h>

#include "Nco.h"

int main(int argc,char **argv)
{
  int i, j;
  int opt;
  bool done;
  float iValue, qValue;
  int16_t cosineValue;
  float sampleRate;
  float startFrequency, endFrequency;
  float currentFrequency;
  float duration;
  float frequencyStep;
  int numberOfSamples, samplesPerDwell, numberOfDwells;
  Nco *myNcoPtr;

  //_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
  // Default parameters.
  //_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
  // Default to 200Hz.
  startFrequency = 100;

  // Default to 600Hz.
  endFrequency = 600;

  // Default to 24000 S/s.
  sampleRate = 24000;

  // Default for a 1 second sweep.
  duration = 1;

  // Default to a 10 Hz frequency step.
  frequencyStep = 10;
  //_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/

  // Set up for loop entry.
  done = false;

  //_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
  // Retrieve the command line arguments.
  //_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
  while (!done)
  {
    // Retrieve the next option.
    opt = getopt(argc,argv,"S:E:s:r:d:h");

    switch (opt)
    {
      case 'S':
      {
        startFrequency  = atof(optarg);
        break;
      } // case

      case 'E':
      {
        endFrequency  = atof(optarg);
        break;
      } // case

      case 's':
      {
        frequencyStep  = atof(optarg);
        break;
      } // case

      case 'r':
      {
        sampleRate  = atof(optarg);
        break;
      } // case

      case 'd':
      {
        duration  = atof(optarg);
        break;
      } // case

      case 'h':
      {
        // Display usage.
        fprintf(stderr,"./sweep -S startFrequency -E endFrequency\n");
        fprintf(stderr,"        -s frequencyStep -r sampleRate\n");
        fprintf(stderr,"        -d duration\n");

        // Okay this is unstructured.
        return (0);

        break;
      } // case

      case -1:
      {
        // All options consumed, so bail out.
        done = true;
        break;
      } // case
    } // switch

  } // while
  //_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/

  // We derive this.
  numberOfSamples = (int)(sampleRate * duration);

  if (endFrequency < startFrequency)
  {
    // Reverse the direction of the sweep.
    frequencyStep = -frequencyStep;
  } // if

  // Number of dwells is a dependent variable.
  numberOfDwells = 
    (int)(fabs(endFrequency - startFrequency) / fabs(frequencyStep));

  // This is derived.
  samplesPerDwell = numberOfSamples / numberOfDwells;

  // Create an NCO with the appropriate sample rate and frequency.
  myNcoPtr = new Nco(sampleRate,startFrequency);

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
