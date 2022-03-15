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
//     ./cosine > -a amplitude -f frequency -r sampleRate
//                -d duration > ncoFileName,
//
// where,
//
//    amplitude - The ampoitude between 0 and 1 inclusive.
//    frequency - frequency in Hz.
//    sampleRate - The sample rate in samples/second.
//    duration - The duration in seconds.
///*************************************************************************

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <unistd.h>
#include <math.h>

#include "Nco.h"

// This structure is used to consolidate user parameters.
struct MyParameters
{
  float *amplitudePtr;
  float *frequencyPtr;
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
  // Default 1/2 scale.
  *parameters.amplitudePtr = 0.5;

  // Default to 200Hz.
  *parameters.frequencyPtr = 200;

  // Default to 24000 S/s.
  *parameters.sampleRatePtr = 24000;

  // Default for a 1 second signal.
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
    opt = getopt(argc,argv,"a:f:r:d:h");

    switch (opt)
    {
      case 'a':
      {
        *parameters.amplitudePtr = atof(optarg);
        *parameters.amplitudePtr = fabs(*parameters.amplitudePtr);
        break;
      } // case

      case 'f':
      {
        *parameters.frequencyPtr = atof(optarg);
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
        fprintf(stderr,"./cosine -a amplitude -f frequency -r sampleRate"
                " -d duration\n");

        // Indicate that program must be exited.
        exitProgram = true;
        break;
      } // case

      case -1:
      {
        // All options consumed, so bail out.
        done = true;
      } // case
    } // switch

  } // while
  //_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/

  return (exitProgram);

} // getUserArguments

//*************************************************************************
// Mainline code.
//*************************************************************************
int main(int argc,char **argv)
{
  int i;
  bool exitProgram;
  float iValue, qValue;
  int16_t cosineValue;
  float amplitude;
  float frequency;
  float sampleRate;
  float duration;
  int numberOfSamples;
  Nco *myNcoPtr;
  struct MyParameters parameters;

  // Set up for parameter transmission.
  parameters.amplitudePtr = &amplitude;
  parameters.frequencyPtr = &frequency;
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

  // Instantiate an NCO.
  myNcoPtr = new Nco(sampleRate,frequency);

  for (i = 0; i < numberOfSamples; i++)
  {
    // Get the next sample pair.
    myNcoPtr->run(&iValue,&qValue);

    // Convert to integer and scale.
    cosineValue = (int16_t)(iValue * amplitude * 32767);

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
