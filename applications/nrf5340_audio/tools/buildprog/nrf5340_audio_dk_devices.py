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

"""
Settings for building and flashing nRF5340 Audio DK for different targets.
"""
from dataclasses import dataclass


@dataclass
class SelectFlags:
    """Holds the available status flags"""
    NOT: "str" = "Not selected"
    TBD: "str" = "Selected TBD"
    DONE: "str" = "Selected done"
    FAIL: "str" = "Selected ERR"


@dataclass
class DeviceConf:
    """This config is populated according to connected segger serial numbers
    (snr) and command line arguments"""
    nrf5340_audio_dk_snr: int = 0
    nrf5340_audio_dk_snr_connected: bool = False
    nrf5340_audio_dk_device: str = ""
    channel: str = ""
    only_reboot: str = SelectFlags.NOT
    hex_path_app: str = ""
    core_app_programmed: str = SelectFlags.NOT
    hex_path_net: str = ""
    core_net_programmed: str = SelectFlags.NOT

    def __init__(self, nrf5340_audio_dk_snr, nrf5340_audio_dk_device, channel):
        self.nrf5340_audio_dk_snr = nrf5340_audio_dk_snr
        self.nrf5340_audio_dk_device = nrf5340_audio_dk_device
        self.channel = channel


@dataclass
class BuildConf:
    """Build config"""
    core: str = ""
    device: str = ""
    build: str = ""
    pristine: bool = False

    def __init__(self, core, device, pristine, build):
        self.core = core
        self.device = device
        self.pristine = pristine
        self.build = build
