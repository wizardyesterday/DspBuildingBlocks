//*************************************************************************
// File name: cosine.cc
//*************************************************************************

//*************************************************************************
// This program tests both the numerically controlled oscillator (NCO).
// The NCO data is written to an output file with the format described
// below.  Note that only the in-phase component of the NCO output is
// used, therefore the output file represents a cosine waveform.
//
// To run this program type,
// 
//     ./cosine > -f frequency -r sampleRate -d duration > ncoFileName,
//
// where,
//
//    frequency - frequency in Hz.
//    sampleRate - The sample rate in samples/second.
//    duration - The duration in seconds.
///*************************************************************************

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <unistd.h>

#include "Nco.h"

int main(int argc,char **argv)
{
  int i;
  int opt;
  bool done;
  float iValue, qValue;
  int16_t cosineValue;
  float frequency;
  float sampleRate;
  float duration;
  int numberOfSamples;
  Nco *myNcoPtr;

  //_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
  // Default parameters.
  //_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
  // Default to 200Hz.
  frequency = 200;

  // Default to 24000 S/s.
  sampleRate = 24000;

  // Default for a 1 second signal.
  duration = 1;
  //_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/

  // Set up for loop entry.
  done = false;

  //_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
  // Retrieve the command line arguments.
  //_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
  while (!done)
  {
    // Retrieve the next option.
    opt = getopt(argc,argv,"f:r:d:h");

    switch (opt)
    {
      case 'f':
      {
        frequency  = atof(optarg);
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
        fprintf(stderr,"./cosine -f startFrequency -r sampleRate");
        fprintf(stderr," -d duration\n");

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

  // Create an NCO with a sample rate of 24000S/s and the appropriate frequency.
  myNcoPtr = new Nco(sampleRate,frequency);

  for (i = 0; i < numberOfSamples; i++)
  {
    // Get the next sample pair.
    myNcoPtr->run(&iValue,&qValue);

    // Convert to integer and scale.
    cosineValue = (int16_t)(iValue * 32767);

   // Write the samples to stdout
   fwrite(&cosineValue,sizeof(int16_t),1,stdout);
  } // for

  // Release resources.
  if (myNcoPtr != NULL)
  {
    delete myNcoPtr;
  } // if

  return (0);

} // main
