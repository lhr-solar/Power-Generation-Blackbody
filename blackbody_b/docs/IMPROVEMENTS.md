# IMPROVEMENTS

- [IMPROVEMENTS](#improvements)
  - [Switch to L432KC chip](#switch-to-l432kc-chip)
  - [Optimize size and cost](#optimize-size-and-cost)

---

## Switch to L432KC chip

A little more complicated planning, but since the nucleo is 11.33$, switching to
the chip will save a couple dollars (7.91$ on Mouser and 5.94$ on JLCPCB!) and
will be an order of magnitude smaller in required area! 

The prereq for this is that a 5V and 3V3 regulator must be added to the system,
as well as a JTAG programmer and UART to USB FTDI chip.

## Optimize size and cost

The current size of the pcb is 60x90mm, or 5400 mm^2. It would be nice to cut
the size down to 50x50mm, with the relevant components in one corner of the board.

The cost of 5 PCBs:
- 7.00 for PCBs
- 67.63 for assembly (most of the cost is in ADM3055Es)
- 87.25 for mouser components (most of the cost is in Nucleos)

For a total of 161.88, or 32.376$ per PCB.

I think we can down the cost to 25$ per PCB (not including shipping and
handling).
