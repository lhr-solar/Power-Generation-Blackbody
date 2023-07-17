# IMPROVEMENTS

- [IMPROVEMENTS](#improvements)
  - [Connector Swap](#connector-swap)
  - [Copper Ring](#copper-ring)
  - [Switch to L432KC chip](#switch-to-l432kc-chip)

---

## Connector Swap

We'd like a connector swap from the existing smd testpoints to a physical,
low-profile, preferably circular connector. This may be a JST connector. The
idea is to have an easily disconnectable board that can be left in the topshell.

## Copper Ring

A copper ring surrounding the main PCB but left floating may be useful for both
structural and EMI purposes. It can protect the PCB from wearing out, and can
reduce noise from other sources (perhaps the solar cells).

Note that we should consider having the ring in internal layers (making the PCB
a 4 layer board) to prevent contact with other live components which may short
across the ring.

## Switch to L432KC chip

A little more complicated planning, but since the nucleo is 11.33$, switching to
the chip will save a couple dollars (7.91$ on Mouser and 5.94$ on JLCPCB!) and
will be an order of magnitude smaller in required area! 