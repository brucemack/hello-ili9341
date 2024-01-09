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

Vertical Scrolling
------------------

I had some trouble fully understanding the vertical scrolling feature of the ILI9341.  Here is some text that 
may help.  There are a few important things to understand about this feature:
* Scrolling only works in the "vertical" direction (i.e. along the page dimension).  (But keep in mind that the 
MADCTL B5 bit may be able to achieve rotation of the physical display.)
* Scrolling only works in the "up" direction along the vertical axis.
* Scrolling is extremely smooth/fast. This is the most efficient and visually pleasing way
to scroll the screen.
* This part may be counter-intuitive: scrolling *doesn't move anything* in display RAM. You can think
of this feature as manipulating the "origin" used to map display RAM onto the physical screen for the
section of display RAM that lives inside of the Vertical Scrolling Area.
* The Vertical Scrolling Area will visually wrap.  Anything that disappears "off the top" of the 
scrolling area will re-appear on the bottom unless you take action to clear the "old top" row first.

All of this description is written from the perspective of a display that is configured with MADCTL B4=0.

The setting of the Vertical Scrolling Start Address indicates which page in display RAM appears *immediately
above* the Bottom Fixed Area that was defined using Vertical Scrolling Definition.
