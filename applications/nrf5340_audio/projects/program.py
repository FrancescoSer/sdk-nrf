# Copyright (c) 2018 Nordic Semiconductor ASA
#
# All rights reserved.
#
# SPDX-License-Identifier: Nordic-5-Clause
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions are met:
#
# 1. Redistributions of source code must retain the above copyright notice,
#    this list of conditions and the following disclaimer.
#
# 2. Redistributions in binary form, except as embedded into a Nordic
#    Semiconductor ASA integrated circuit in a product or a software update for
#    such product, must reproduce the above copyright notice, this list of
#    conditions and the following disclaimer in the documentation and/or other
#    materials provided with the distribution.
#
# 3. Neither the name of Nordic Semiconductor ASA nor the names of its
#    contributors may be used to endorse or promote products derived from this
#    software without specific prior written permission.
#
# 4. This software, with or without modification, must only be used with a
#    Nordic Semiconductor ASA integrated circuit.
#
# 5. Any software provided in binary form under this license must not be
#    reverse engineered, decompiled, modified and/or disassembled.
#
# THIS SOFTWARE IS PROVIDED BY NORDIC SEMICONDUCTOR ASA "AS IS" AND ANY EXPRESS
# OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
# OF MERCHANTABILITY, NONINFRINGEMENT, AND FITNESS FOR A PARTICULAR PURPOSE ARE
# DISCLAIMED. IN NO EVENT SHALL NORDIC SEMICONDUCTOR ASA OR CONTRIBUTORS BE
# LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
# CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
# SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
# INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
# CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
# ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
# POSSIBILITY OF SUCH DAMAGE.

""" Tools to program multiple nRF5340 Audio DKs """

from threading import Thread
from os import system
from nrf5340_audio_dk_devices import SelectFlags
import subprocess

MEM_ADDR_UICR_SNR = 0x00FF80F0
MEM_ADDR_UICR_CH = 0x00FF80F4

UICR_CHANNEL_LEFT = 0
UICR_CHANNEL_RIGHT = 1


def __populate_UICR(dev):
    """ Program UICR in device with information from JSON file """
    if dev.nrf5340_audio_dk_device == "headset":
        if dev.channel == "left":
            cmd = "nrfjprog --memwr " + str(MEM_ADDR_UICR_CH) + " --val " + \
                   str(UICR_CHANNEL_LEFT) + " --snr " + \
                   str(dev.nrf5340_audio_dk_snr)
        elif dev.channel == "right":
            cmd = "nrfjprog --memwr " + str(MEM_ADDR_UICR_CH) + " --val " + \
                  str(UICR_CHANNEL_RIGHT) + " --snr " + \
                  str(dev.nrf5340_audio_dk_snr)
        else:
            print("Channel: " + dev.channel +
                  " does not equal 'left' or 'right'")
            return False

    if dev.nrf5340_audio_dk_device == "headset":
        # Write channel information to UICR
        ret_val = system(cmd)

        if ret_val:
            return False

    cmd = "nrfjprog --memwr " + str(MEM_ADDR_UICR_SNR) + " --val " + \
          str(dev.nrf5340_audio_dk_snr) + " --snr " + \
          str(dev.nrf5340_audio_dk_snr)

    # Write segger nr to UICR
    ret_val = system(cmd)
    if ret_val:
        return False
    else:
        return True


def __program_thread(dev, current_core):
    if dev.only_reboot == SelectFlags.TBD:
        cmd = "nrfjprog -r --snr " + str(dev.nrf5340_audio_dk_snr)
    elif current_core == "net":
        print("Programming " + current_core + " on dev snr: " +
              str(dev.nrf5340_audio_dk_snr))
        cmd = "nrfjprog --program " + dev.hex_path_net + \
              " -f NRF53  -q --snr " + str(dev.nrf5340_audio_dk_snr) + \
              " --sectorerase" + " --coprocessor CP_NETWORK"
    elif current_core == "app":
        print("Programming " + current_core + " on dev snr: " +
              str(dev.nrf5340_audio_dk_snr))
        cmd = "nrfjprog --program " + dev.hex_path_app + \
              " -f NRF53  -q --snr " + str(dev.nrf5340_audio_dk_snr) + \
              " --chiperase" + " --coprocessor CP_APPLICATION"
    else:
        raise Exception("Core definition error")

    ret_val = system(cmd)
    if ret_val and dev.only_reboot == SelectFlags.TBD:
        dev.only_reboot = SelectFlags.FAIL
        return
    elif ret_val and current_core == "net":
        dev.core_net_programmed = SelectFlags.FAIL
        return
    elif ret_val and current_core == "app":
        dev.core_app_programmed = SelectFlags.FAIL
        return

    # Populate UICR data matching the JSON file
    if current_core == "app":
        if not __populate_UICR(dev):
            dev.core_app_programmed = SelectFlags.FAIL
            return

    if dev.only_reboot == SelectFlags.TBD:
        dev.only_reboot = SelectFlags.DONE
    elif current_core == "net":
        dev.core_net_programmed = SelectFlags.DONE
    elif current_core == "app":
        dev.core_app_programmed = SelectFlags.DONE

    # Make sure boards are reset
    cmd = "nrfjprog -r --snr " + str(dev.nrf5340_audio_dk_snr)
    system(cmd)


def program_threads_run(devices_list, sequential=False):
    """ Program devices in parallel"""
    threads = list()
    # First program net cores if applicable
    for dev in devices_list:
        if not dev.nrf5340_audio_dk_snr_connected:
            continue

        if dev.only_reboot == SelectFlags.TBD:
            threads.append(Thread(target=__program_thread, args=(dev, None)))
            threads[-1].start()
            if sequential:
                threads[-1].join()
        elif dev.hex_path_net:
            threads.append(Thread(target=__program_thread, args=(dev, "net")))
            threads[-1].start()
            if sequential:
                threads[-1].join()

    for thread in threads:
        thread.join()

    threads.clear()

    for dev in devices_list:
        if not dev.nrf5340_audio_dk_snr_connected:
            continue

        if dev.only_reboot == SelectFlags.NOT and dev.hex_path_app:
            threads.append(Thread(target=__program_thread, args=(dev, "app")))
            threads[-1].start()
            if sequential:
                threads[-1].join()

    for thread in threads:
        thread.join()
