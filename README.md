Overview
========
A very basic demonstration of a TFT display driver over SPI.  Includes extensive commentary.

The most recent datasheet I can find (V1.11) is on the Adafruit website: https://cdn-shop.adafruit.com/datasheets/ILI9341.pdf

Some Key Concepts
=================

Gamma Correction
----------------

(From Wikipedia)

Gamma correction or gamma is a nonlinear operation used to encode and decode luminance or tristimulus values in video or still image systems. Gamma correction is, in the simplest cases, defined by the following power-law expression:

$Vout = A * Vin^{\gamma}$

where the non-negative real input value $Vin$ is raised to the power $\gamma$ and multiplied by the constant $A$ to get the output value $Vout$. In the common case of $A = 1$, inputs and outputs are typically in the range 0–1.
