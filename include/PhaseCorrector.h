//**************************************************************************
// file name: PhaseCorrector.h
//**************************************************************************
//_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
// This class implements a signal processing block that performs a
// phase correction function.
//_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/

#ifndef __PHASECORRECTOR__
#define __PHASECORRECTOR__

#include <stdint.h>
#include "PhaseAccumulator.h"

class PhaseCorrector
{
  //***************************** operations **************************

  public:

  PhaseCorrector(float sampleRate,float frequency);

  ~PhaseCorrector(void);

  void setFrequency(float frequency);
  void reset(void);
  int16_t run(int16_t uncorrectedPhase);
  float run(float uncorrectedPhase);

  //***************************** attributes **************************
  private:

  // The sample rate is needed when performing frequency changes.
  float sampleRate;

  // The operating frequency of the PHASECORRECTOR.
  float frequency;

  PhaseAccumulator *phaseAccumulatorPtr;
};

#endif // __PHASECORRECTOR__
