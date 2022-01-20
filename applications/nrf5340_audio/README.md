# The nRF5340 Audio repo
This readme is intended for nRF5340 Audio developers.
The repo contains resources for building the nRF5340 Audio applications (APP core), the NET core is pre-compiled.

Some non-disclosable code is located in a separate repo (lc3).
Developers outside Nordic Semiconductor will be warned when running `west update`
that this repo is unavailable, this can be ignored as the repo is manually added with the release

# Required tools
- All major hosts are supported, although OSX is not tested.
- `python3` and `pip` to be in in path and pointing to python 3.6.x.
- nrfjprog `https://www.nordicsemi.com/Products/Development-tools/nrf-command-line-tools`

## Working with the repo
### Setting up
- Please install `west` by executing `pip install west` in command line interface.
- Execute `mkdir <base_folder> && cd <base_folder>`. Please make path and name of the `<base_folder>` as short as possible, also the path and the name of `<base_folder>` should not contain spaces or special characters.\
E.g., the path of the `<base_folder>` should be like this `C:\nrf` for Windows, or `~/nrf` for Linux
- Execute `west init -m <nrf5340_audio repo address>` under the `<base_folder>`.\
  Alternatively, (e.g if received as a .zip),: extract received zip to `<base_folder>`\
  E.g., After the package is extracted or downloaded the repo through west, the workspace should be `C:\nrf\minim5340`
- Run `west init -l minim5340` in the `<base_folder>`
- Navigate to `minim5340` folder, and execute `pip install -r verification/requirements.txt` for installing the necessary python modules
- Execute `west update` under minim5340. West will fetch all the required components, including Zephyr. This is a parallel to git submodules. Run this whenever a pull is made to ensure that all modules are at the correct version.
- Set up a toolchain: https://docs.zephyrproject.org/latest/getting_started/index.html#set-up-a-toolchain
- Zephyr supports both the Zephyr SDK compiler as well as GNU ARM embedded. Although both should work, using any other compiler than GNU ARM embedded toolchain will prompt an error from the build system. The GNU toolchain can be downloaded from:
  https://developer.arm.com/tools-and-software/open-source-software/developer-tools/gnu-toolchain/gnu-rm/downloads \
  Please note that this release is only tested with `GNU Arm Embedded Toolchain 9-2019-q4-major (9.2.1)`
  All tests and development have been made under this toolchain. Using a different toolchain can have undesired effects.
  You must then set the following environment variables:
  `ZEPHYR_TOOLCHAIN_VARIANT=gnuarmemb`
  `GNUARMEMB_TOOLCHAIN_PATH=<path to compiler>` e.g.: `~/bin/gcc-arm-none-eabi-9-2019-q4-major`
- `pip install -r ../zephyr/scripts/requirements.txt`
- If not already installed, clang-format version 10.0.0 must be installed and added to PATH. Pre-built binaries can be downloaded from:
https://releases.llvm.org/download.html
- The repo should now be set up correctly.

# How to build
## Automatic build and flash
In `minim5340/projects`, there's a python script `buildprog.py` which can build app core, and flash both cores.
Run `python buildprog.py -h` for additional usage information.
Please also update the `nrf5340_audio_dk_snr` in `minim5340/projects/nrf5340_audio_dk_devices.json`. The `nrf5340_audio_dk_snr` is the Segger serial number on the nRF5340 Audio DK. You can assign a specific nRF5340 Audio DK to be a headset or a gateway, then `buildprog.py` will program the nRF5340 Audio DK according to the settings in `nrf5340_audio_dk_devices.json`. You can also set the channel you wish each headset to be, left is default if no channel is set.

The project can also be built directly using 'west build -b nrf5340_audio_dk_nrf5340_cpuapp' but then it is important to remember to add compile flags for debug/release as well as flags for what device you want to build for; headset or gateway.
Examples of flags:
  -DDEV_HEADSET=ON
  -DDEV_GATEWAY=ON
  -DCMAKE_BUILD_TYPE=DEBUG
  -DCMAKE_BUILD_TYPE=RELEASE

Note that the net core is precompiled and is found in `minim5340/projects/nrf5340_audio_net`

# Running on target
## Running on nRF5340 Audio DK
This project will only run on the nRF5340 Audio DK (PCA10121).

# Projects
All projects reside with `projects\`.

# Contribution
For contributing code back to the repo, it is necessary to setup tools for checking indentation and also the format of commit messages.

- Modify the first line of `<base_folder>/minim5340/tools/commit-msg-hook.py` and `pre-commit-hook.py` depending on your OS.
  - For Windows, the first line can be `#!C:/Python37/python.exe`
  - For Linux, the first line can be `#!/usr/bin/env python3`

- Navigate to `<base_folder>/minim5340` and execute `verification/scripts/prepare.sh` or `verification/scripts/prepare.bat`
- After finishing these steps, every time when committing through git, `clang-format` and `checkpatch` will check the code. The commit message will also be checked automatically.
