//************************************************************************
// file name: CtcssDetector.cc
//************************************************************************
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <ctype.h>
#include <string.h>

#include "CtcssDetector.h"

using namespace std;

#define DFT_SCALE_FACTOR (1.0f / 32767.0f)
#define DEFAULT_DETECTOR_THRESHOLD (1000);
#define REQUIRED_NUMBER_OF_SAMPLES (8000)

static int16_t ctcssFrequencies[] =
{
  670,
  693,
  719,
  744,
  770,
  797,
  825,
  854,
  885,
  915,
  948,
  974,
  1000,
  1035,
  1072,
  1109,
  1148,
  1188,
  1230,
  1273,
  1318,
  1365,
  1413,
  1462,
  1514,
  1567,
  1622,
  1679,
  1738,
  1799,
  1862,
  1928,
  2035,
  2065,
  2107,
  2181,
  2257,
  2336,
  2418,
  2503,
  2541
};

//*******************************************************
//  These coefficients realize a lowpass filter with the
//  specifications listed below.
//
//  Pass Band: 0 <= F <= 255 Hz.
//  Transition Band: 255 < F <= 350 Hz.
//  Stop Band: 350 < F < 4000 Hz.
//  Passband Ripple: 1dB
//  Stopband Attenuation: 50dB
//*******************************************************
static float lowpassFilterCoefficients[] =
{
  -0.0014203,
  -0.0026780,
  -0.0016075,
  -0.0031624,
  -0.0032862,
  -0.0042837,
  -0.0047894,
  -0.0055456,
  -0.0060751,
  -0.0066013,
  -0.0069422,
  -0.0071520,
  -0.0071549,
  -0.0069567,
  -0.0065280,
  -0.0058738,
  -0.0049938,
  -0.0039076,
  -0.0026418,
  -0.0012359,
    0.0002605,
    0.0017914,
    0.0032897,
    0.0046887,
    0.0059170,
    0.0069062,
    0.0075956,
    0.0079294,
    0.0078697,
    0.0073895,
    0.0064820,
    0.0051600,
    0.0034572,
    0.0014266,
  -0.0008540,
  -0.0032929,
  -0.0057811,
  -0.0081940,
  -0.0104051,
  -0.0122808,
  -0.0136909,
  -0.0145146,
  -0.0146452,
  -0.0139908,
  -0.0124900,
  -0.0101046,
  -0.0068278,
  -0.0026878,
    0.0022557,
    0.0079126,
    0.0141604,
    0.0208475,
    0.0278055,
    0.0348453,
    0.0417700,
    0.0483778,
    0.0544764,
    0.0598774,
    0.0644159,
    0.0679495,
    0.0703706,
    0.0716016,
    0.0716016,
    0.0703706,
    0.0679495,
    0.0644159,
    0.0598774,
    0.0544764,
    0.0483778,
    0.0417700,
    0.0348453,
    0.0278055,
    0.0208475,
    0.0141604,
    0.0079126,
    0.0022557,
  -0.0026878,
  -0.0068278,
  -0.0101046,
  -0.0124900,
  -0.0139908,
  -0.0146452,
  -0.0145146,
  -0.0136909,
  -0.0122808,
  -0.0104051,
  -0.0081940,
  -0.0057811,
  - .0032929,
  -0.0008540,
    0.0014266,
    0.0034572,
    0.0051600,
    0.0064820,
    0.0073895,
    0.0078697,
    0.0079294,
    0.0075956,
    0.0069062,
    0.0059170,
    0.0046887,
    0.0032897,
    0.0017914,
    0.0002605,
  -0.0012359,
  -0.0026418,
  -0.0039076,
  -0.0049938,
  -0.0058738,
  -0.0065280,
  -0.0069567,
  -0.0071549,
  -0.0071520,
  -0.0069422,
  -0.0066013,
  -0.0060751,
  -0.0055456,
  -0.0047894,
  -0.0042837,
  -0.0032862,
  -0.0031624,
  -0.0016075,
  -0.0026780,
  -0.0014203,
};
//*******************************************************


/*****************************************************************************

  Name: CtcssDetector

  Purpose: The purpose of this function is to serve as the constructor for
  an instance of a CtcssDetector.

  Calling Sequence: CtcssDetector(sampleRate)

  Inputs:

    sampleRate - The sample rate in units of samples/second.

 Outputs:

    None.

*****************************************************************************/
CtcssDetector::CtcssDetector(float sampleRate)
{
  int numberOfLowpassFilterTaps;

  // We do this because we're going to decimate by 2.
  this->sampleRate = sampleRate / 2;

  numberOfLowpassFilterTaps =
    sizeof(lowpassFilterCoefficients) / sizeof(float);

  // Instantiate the decimating lowpass filter.
  lowpassFilterPtr = new Decimator_int16(numberOfLowpassFilterTaps,
                                         lowpassFilterCoefficients,
                                         2);

  // This compensates for the "gain" that a DFT provides.
  dftScaleFactor = DFT_SCALE_FACTOR;

  // Set to  nominal values.
  detectorThreshold = DEFAULT_DETECTOR_THRESHOLD;

  // Reference the beginning of the buffer.
  bufferedDataIndex = 0;

  return;

} // CtcssDetector

/*****************************************************************************

  Name: ~CtcssDetector

  Purpose: The purpose of this function is to serve as the destructor for
  an instance of a CtcssDetector.

  Calling Sequence: ~CtcssDetector()

  Inputs:

    None.

  Outputs:

    None.

*****************************************************************************/
CtcssDetector::~CtcssDetector(void)
{

  // Release resources.
  delete lowpassFilterPtr;

  return;

} // ~CtcssDetector

/*****************************************************************************

  Name: reset

  Purpose: The purpose of this function is to reset the codeword detector.
  This involves resetting the buffer index, and resetting all filters and
  correlators.  

  Calling Sequence: reset

  Inputs:

    None.

  Outputs:

    None.

*****************************************************************************/
void CtcssDetector::reset(void)
{
  int i;

  // Reference the beginning of the buffer.
  bufferedDataIndex = 0;

  // Reset the decimator.
  lowpassFilterPtr->resetFilterState();

  return;

} // reset

/*****************************************************************************

  Name: setDetectorThreshold

  Purpose: The purpose of this function is to set the threshold of the
  tone detector.  

  Calling Sequence: setDetectorThreshold(threshold)

  Inputs:

    threshold - The detector threshold.

  Outputs:

    None.

*****************************************************************************/
void CtcssDetector::setDetectorThreshold(float threshold)
{

  this->detectorThreshold = threshold;

  return;

} // setDetectorThreshold

/*****************************************************************************

  Name: detectTone

  Purpose: The purpose of this function is to perform all of the necessary
  processing of an audio signal.  The goal is to bandlimit the
  signal to minimize noise and speech information, and identify the CTCSS
  tone that was used for the transmission.

  Calling Sequence: result = detectTone(pcmDataPtr,
                                        numberOfSamples,
                                        frequencyPtr,
                                        toneDetectedPtr)

  Inputs:

    pcmDataPtr - A pointer to data in the form of 16-bit, signed,
    little endian PCM samples..

    numberOfSamples - The number of samples contained in the input buffer.

    frequencyPtr - A pointer to storage for the returned frequency with a
    resolution of 0.1Hz.

    *toneDetectedPtr - A pointer to a flag that indicates that a tone
    was detected.  A value of true indicates that a tone was detected,
    and a value of false indicates that a tone was not detected.

  Outputs:

    None.

*****************************************************************************/
void CtcssDetector::detectTone(int16_t *pcmDataPtr,
                               uint32_t numberOfSamples,
                               int16_t *frequencyPtr,
                               bool *toneDetectedPtr)
{
  uint32_t numberOfDecimatedSamples;
  uint32_t length;

  // Default to false since processing is conditional.
  *toneDetectedPtr = false;

  if (bufferedDataIndex < REQUIRED_NUMBER_OF_SAMPLES)
  {
    // Concatenate the data to the buffer.
    memcpy(&bufferedData[bufferedDataIndex],
           pcmDataPtr,
           (numberOfSamples * 2));

    // Update the index to account for the new samples.
    bufferedDataIndex = bufferedDataIndex + numberOfSamples;
  } // if

  if (bufferedDataIndex >= REQUIRED_NUMBER_OF_SAMPLES)
  {
    // Apply the lowpass filter to the demodulated data.
    numberOfDecimatedSamples =
      removeHighFrequencyComponent(bufferedData,bufferedDataIndex);

    *frequencyPtr =
      determineToneFrequency(filteredData,numberOfDecimatedSamples);

    // Reference the beginning of the buffer.
    bufferedDataIndex = 0;

    if (*frequencyPtr != -1)
    {
      // Indicate that a CTCSS frequency was found.
      *toneDetectedPtr = true;
    } // if
  } // if

  return;

} // detectTone

/*****************************************************************************

  Name: removeHighFrequencyComponent

  Purpose: The purpose of this function is to remove the high frequency
  component from an audio signal.  The data is decimated to ease
  processing.

  Calling Sequence: sampleCount = 
                      removeHighFrequencyComponent(bufferPtr,bufferLength)

  Inputs:

    bufferPtr - A pointer to the demodulated signal.

    bufferLength - The number of samples contained in the input buffer.

  Outputs:

    sampleCount - The number of decimated samples stored.

    filteredData[] - An array for which filtered data is to be stored.

*****************************************************************************/
uint32_t CtcssDetector::removeHighFrequencyComponent(int16_t *bufferPtr,
                                                     uint32_t bufferLength)
{
  uint32_t i;
  uint32_t outputBufferIndex;
  int16_t sampleValue;
  bool sampleAvailable;

  // Default to no samples stored.
  outputBufferIndex = 0;

  for (i = 0; i < bufferLength; i++)
  {
    sampleAvailable = lowpassFilterPtr->decimate(bufferPtr[i],&sampleValue);

    if (sampleAvailable)
    {
      // Store decimated value.
      filteredData[outputBufferIndex] = sampleValue;

      // Reference the next storage location.
      outputBufferIndex++;
    } // if

  } // for

  return (outputBufferIndex);

} // removeHighFrequencyComponent

/*****************************************************************************

  Name: determineToneFrequency

  Purpose: The purpose of this function is to find the DFT bin that has
  the maximum power.  This ultimately determines the frequency of
  the CTCSS tone of interest.  The final decision of whether or not the
  frequency of interest is in the sample buffer is based upon the power
  of the frequency matching or exceeding the detector threshold.
  Additionally, the CTCSS frequency component must have the maximum
  relative power as compared to all other frequency components in the
  sample buffer.

  Calling Sequence: frequency = determineToneFrequency()

  Inputs:

    None.

  Outputs:

    frequency - The frequency of the CTCSS tone.  If a signal does not
    match or exceed the detector threshold, a value of -1 is returned.

*****************************************************************************/
int16_t CtcssDetector::determineToneFrequency(int16_t *bufferPtr,
                                              uint32_t bufferLength)
{
  uint32_t i;
  uint32_t index;
  int16_t frequency;

  // Default to something incorrect if nothing is found.
  frequency = -1;

  for (i = 0; i < NUMBER_OF_CTCSS_TONES; i++)
  {
    // Save the magnitude-squared values for later analysis.
    tonePowers[i] = goertzelSquared((float)ctcssFrequencies[i]/10,
                                     sampleRate,
                                     bufferPtr,
                                     bufferLength);
  } // for

  // Find the index of the peak value.
  index = findMaximumPowerIndex();

  if (tonePowers[index] >= detectorThreshold)
  {
    // Look up the frequency value.
    frequency = ctcssFrequencies[index];
  } // if 

  return (frequency);

} // determineToneFrequency

/*****************************************************************************

  Name: goertzelSquared

  Purpose: The purpose of this function is to determine the magnitude-squared
  value of a frequency bin by performing a modified version of the Goertzel
  algorithm. This algorithm implementation was taken from
  "Understanding Digital Signal Processing, Third Edition" by Richard Lyons.
  Specifically what was used was the simplified processing that can be
  carried out using only real quantities (versus complex quantities) when
  computing the magnitude-squared value of the frequency bin.

  Calling Sequence: y = goertzelSquared(toneFrequency,sampleRate,
                                        bufferPtr,bufferLength)

  Inputs:

    toneFrequency The frequency of the tone whose magnitude-squared value
    is to be computed.

    sampleRate - The sample rate in samples/second.

    bufferPtr - A pointer to PCM samples.

    bufferLength - The number of PCM samples referenced by bufferPtr.

  Outputs:

    y - The magnitude-squared value of the sinusoidal component at a
    frequency of toneFrequency.

*****************************************************************************/
float CtcssDetector::goertzelSquared(float toneFrequency,
                                     float sampleRate,
                                     int16_t *bufferPtr,
                                     uint32_t bufferLength)
{
  float y;
  float theta;
  float a1;
  float w0, w1, w2;
  uint32_t m, n;

  // Compute DFT index.
  m = (uint32_t)(0.5 + (toneFrequency / (sampleRate / bufferLength)));

  // Precompute the cosine argument.
  theta = (2 * M_PI * m) / bufferLength;

  // Precompute the coefficient.
  a1 = 2 * cos(theta);

  // Initialize pipeline.
  w1 = 0;
  w2 = 0;

  // Run through the recursive part of the filter.
  for (n = 0; n < bufferLength; n++)
  {
    // Compute filtered value using scalled PCM data.
    w0 = (a1 * w1) - w2 + (float)bufferPtr[n] * dftScaleFactor;

    // Update the pipeline.
    w2 = w1;
    w1 = w0; 
  } // for

  // Compute the magnitude squared.
  y = (w1 * w1) + (w2 * w2) - (a1 * w1 * w2);

  return (y);

} // goertzelSquared

/*****************************************************************************

  Name: findMaximumPowerIndex

  Purpose: The purpose of this function is to find the DFT bin that has
  the maximum power.  This ultimately determines the frequency of
  the CTCSS tone of interest.

  Calling Sequence: index = findMaximumPowerIndex()

  Inputs:

    None.

  Outputs:

    index - The index of the DFT bin which has the maximum power.

*****************************************************************************/
uint32_t CtcssDetector::findMaximumPowerIndex(void)
{
  uint32_t i;
  uint32_t index;
  float maximum;

  // Start here.
  maximum = 0;

  // Assume that the initial maximum is in DFT bin 0.
  index = 0;

  for (i = 0; i < NUMBER_OF_CTCSS_TONES; i++)
  {
    if (tonePowers[i] > maximum)
    {
      maximum = tonePowers[i];
      index = i;
    } // if
  } // for

  return (index);

} // findMaximumPowerIndex

/**************************************************************************

  Name: displayInternalInformation

  Purpose: The purpose of this function is to display internal information
  in the CTCSS detector.

  Calling Sequence: displayInternalInformation()

  Inputs:

    None.

  Outputs:

    None.

**************************************************************************/
void CtcssDetector::displayInternalInformation(void)
{

  fprintf(stderr,"\n--------------------------------------------\n");
  fprintf(stderr,"CTCSS Detector Internal Information\n");
  fprintf(stderr,"--------------------------------------------\n");

  fprintf(stderr,"Detector Sample Rate     : %f\n",(sampleRate * 2));
  fprintf(stderr,"Detector Threshold       : %f\n",detectorThreshold);

  return;

} // displayInternalInformation


