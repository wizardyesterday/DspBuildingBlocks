//************************************************************************
// file name: testCtcssDetector.cc
//************************************************************************
//_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
// This program tests the CtcssDetector class.
// To build, type,
//  g++ -o testCtcssDetector tesCtcssDetector.cc Decimator_int16.cc -lm
//
// To run, type,
// ./testCtcssDetector -s <samplerate> -t <detectionthreshold> -g <gain>
//
// where,
//
// -s (samplerate):
//    sample rate of the audio signal in S/s.
//
// -t (threshold):
//    threshold of the CTCSS detector.
//
// -g (gain):
//    gain of the CTCSS detector.
//
// Note that all flags are options.  If any flag is omitted, a
// reasonable default value will be used.
//_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <ctype.h>
#include <math.h>

#include "CtcssDetector.h"

using namespace std;

//************************************************************
// Structures.
//************************************************************
// This structure is used to consolidate user parameters.
struct MyParameters
{
  float *sampleRatePtr;
  float *thresholdPtr;
  float *gainPtr;
};
//************************************************************

//************************************************************
// These are attributes in a baseband processor class.
//************************************************************
CtcssDetector *myCtcssPtr;

bool ctcssDataAvailable;
int16_t ctcssFrequency;
float sampleRate;
float threshold;
float gain;

int16_t pcmBuffer[32768];
//************************************************************

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
    of false indicates that the program should not be exited.

*****************************************************************************/
bool getUserArguments(int argc,char **argv,struct MyParameters parameters)
{
  bool exitProgram;
  bool done;
  int opt;
  float temporaryValue;

  // Default not to exit program.
  exitProgram = false;

  //_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
  // Default parameters.
  //_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
  // Default to 8000S/s.
  *parameters.sampleRatePtr = 8000;

  // Default to a reasonable threshold.
  *parameters.thresholdPtr = 1000;

  // Scale to compensate for the gain of a DFT.
  *parameters.gainPtr = 1.0f / 32768.0f;
  //_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/

  // Set up for loop entry.
  done = false;

  //_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
  // Retrieve the command line arguments.
  //_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
  while (!done)
  {
    // Retrieve the next option.
    opt = getopt(argc,argv,"s:t:g:h");

    switch (opt)
    {
      case 's':
      {
        // Retrieve for error checking.
        temporaryValue = atof(optarg);

        if (temporaryValue >= 0)
        {
          *parameters.sampleRatePtr = temporaryValue;
        } // if
        break;
      } // case

      case 't':
      {
        // Retrieve for error processing.
        temporaryValue = atof(optarg);

        if (temporaryValue >= 0)
        {
          *parameters.thresholdPtr = temporaryValue;
        } // if
        break;
      } // case

      case 'g':
      {
        // Retrieve for error processing.
        temporaryValue = atof(optarg);

        if (temporaryValue >= 0)
        {
          *parameters.gainPtr = temporaryValue;
        } // if
        break;
      } // case

      case 'h':
      {
        // Display usage.
        fprintf(stderr,"./testCtcssDetector -s samplerate -t threshold"
                       " -g gain\n");

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
  //_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/

  return (exitProgram);

} // getUserArguments

//***********************************************************
// Mainline code.
//***********************************************************

int main(int argc,char **argv)
{
  bool exitProgram;
  bool done;
  uint32_t count;
  struct MyParameters parameters;

  // Initially clear.
  ctcssDataAvailable = false;
  ctcssFrequency = -1;

  //_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
  // Retrieve command line arguments.
  //_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
  // Set up for parameter transmission.
  parameters.sampleRatePtr = &sampleRate;
  parameters.thresholdPtr = &threshold;
  parameters.gainPtr = &gain;

  // Retrieve the system parameters.
  exitProgram = getUserArguments(argc,argv,parameters);

  // Either an invalid parameter occurred or help requested.
  if (exitProgram)
  {
    // Bail out.
    return (0);
  } // if
  //_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/

  //_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
  // Display system configuration.
  //_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
  fprintf(stderr,"Sample Rate: %f\n",sampleRate);
  fprintf(stderr,"Detection Threshold: %f\n",threshold);
  fprintf(stderr,"Detector Gain: %f\n",gain);
  //_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/

  // Instantiate a CTCSS detector with a sample rate of 8000S/s.
  myCtcssPtr = new CtcssDetector(sampleRate);

  // Set the necessary gain.
  myCtcssPtr->setDetectorGain(gain);

  // Try this threshold.
  myCtcssPtr->setDetectorThreshold(threshold);

  // Let's show what we got.
  myCtcssPtr->displayInternalInformation();

  // Set up for loop entry.
  done = false;

  while (!done)
  {
    // Read a block of input samples.
    count = fread(pcmBuffer,2,4000,stdin);

    if (count == 0)
    {
      // We're done.
      done = true;
    } // if
    else
    {
      myCtcssPtr->processData(pcmBuffer,
                              count,
                              &ctcssFrequency,
                              &ctcssDataAvailable);

      if (ctcssDataAvailable)
      {
        fprintf(stderr,"Ctcss Frequency: %d\n",ctcssFrequency);
      } // if
    } // else
  } // while

  // Release resources.
  delete myCtcssPtr;

  return (0);

} // main
