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

// This structure is used to consolidate user parameters.
struct MyParameters
{
  float *startFrequencyPtr;
  float *endFrequencyPtr;
  float *frequencyStepPtr;
  float *sampleRatePtr;
  float *durationPtr;
};

/*****************************************************************************

  Name: getUserArguments

  Purpose: The purpose of this function is to retrieve the user arguments
  that were passed to the program.  Any arguments that are specified are
  set to reasonable default values.

  Calling Sequence: exitProgram = getUserArguments(parameters)

  Inputs:

    parameters - A structure that contains pointers to the user parameters.

  Outputs:

    exitProgram - A flag that indicates whether or not the program should
    be exited.  A value of true indicates to exit the program, and a value
    of false indicates that the program should not be exited..

*****************************************************************************/
bool getUserArguments(int argc,char **argv,struct MyParameters parameters)
{
  bool exitProgram;
  bool done;
  int opt;

  // Default not to exit program.
  exitProgram = false;

  //_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
  // Default parameters.
  //_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
  // Default to 200Hz.
  *parameters.startFrequencyPtr = 100;

  // Default to 600Hz.
  *parameters.endFrequencyPtr = 600;

  // Default to a 10 Hz frequency step.
  *parameters.frequencyStepPtr = 10;

  // Default to 24000 S/s.
  *parameters.sampleRatePtr = 24000;

  // Default for a 1 second sweep.
  *parameters.durationPtr = 1;
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
        *parameters.startFrequencyPtr = atof(optarg);
        break;
      } // case

      case 'E':
      {
        *parameters.endFrequencyPtr = atof(optarg);
        break;
      } // case

      case 's':
      {
        *parameters.frequencyStepPtr = atof(optarg);
        break;
      } // case

      case 'r':
      {
        *parameters.sampleRatePtr = atof(optarg);
        break;
      } // case

      case 'd':
      {
        *parameters.durationPtr = atof(optarg);
        break;
      } // case

      case 'h':
      {
        // Display usage.
        fprintf(stderr,"./sweep -S startFrequency -E endFrequency\n");
        fprintf(stderr,"        -s frequencyStep -r sampleRate\n");
        fprintf(stderr,"        -d duration\n");

        // Indicate that program must be exited.
        exitProgram = true;
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

  return (exitProgram);

} // getUserArguments

//*************************************************************************
// Mainline code.
//*************************************************************************
int main(int argc,char **argv)
{
  int i, j;
  bool exitProgram;
  float iValue, qValue;
  int16_t cosineValue;
  float sampleRate;
  float startFrequency, endFrequency;
  float currentFrequency;
  float duration;
  float frequencyStep;
  int numberOfSamples, samplesPerDwell, numberOfDwells;
  Nco *myNcoPtr;
  struct MyParameters parameters;

  // Set up for parameter transmission.
  parameters.startFrequencyPtr = &startFrequency;
  parameters.endFrequencyPtr = &endFrequency;
  parameters.frequencyStepPtr = &frequencyStep;
  parameters.sampleRatePtr = &sampleRate;
  parameters.durationPtr = &duration;

  // Retrieve the system parameters.
  exitProgram = getUserArguments(argc,argv,parameters);

  if (exitProgram)
  {
    // Bail out.
    return (0);
  } // if

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
