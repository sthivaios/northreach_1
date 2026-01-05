# NorthReach 1

## Introduction

NorthReach 1 is a wired USB remote controller, to aid during presentations, particularly
on touchscreen interactive whiteboards.

Apart from basic features that other commercially available presenter remotes have, NorthReach
includes buttons for controlling Windows computers by default, such as a separate button for closing
the focused window, or a button to bring up Task Manager in the event that some application freezes.

The firmware is written in C, with the Espressif ESP-IDF framework. Through the `menuconfig` utility,
the board can be reconfigured to assign different actions to any button, for example to make the remote
compatible with MacOS.

Additionally, under the `hardware/` directory, the KiCad project for the PCB is available, as well
as the 3D printed enclosure inside `enclosure/`.

**The firmware and the PCB is only designed for ESP-S3 modules which natively support USB.**

In the future, there will also be a wireless version of NorthReach, possibly based on a Nordic nRF SoC.
However, for now, I'm focusing on the wired version.

## Contributing

You are more than welcome to contribute to the project, by forking then creating a pull request.
If you are an end user, feel free to request new features or report a bug by creating an issue
on this repository on GitHub.

If you are actually contributing code to the project, make sure that you understand the license terms.

## Licensing
This project contains multiple components under different licenses.
See [LICENSE](LICENSE) for more details.