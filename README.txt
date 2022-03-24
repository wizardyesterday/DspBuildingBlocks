This is the start of a collection of digital signal processing building
blocks.  It is here for educational purposes, and it is hoped that it will
help people.

The initial items that I have populated in this repo are a numerically
controlled oscillator (NCO), a phase accumulator, and a phase corrector
system.  The NCO uses the services of the phase accumulator as does the
phase corrector.

An NCO can serve the purpose of an FM modulator since the control input
would be that of instantaneous PCM sample values.  Another use would be
for a frequency error correction system.  This particular NCO provides a
complex output (sine and cosine components), so it is suitable for SDR
use.

One can use google and see tutorials that describe how to make an NCO.
Unfortunately, lots of optimizations are made, and it can obscure the
clarity of an NCO.  For example, rather than working with a phase that is
represented directly in radians, it is an integer that is used to index into
a lookup table that describes the waveform of interest.  There is absolutely
nothing wrong with that technique; as a matter of fact, it *is* the way to
implement the NCO function in a realtime system.  On the other hand, I have
created this software for educational purposes.  It is hoped that one can
look at my code and say "yeah, this is dead simple".  We'll see how that
works out.

Now, why would I want to have a phase corrector when I have an NCO?  Well,
sometimes data is provided in polar format rather than Cartesian format.
When implementing frequency correction, you can counteract the phase rotation
(from sample to sample) by merely subtracting the current phase of a phase
accumulator.  The only function that the phase corrector is to provide the
subtraction of phase and ensure that the corrected phase value lies in the
branch bounded by -PI to PI.

I was on IRC a couple of days ago, and I heard people talking about
different ways to do CTCSS decoding. I mentioned that I made a CTCSS
detector, and since the other person had in interest in these items, I
decided to put my CTCSS detector into this repository. You feed it PCM data
(raw PCM samples), and it will print out the frequency of a PL tone.  It
also echos the PCM data that it read from stdin and echos it to stdout.
What use is this stdout thing? Well, if you got an SDR and it sends
demodulated data (PCM) to stdout, you can pipe the output the my test CTCSS
program (that instantiates my CtcssDetector class), followed by piping the
output of the test program to something like aplay.  In this way, you can
see the frequency of a CTCSS tone when listening to public safety comms
that use analog modulation and PL tones, as you listen to the comms.

There will be more things to come...
