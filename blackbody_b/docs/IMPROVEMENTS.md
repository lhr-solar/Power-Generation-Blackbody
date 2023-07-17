# IMPROVEMENTS

- [IMPROVEMENTS](#improvements)
  - [Connector Swap](#connector-swap)
  - [Copper Ring](#copper-ring)

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