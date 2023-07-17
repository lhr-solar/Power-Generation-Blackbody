# IMPROVEMENTS

- [IMPROVEMENTS](#improvements)
  - [Multiplexer](#multiplexer)
  - [eFUSE](#efuse)

---

## Multiplexer

The sampling rate of the RTDs is proposed to be on the order of 10 - 100 Hz
(depending if you want to SW filter it). This means that there is plenty of
bandwidth for the processor to perform serial sampling of each of the RTDs. The
proposed improvement is to replace the 8 MAX chips with a single MAX chip
connected to a multiplexer, which hooks up one of each of the lead pairs (e.g.
both positive wires in a 4-wire kelvin measurement) of the RTDs at a time.

This has a notable advantage of reducing board size, complexity, and cost,
potentially by up to 50%.

The disadvantage is that a multiplexer may introduce notable resistance to the
RTD measurement. Care must be taken both in designing and testing to determine
the added error of this change to see if it is feasible.

## eFUSE

For a sensing board it may make sense to add an eFuse to the board to protect it
against accidental user error that results in shorting of the board during
testing or usage.

The advantage of an eFuse is that it is small, cheap, and resettable; blowing
the fuse means that it can recover in a short period of time. It is added
protection against power being shorted by the user by touching the wrong pins
during use. 

The disadvantage of an eFuse is that as a PTC resistor, it is more inefficient
than a regular fuse. It consumes more power and may enable the board to re-trip
if there is a hardware failure.
