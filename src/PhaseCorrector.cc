//************************************************************************
// file name: PhaseCorrector.cc
//************************************************************************
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <math.h>

#include "PhaseCorrector.h"

using namespace std;

/*****************************************************************************

  Name: PhaseCorrector

  Purpose: The purpose of this function is to serve as the constructor for
  an instance of a PhaseCorrector.

  Calling Sequence: PhaseCorrector(sampleRate,frequency);

  Inputs:

    sampleRate - The sample rate in S/s.

    frequency - The frequency in Hz.

  Outputs:

    None.

*****************************************************************************/
PhaseCorrector::PhaseCorrector(float sampleRate,float frequency)
{

  // Save for frequency updates.
  this->sampleRate = sampleRate;

  // Save for display purposes.
  this->frequency = frequency;

  // Create an instance of a phase accumulator.
  phaseAccumulatorPtr = new PhaseAccumulator(sampleRate,frequency);

  // Set system to an initial state.
  reset();

  return;

} // PhaseCorrector

/*****************************************************************************

  Name: ~PhaseCorrector

  Purpose: The purpose of this function is to serve as the destructor for
  an instance of a PhaseCorrector.

  Calling Sequence: ~PhaseCorrector()

  Inputs:

    None.

  Outputs:

    None.

*****************************************************************************/
PhaseCorrector::~PhaseCorrector(void)
{

  // Release resources.
  if (phaseAccumulatorPtr != NULL)
  {
    delete phaseAccumulatorPtr;
  } // if

  return;

} // ~PhaseCorrector

/*****************************************************************************

  Name: setFrquency

  Purpose: The purpose of this function is to reset all runtime values to
  initial values.

  Calling Sequence: setFrequency(frequency)

  Inputs:

    frequency - The operating frequency in Hz..

  Outputs:

    None.

*****************************************************************************/
void PhaseCorrector::setFrequency(float frequency)
{
  uint32_t i;

  // Save for later use.
  this->frequency = frequency;

  // Update the phase accumulator.
  phaseAccumulatorPtr->setFrequency(frequency);

  return;

} // setFrequency

/*****************************************************************************

  Name: reset

  Purpose: The purpose of this function is to reset all runtime values to
  initial values.

  Calling Sequence: reset()

  Inputs:

    None.

  Outputs:

    None.

*****************************************************************************/
void PhaseCorrector::reset(void)
{
  uint32_t i;

  // Reset the phase accumulator to the starting point.
  phaseAccumulatorPtr->reset();

  return;

} // reset

/*****************************************************************************

  Name: run

  Purpose: The purpose of this function is to perform a phase correction
  based upon the current value of the phase accumulator.

  Calling Sequence: correctedPhase = run(uncorrectedPhase)

  Inputs:

    uncorrectedPhase - The uncorrected phase in the range,
    -PI < uncorrectedPhase < PI.

  Outputs:

    correctedPhase - The corrected phase in the range,
    -PI < uncorrectedPhase < PI.

*****************************************************************************/
float PhaseCorrector::run(float uncorrectedPhase)
{
  float phase;
  float correctedPhase;

  // Run and get the next phase value.
  phase = phaseAccumulatorPtr->run();

  // Computed corrected phase.
  correctedPhase = uncorrectedPhase - phase;

  //_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
  // Ensure that -PI < correctedPhase < PI.  This
  // is necessary to prevent overflow in the signed
  // fractional representation of the phase.
  //_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
  while (correctedPhase > M_PI)
  {
    correctedPhase -= (2 * M_PI);
  } // while

  while (correctedPhase < (-M_PI))
  {
    correctedPhase += (2 * M_PI);
  } // while
  //_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/

  return (correctedPhase);

} // run

