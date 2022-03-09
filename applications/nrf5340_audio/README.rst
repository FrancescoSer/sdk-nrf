.. _octave:

nRF5340 Audio
#############

.. contents::
   :local:
   :depth: 2

The nRF5340 Audio is a reference design that demonstrates Bluetooth Low Energy audio over isochronous channels (ISO) using LC3 or SBC codec compression and decompression.

.. _octave_overview:

Overview
********

The application can work either in the *connected isochronous stream* (CIS) mode or in the *broadcast isochronous stream* (BIS) mode, depending on the chosen firmware configuration.

.. _octave_overview_modes:

Application modes
=================

The software design of the nRF5340 Audio development kit supports the isochronous channels (ISO) feature in the following configurations:

* Broadcast Isochronous Streams (BIS):

  In this configuration, you can use the nRF5340 Audio development kit in the role of the gateway or as one of the headsets.
  Use multiple nRF5340 Audio development kits to test BIS having multiple receiving headsets.

  .. note::
     In BIS mode, you can use any number of nRF5340 Audio development kits as receivers.

* Connected Isochronous Streams (CIS):

  In this configuration, you can use the nRF5340 Audio development kit in the role of the gateway, the left headset, or the right headset.
  The gateway can send the audio data using both the left and the right ISO channels at the same time, allowing for stereophonic sound reproduction with synchronized playback.

.. _octave_overview_architecture:

Firmware architecture
=====================

The following figure illustrates the software layout for the nRF5340 Audio application:

TODO figure_update

.. figure:: doc/images/minim_arch.jpg
   :alt: nRF5340 Audio high-level design (overview)

   nRF5340 Audio high-level design (overview)


Module usage per firmware type
------------------------------

The following figures show an overview of the modules currently included in the firmware:

.. figure:: doc/images/overview_gateway_using_usb_v1.svg
   :alt: nRF5340 Audio modules on the gateway using USB

   nRF5340 Audio modules on the gateway using USB


.. figure:: doc/images/overview_headsets_and_gateway_using_i2s_v1.svg
   :alt: nRF5340 Audio modules on the gateway and the headsets using I2S

   nRF5340 Audio modules on the gateway and the headsets using I2S

The network core of the nRF5340 SoC runs the *LE Audio Controller Subsystem for nRF53*.
The application core runs Zephyr as the Bluetooth LE host, as well as other modules, like the following:

* From the |NCS|:

  * I2S
  * USB
  * SPI
  * TWI/I2C
  * UART (debug)
  * Timer

* From this application:

  * Stream Control
  * FIFO buffers
  * SBC encoder/decoder

* From other sources:

  * LC3 encoder/decoder (as a precompiled library, see `octave_requirements`_)

Since the application architecture is uniform and the firmware code is shared, the set of modules in use depends on the chosen stream mode (BIS or CIS),  the chosen audio inputs and outputs (USB or analog jack), and if the gateway or the headset configuration is selected.

.. note::
   In the current version of the application, no bootloader is used, and device firmware update (DFU) is not supported.

Synchronization module overview
-------------------------------

TODO

Synchronization module flow (headset)
+++++++++++++++++++++++++++++++++++++

The audio data in the headset devices follows the following path:

1. The LE Audio Controller Subsystem for nRF53 running on the network core receives the compressed audio data.
#. It communicates the audio data to the Zephyr Bluetooth LE host in a similar way to the :ref:`zephyr:bluetooth-hci-rpmsg-sample` sample.
#. The host sends the data to the stream control module (:file:`streamctrl.c`).
#. The data is sent to a FIFO buffer.
#. The data is sent from the FIFO buffer to the :file:`audio_datapath.c` module.
   The :file:`audio_datapath.c` module performs the audio synchronization based on the timestamps applied by the headset controllers to the packets sent from the gateway.
   This enables the creation of True Wireless Stereo (TWS) earbuds where the audio is synchronized in CIS mode.
   It does also keep the speed of the inter-IC sound (I2S) interface synchronized with the Bluetooth packets receiving speed.
#. The :file:`audio_datapath.c` module sends the compressed audio data to the LC3 or the SBC audio decoders for decoding.
   The LC3 audio codec is not open source.
   For more information, see `octave_requirements`_.
   The SBC audio codec is open source.
#. The audio decoder decodes the data and sends the uncompressed audio data (PCM) back to the :file:`audio_datapath.c` module.
#. The :file:`audio_datapath.c` module pipes the uncompressed audio data to the hardware codec.
#. The hardware codec receives the uncompressed audio data over the inter-IC sound (I2S) interface and performs the digital-to-analog (DAC) conversion to an analog audio signal.

.. _octave_requirements:

Requirements
************

The nRF5340 Audio application is designed to be used only with the following custom hardware:

+---------------------+----------+--------------------------+---------------------------------+
| Hardware platforms  | PCA      | Board name               | Build target                    |
+=====================+==========+==========================+=================================+
| nRF5340 Audio       | PCA10121 | nrf5340_audio_dk_nrf5340 | nrf5340_audio_dk_nrf5340_cpuapp |
+---------------------+----------+--------------------------+---------------------------------+

You need at least two nRF5340 Audio development kits (one with the gateway firmware and one with headset firmware) to test the application.
For CIS with TWS in mind, three kits are required.

.. note::
   The LC3 codec is not open source.

   To build the application using the LC3 codec, you also need access to the LC3 codec repository.
   To obtain access to the repository, please contact the sales department `here <Contact Us>`_.

.. _octave_dk:

nRF5340 Audio DK
================

The nRF5340 Audio development kit is a hardware development platform that demonstrates the nRF5340 Audio application.

.. _octave_dk_drawings:

Hardware drawings
-----------------

The nRF5340 DK hardware drawings show both sides of the development kit:

.. figure:: doc/images/nRF5340_audio_dk_front.svg
   :alt: Figure 1. nRF5340 Audio DK (PCA10121) front view

   Figure 1. nRF5340 Audio DK (PCA10121) front view

.. figure:: doc/images/nRF5340_audio_dk_back.svg
   :alt: Figure 2. nRF5340 Audio DK (PCA10121) back view

   Figure 2. nRF5340 Audio DK (PCA10121) back view

User interface
**************

The application implements a simple user interface.
You can control the application using predefined buttons while the LEDs display information.

Buttons
=======

The application uses the following buttons on the supported development kit:

+---------------+------------------------------------------------------+
| Button        | Function                                             |
+===============+======================================================+
| VOL-          | Volume down (and unmute)                             |
+---------------+------------------------------------------------------+
| VOL+          | Volume up (and unmute)                               |
+---------------+------------------------------------------------------+
| BTN3          | Play/pause                                           |
+---------------+------------------------------------------------------+
| BTN4          | Toggle between test tones and audio (gateway only)   |
+---------------+------------------------------------------------------+
| BTN5          | Mute volume                                          |
+---------------+------------------------------------------------------+

LEDs
====

To indicate the tasks performed, the application uses the LED behavior shown in the following table:

+-----------------------+-----------------------------------------------------------------------------------------------+
|LED                    |Indication                                                                                     |
+=======================+===============================================================================================+
|LED1                   |*Off:* No Bluetooth connection                                                                 |
|                       +-----------------------------------------------------------------------------------------------+
|                       |*Solid blue:* Headset: Kits have connected or found a broadcast Streaming                      |
|                       +-----------------------------------------------------------------------------------------------+
|                       |*Blinking blue:*                                                                               |
|                       |                                                                                               |
|                       |* Headset: Streaming audio                                                                     |
|                       |* Gateway BIS: Broadcasting audio                                                              |
|                       |* Gateway CIS: Streaming audio                                                                 |
+-----------------------+-----------------------------------------------------------------------------------------------+
|LED2                   |*Off:* Sync not achieved                                                                       |
|                       +-----------------------------------------------------------------------------------------------+
|                       |*Solid green:* Sync achieved                                                                   |
+-----------------------+-----------------------------------------------------------------------------------------------+
|LED3                   |*Blinking green:* The nRF5340 APP core is running.                                             |
+-----------------------+-----------------------------------------------------------------------------------------------+
|CODEC                  |*Off:* No config loaded to on-board HW codec.                                                  |
|                       +-----------------------------------------------------------------------------------------------+
|                       |*Solid green:* Configuration loaded                                                            |
+-----------------------+-----------------------------------------------------------------------------------------------+
|RGB1 (center opening)  |*Green:* Gateway                                                                               |
|                       +-----------------------------------------------------------------------------------------------+
|                       |*Blue:* Left Headset                                                                           |
|                       +-----------------------------------------------------------------------------------------------+
|                       |*Magenta:* Right Headset                                                                       |
|                       +-----------------------------------------------------------------------------------------------+
|                       |*Red:*                                                                                         |
|                       |APP core fault has occurred.                                                                   |
|                       |In debug mode, see UART output for details.                                                    |
+-----------------------+-----------------------------------------------------------------------------------------------+
|RGB2                   |Controlled by the Bluetooth LE controller on the net core.                                     |
|                       +-----------------------------------------------------------------------------------------------+
|                       |*Green:* Shows CPU activity                                                                    |
|                       +-----------------------------------------------------------------------------------------------+
|                       |*Red:* Error                                                                                   |
|                       +-----------------------------------------------------------------------------------------------+
|                       |*White (all colors on):* The RGB LED is not initialized by the BLE controller.                 |
+-----------------------+-----------------------------------------------------------------------------------------------+
|ERR                    |Indicates PMIC/charging error.                                                                 |
+-----------------------+-----------------------------------------------------------------------------------------------+
|CHG                    |*Solid yellow:* Charging                                                                       |
|                       +-----------------------------------------------------------------------------------------------+
|                       |*Off:*  Charge completed/no battery connected                                                  |
+-----------------------+-----------------------------------------------------------------------------------------------+
|OB/EXT                 |*Off:* No 3.3 V power                                                                          |
|                       +-----------------------------------------------------------------------------------------------+
|                       |*Green:* On-board codec selected                                                               |
|                       +-----------------------------------------------------------------------------------------------+
|                       |*Yellow:* External codec selected                                                              |
+-----------------------+-----------------------------------------------------------------------------------------------+
|FTDI SPI               |*Off:* Normal                                                                                  |
|                       +-----------------------------------------------------------------------------------------------+
|                       |*Yellow:* The FTDI has control over the SPI lines of the on-board hardware codec               |
+-----------------------+-----------------------------------------------------------------------------------------------+
|IFMCU (bottom side)    |*Off:* No PC connection                                                                        |
|                       +-----------------------------------------------------------------------------------------------+
|                       |*Solid green:* Connected to PC                                                                 |
|                       +-----------------------------------------------------------------------------------------------+
|                       |*Rapid green flash:* Failed USB enumeration                                                    |
+-----------------------+-----------------------------------------------------------------------------------------------+
|HUB (bottom side)      |*Off:* No PC connection                                                                        |
|                       +-----------------------------------------------------------------------------------------------+
|                       |*Green:* Standard USB hub operation                                                            |
+-----------------------+-----------------------------------------------------------------------------------------------+

.. _octave_requirements_build_types:

nRF5340 Audio build types
=========================

The nRF5340 Audio uses combinations of multiple :file:`.conf` files for different build types.

The :file:`prj.conf` file is the main configuration file, and it is always included.
The configuration files for specifying the build types are named using the format :file:`prj_<buildtype>.conf`.
For example, the ``release`` build type file name is :file:`prj_release.conf`.

The following build types are available for the nRF5340 Audio development kit:

* ``release`` - Release version of the application with no debugging features.
* ``debug`` - Debug version of the application.
  It has the same options settings from the ``release`` build type, also enabling debug options.
* ``headset``
* ``gateway``

You can combine them as follows:

+---------+-------+---------+
|         | Debug | Release |
+---------+-------+---------+
| Headset |       |         |
+---------+-------+---------+
| Gateway |       |         |
+---------+-------+---------+

See :ref:`octave_building` for detailed information about selecting the desired combination of build types for your build.

.. _octave_building:

Building and running
********************

This sample can be found under :file:`applications/nrf5340_audio` in the nRF Connect SDK folder structure.

You can build and program the application in two ways:

* Using the :file:`buildprog.py` script.
* Using the standard |NCS| build steps.

Out-of-the-box testing
======================

Each development kit comes preprogrammed with basic firmware that indicates if the kit is functional.
Before building the application, you can verify if it is functional following these steps:

1. Plug the devices into the USB port using USB-C.
#. Turn on the development kit using the On/Off switch.
#. Observe **RGB1** (center opening) turn solid yellow and **LED3** start blinking green.

.. _octave_building_conf_audio:

Configuring the audio codec
===========================

The nRF5340 audio application can use either the LC3 or the SBC codec.
The codec selected by default is LC3.
See `octave_requirements` for more information about the LC3 codec.


Configuring LC3
---------------

To use LC3, you must run the following commands from the command line:

1. Add the LC3 codec repository to the west manifest file of your project.
   You can do that as follows::

      west config manifest.group-filter +nrf5340_audio

#. Update west to let it fetch the LC3 private repository::

      west update

If west can fetch the repository correctly, you can now build the application.

For more information about west, see :ref:`here <zephyr:west>`.
For more information about accessing the LC3 codec repository, see `octave_requirements`_

Switching to SBC
----------------

The SBC codec is open source.
You can configure the application to use the SBC codec in two ways:

* Change the value of the `SBC_CODEC_DEFAULT` Kconfig option to edit the default setting.
  You can set its value either to `SW_CODEC_LC3` or `SW_CODEC_SBC`

* Change the values of the following two Kconfig options:

  * Set `CONFIG_SW_CODEC_LC3` to `n`.
  * Set `CONFIG_SW_CODEC_SBC` to `y`.

The method used for building the application restricts the methods you can use to set these options.
See `octave_building_script`_ and `octave_building_standard`_ for more information.

.. _octave_building_script:

Building and programming using buildprog.py
===========================================

The suggested method for building the application and programming it to the development kit is running the :file:`buildprog.py` python script in the :file:`applications/nrf5340_audio/tools/buildprog` folder.

Before using the script, you must update ``nrf5340_audio_dk_snr`` in the :file:`nrf5340_audio_dk_devices.json` file.
The ``nrf5340_audio_dk_snr`` is the SEGGER serial number on the nRF5340 Audio development kit.
Run ``nrfjprog -i`` in a command prompt to print the SEGGER serial numbers of all connected boards

You can assign a specific nRF5340 Audio development kit to be a headset or gateway
You can then use :file:`buildprog.py` to program the development kit according to the serial number set in `nrf5340_audio_dk_devices.json`.

You can also use the same JSON file to set the channel you wish each headset to be.
When no channel is set, the headset is programmed as a left channel one.

Run ``python buildprog.py -h`` for additional usage information.

To configure any other kconfig option when using the script, see :ref:`configure_application` for more information.

.. _octave_building_standard:

Building and programming using standard |NCS| steps
===================================================

You can also build the nRF5340 Audio application using the standard |NCS| :ref:`build steps<gs_programming>`.

When building the application using the standard |NCS| build steps, you must select the build type using the following two build flags in each build:

* ``CMAKE_BUILD_TYPE`` - it can be either ``DEBUG`` or ``RELEASE``.
* Either ``DEV_HEADSET`` or ``DEV_GATEWAY`` set as ``ON``.

The necessary .conf files are automatically included based on the values set for these flags.

For more information on build types, see :ref:`octave_requirements_build_types`.
For more information about how to configure applications in |NCS|, see :ref:`configure_application`.

TODO restructure_build_example

As an example, you can follow these steps to build and program the application from the command line:

1. Plug the devices into the USB port using USB-C.
#. Turn on the development kits using the On/Off switch.
#. Open a command prompt.
#. Run the following command to print the SEGGER serial numbers of the development kits::

     nrfjprog -i

#. Program the network core on the development kits by running the following command::

     nrfjprog --program <NET hex> --chiperase --coprocessor CP_NETWORK -r

   ``<NET hex>`` is the ``.hex`` binary file of the LE Audio Controller Subsystem for nRF53.
   It is located in the :file:`applications/nrf5340_audio/bin` folder.

#. Program the application core on the development kits with the respective hexadecimal files by running the following command::

     nrfjprog --program <APP hex> --coprocessor CP_APPLICATION --sectorerase -r

   .. note::
      Pay attention to which device is programmed with the gateway hexadecimal file and which with the headset one.

#. If any device is not programmed due to readback protection, run the following commands to recover the devices::

     nrfjprog --recover --coprocessor CP_NETWORK
     nrfjprog --recover

#. Follow steps 5 and 6 to program both cores again.
#. When using the CIS configuration, if you want to use two headset devices, you must also populate the UICR with the desired channel for each headset.
   Use the following command, depending on which headset you want to populate:

   * Left: ``nrfjprog --memwr 0x00FF80F4 --val 10``
   * Right: ``nrfjprog --memwr 0x00FF80F4 --val 20``

   Select the correct board when prompted with the popup, or add ``--snr`` (SEGGER serial number of the correct board) at the end of the ``nrfjprog`` command.

.. _octave_testing_steps:

Testing
=======

After building and programming the application, you can test it by performing the following steps:

1. Plug the devices into the USB port using USB-C.
#. Turn on the development kits using the On/Off switch.
#. Wait for the **LED1** on the gateway to start blinking blue.
   This indicates that the gateway device is ready to send data.
#. Search the list of audio devices listed in the sound settings of your operating system for *nRF5340 USB Audio* and select it as the output device.
#. Connect headphones to the **HEADPHONE** jack on the headset devices.
#. When **LED1** turns solid blue on the headsets, press the **BTN3** play/pause button on the headset.
   **LED1** blinks blue and the audio stream starts.
#. When you finish testing, power off the nRF5340 Audio development kits by switching the power switch from On to Off.

.. _octave_porting_guide:

Adapting application for end products
*************************************

This section describes how to adapt the nRF5340 Audio application to end products.
It describes the configuration sources used in the default configuration and lists the steps required for getting the firmware ready for end-product design.

Configuration sources
=====================

The nRF5340 Audio application uses the following files as configuration sources:

* Devicetree Specification (DTS) files - These reflect the hardware configuration.
  See :ref:`zephyr:dt-guide` for more information about the DTS data structure.
* Kconfig files - These reflect the software configuration.
  See :ref:`kconfig_tips_and_tricks` for information about how to configure them.

You must modify these configuration sources when `Adding a new board`_, as described below.

For information about differences between DTS and Kconfig, see :ref:`zephyr:dt_vs_kconfig`.
For detailed instructions for adding Zephyr support to a custom board, see Zephyr's :ref:`zephyr:board_porting_guide`.

.. _octave_board_configuration:

Board configuration
===================

To add support for a board in the application, provide a set of configuration files in the :file:`nrf/boards/arm/` folder.
You can use the :file:`nrf/boards/arm/nrf5340_audio_dk_nrf5340` folder as an example.

The application configuration files define both a set of options with which the nRF5340 Audio application is created for your board and the selected :ref:`octave_requirements_build_types`.
Include the following files in this directory:

Mandatory configuration files
    * The :file:`prj.conf` application configuration file
    * Either :file:`debug.conf` or :file:`release.conf`
    * Either :file:`gateway.conf` or :file:`headset.conf`
    * Configuration files for the selected modules

Optional configuration files
    * Memory layout configuration
    * DTS overlay file

See `Adding a new board`_ for information about how to add these files.

.. _octave_porting_guide_adding_board:

Adding a new board
==================

When adding a new board for the first time, focus on a single configuration.
Moreover, use the ``debug`` build type and do not add any additional build type parameters.

.. note::
    * The following procedure uses the CIS mode as an example.
    * The first three steps of the configuration procedure are identical to the steps described in Zephyr's :ref:`zephyr:board_porting_guide`.

To use the nRF5340 Audio application with your custom board:

1. Define the board files for your custom board by copying the nRF5340 Audio reference design files located in the :file:`nrf/boards/arm/` folder.
#. Edit the DTS files to make sure they match the hardware configuration.
   Pay attention to the following elements:

   * Pins that are used.
   * Changing interrupt priority.

#. Edit the reference design's Kconfig files to make sure they match the required system configuration.
   For example, disable the drivers that will not be used by your device.
#. Optionally, depending on the reference design, edit the DTS overlay file.
   This step is not required if you have created new board files and their DTS files fully describe your hardware.
   In this case, you don't need to have a DTS overlay file.
#. Build the application by selecting the name of the desired board (for example, ``new_audio_board_name``) in your build system.
   For example, when building from the command line, add ``-b new_audio_board_name`` to your build command.

.. _octave_bootloader:

Dependencies
************

This application uses the following `nrfx`_ libraries:

* :file:`nrfx_clock.h`
* :file:`nrfx_gpiote.h`
* :file:`nrfx_timer.h`
* :file:`nrfx_dppi.h`
* :file:`nrfx_i2s.h`
* :file:`nrfx_ipc.h`
* :file:`nrfx_nvmc.h`

The application also depends on the following Zephyr libraries:

* :ref:`zephyr:logging_api`
* :ref:`zephyr:kernel_api`
* :ref:`zephyr:api_peripherals`:

   * :ref:`zephyr:usb_api`

* :ref:`zephyr:bluetooth_api`:

  * :file:`include/bluetooth/bluetooth.h`
  * :file:`include/bluetooth/gatt.h`
  * :file:`include/bluetooth/hci.h`
  * :file:`include/bluetooth/uuid.h`

Application configuration options
*********************************

.. options-from-kconfig::
