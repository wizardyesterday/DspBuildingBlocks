//*************************************************************************
// File name: nco.cc
//*************************************************************************

//*************************************************************************
// This program tests both the numerically controlled oscillator (NCO).
// The NCO data is written to an output file with the format described
// below.
//
// To run this program type,
// 
//     ./nco > -a amplitude -f frequency -r sampleRate -d duration
//                  -n numberofbits > ncoFileName,
//
// where,
//
//    amplitude - the amplitude of the waveform.
//    frequency - frequency in Hz.
//    sampleRate - The sample rate in samples/second.
//    duration - The duration in seconds.
//    numberOfBits - number of bits in signed integer.
//*************************************************************************

#include <stdio.h>
#include <stdint.h>
#include <unistd.h>
#include <stdlib.h>

#include "Nco.h"

// This structure is used to consolidate user parameters.
struct MyParameters
{
  float *amplitudePtr;
  float *frequencyPtr;
  float *sampleRatePtr;
  float *durationPtr;
  int *numberOfBitsPtr;
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
  // Default to unity amplitude
  *parameters.amplitudePtr = 1;

  // Default to 200Hz.
  *parameters.frequencyPtr = 200;

  // Default to 32000 S/s.
  *parameters.sampleRatePtr = 32000;

  // Default for a 1 second signal.
  *parameters.durationPtr = 1;

  // Default to floating point output.
  *parameters.numberOfBitsPtr = 0;
  //_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/

  // Set up for loop entry.
  done = false;

  //_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
  // Retrieve the command line arguments.
  //_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
  while (!done)
  {
    // Retrieve the next option.
    opt = getopt(argc,argv,"a:f:r:d:n:h");

    switch (opt)
    {
      case 'a':
      {
        *parameters.amplitudePtr = atof(optarg);
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

      case 'n':
      {
        *parameters.numberOfBitsPtr = atoi(optarg);

        break;
      } // case

      case 'h':
      {
        // Display usage.
        fprintf(stderr,"./testNco -a amplitude-f frequency -r sampleRate "
                " -d duration -n [8 | 16 | 0 (floating point)]\n");

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
  int8_t iByteValue, qByteValue;
  int16_t iIntValue, qIntValue;
  float amplitude;
  float frequency;
  float sampleRate;
  float duration;
  int numberOfBits;
  int numberOfSamples;
  Nco *myNcoPtr;
  struct MyParameters parameters;

  // Set up for parameter transmission.
  parameters.amplitudePtr = &amplitude;
  parameters.frequencyPtr = &frequency;
  parameters.sampleRatePtr = &sampleRate;
  parameters.durationPtr = &duration;
  parameters.numberOfBitsPtr = &numberOfBits;

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

    switch (numberOfBits)
    {

      case 8:
      {
        // Set up the 8-bit samples.
        iByteValue = (int8_t)(iValue * amplitude * 127);
        qByteValue = (int8_t)(qValue * amplitude * 127);

        // Write 8-bit samples to stdout
        fwrite(&iByteValue,sizeof(int8_t),1,stdout);
        fwrite(&qByteValue,sizeof(int8_t),1,stdout);
        break;
      } // case

      case 16:
      {
        // Set up the 16-bit samples.
        iIntValue = (int16_t)(iValue * amplitude * 32767);
        qIntValue = (int16_t)(qValue * amplitude * 32767);

        // Write 16-bit samples to stdout
        fwrite(&iIntValue,sizeof(int16_t),1,stdout);
        fwrite(&qIntValue,sizeof(int16_t),1,stdout);
        break;
      } // case

      default:
      {
        // Write floating point samples to stdout
        fwrite(&iValue,sizeof(float),1,stdout);
        fwrite(&qValue,sizeof(float),1,stdout);
        break;
      } // case
    } // switch

 } // for

  // Release resources.
  if (myNcoPtr != NULL)
  {
    delete myNcoPtr;
  } // if

  return (0);

} // main
