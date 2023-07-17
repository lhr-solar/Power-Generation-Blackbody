# :sunny: Blackbody C - Topshell Irradiance Sensor

- [:sunny: Blackbody C - Topshell Irradiance Sensor](#sunny-blackbody-c---topshell-irradiance-sensor)
  - [Repository Structure](#repository-structure)
  - [Maintainers](#maintainers)
  - [Versioning](#versioning)
  - [TODO](#todo)
    - [Documentation](#documentation)
    - [HW](#hw)
    - [SW](#sw)

---

## Repository Structure

- **datasheets** - contains datasheets for major components of the PCB.
- **docs** - contains documentation on how to build, test, and use the PCB.
- **fw** - contains fw that is loaded onto the PCB.
  - **tests** - test programs used to characterize and validate the PCB.
  - **src** - main program used to run the PCB.
  - **inc** - internally developed libraries specific for the PCB.
  - **lib** - third party libraries used by the PCB.
- **hw**
  - **footprints** - project specfic footprint association files for the PCB.
  - **models** - 3d .step files for the PCB 3d viewer.
  - **symbols** - project specific symbol files for the PCB.
  - **vX.Y.Z** - frozen versioning folder for PCB production files.
  - design files

---

## Maintainers

The current maintainer of this project is Matthew Yu as of 07/16/2023. His email
is [matthewjkyu@gmail.com](matthewjkyu@gmail.com).

Special thanks to Professor Gary Hallock, who supervised the development and
design of this project.

---

## Versioning

This PCB is on unified version `0.1.0`. Every time the schematic and/or layout
is updated, this version number should go up. We use [semantic
versioning](https://semver.org/) to denote between versions. See the
[changelog](./docs/CHANGELOG.md) for more details.

---

## TODO

### Documentation

- Update the following documents:
  - [Ordering and assembly](./docs/ORDERING_AND_ASSEMBLY.md)
  - [Testing](./docs/TESTING.md)
- Add datasheets for key components of the device.

### HW

- See [changelog 0.2.0rc](./docs/CHANGELOG.md#020rc-proposed) proposed changes to improve board cost, size.

### SW

- Regenerate code for MBED with proposed file structure.