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

where the non-negative real input value $Vin$ is raised to the power $\gamma$ (that's gamma) and multiplied by the constant $A$ to get the output value $Vout$. In the common case of $A = 1$, inputs and outputs are typically in the range 0–1.

So the idea here is to take the encoding of $Vin$ values and "stetch" the output in the higher range. More luminance resolution is used on the lower end than on the higher end of the scale, which apparently is consistent with human vision.

There is only one Gamma curve built into the ILI9341 and it uses $A = 1$ and $\gamma = 2.2$.