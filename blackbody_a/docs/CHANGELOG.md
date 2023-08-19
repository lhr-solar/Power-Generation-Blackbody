# CHANGELOG

- [CHANGELOG](#changelog)
  - [0.1.0](#010)
  - [0.2.0](#020)
  - [0.3.0rc (proposed)](#030rc-proposed)

---

## 0.1.0

Initial version.

## 0.2.0

**Schematic**

- Errata fix for no power connections to Blackbody C.
- Switched to M3 mounting holes.
- Standardized schematic into hierarchical sheets.
- Updated component listings with a focus on JLCPCB assembly and cost reduction.
- Swapped out isolated 12V-9V regulator for nonisolated 12V-5V regulator.

**Layout**

- Major layout changes oriented towards a single sided PCB assembly (as most parts now are assembled by JLCPCB for cheaper).
- Improvement to interface connector placement and user interface (buttons, LEDs).

## 0.3.0rc (proposed)

- Multiplexer to simplify 8 MAX chips to 1 MAX chip and 8 outlets. Consider resistance increase and error.
  - Look at 2 multichannel FET [chips](mouser.com/ProductDetail/Diodes-Incorporated/DMHT3006LFJ-13?qs=PqoDHHvF64%2FIw5fnwz%252BNTw%3D%3D) each for positive line (4 chips)
  - Set requirement on resistance [error](https://www.tnp-instruments.com/sitebuildercontent/sitebuilderfiles/pt100_385c_table.pdf)