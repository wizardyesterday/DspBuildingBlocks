//**************************************************************************
// file name: CtcssDetector.h
//**************************************************************************
//_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
// This class implements a CTCSS detector.  This class has a configurable
// gain to appropriately scale PCM data and a configurable threshold.
///_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/

#ifndef __CTCSSDETECTOR__
#define __CTCSSDETECTOR__

#include <stdint.h>
#include "Decimator_int16.h"

#define NUMBER_OF_CTCSS_TONES (41)

class CtcssDetector
{
  //***************************** operations **************************

  public:

  CtcssDetector(float sampleRate);
  ~CtcssDetector(void);

  void reset(void);
  void setDetectorThreshold(float threshold);

  void detectTone(int16_t *pcmDataPtr,
                  uint32_t numberOfSamples,
                  int16_t *frequencyPtr,
                  bool *toneDetectedPtr);

  void displayInternalInformation(void);

  private:

  //*******************************************************************
  // Utility functions.
  //*******************************************************************
  uint32_t removeHighFrequencyComponent(int16_t *bufferPtr,
                                        uint32_t bufferLength);

  int16_t determineToneFrequency(int16_t *bufferPtr,
                                 uint32_t bufferLength);

  float goertzelSquared(float toneFrequency,
                        float sampleRate,
                        int16_t *bufferPtr,
                        uint32_t bufferLength);

  uint32_t findMaximumPowerIndex(void);

 //*******************************************************************
  // Attributes.
  //*******************************************************************
  // This is the sample rate in samples/second.
  float sampleRate;

  // This scales the data to a value appropriate for processing.
  float dftScaleFactor;

  // This threshold is used to determine the presense of a signal.
  float detectorThreshold;

  // This array represents the power values at the DFT bins.
  float tonePowers[NUMBER_OF_CTCSS_TONES];

  // This filter is used to remove speech spectra.
  Decimator_int16 *lowpassFilterPtr;

  int16_t filteredData[16000];

  // Buffer management support.
  uint32_t bufferedDataIndex;
  int16_t bufferedData[32000];

};

#endif // __CTCSSDETECTOR__
