#
# Copyright (c) 2021 Nordic Semiconductor ASA
#
# SPDX-License-Identifier: LicenseRef-Nordic-5-Clause
#

"""
Script to do clang-format on all files in the minim5340 folder
"""

import subprocess
from subprocess import PIPE, Popen
import glob
import os
import sys

def clang_format_all_files_run():
    """ Pre-commit check running"""
    print("=== Running checks on all files ===")
    NCS_BASE = os.path.dirname(os.path.dirname(os.path.dirname(os.getcwd())))
    print(NCS_BASE)

    folders = [NCS_BASE+'/applications/nrf5340_audio/nrf5340_audio_app/src/**/', NCS_BASE+'/tests/nrf5340_audio/**/']
    filetypes = ('*.c', '*.h')
    files = []
    files_failed = []

    for folder in folders:
        for filetype in filetypes:
            files.extend(glob.glob(folder+filetype, recursive=True))

    if not files:
        print("No files found")
        sys.exit(-1)

    os.chdir(NCS_BASE)

    for file in files:
        print(file)
        file_passed = True
        cmd = " ls --color=always; clang-format --Werror --dry-run " + file

        try:
            po = subprocess.Popen(cmd, shell=True, stderr=subprocess.PIPE, stdout=subprocess.PIPE)
            err, out = po.communicate()
        except Exception as exep:
            print(exep)
            sys.exit(-1)
        else:
            if po.returncode != 0:
                print("FAIL Clang")
                print(out.decode('utf-8'))
                file_passed = False


        cmd = "git diff /dev/null " + file + " | ${ZEPHYR_BASE}/scripts/checkpatch.pl -"
        try:
            po = subprocess.Popen(cmd, shell=True, stderr=subprocess.PIPE)
            po.communicate()
        except Exception as exep:
            print(exep)
            sys.exit(-1)
        else:
            if po.returncode != 0:
                print("FAIL Checkpatch")
                file_passed = False

        if not file_passed:
            files_failed.append(file)

        print("======================")

    if files_failed:
        print("Check.py Fail. Number of files failed: " + str(len(files_failed)) + " of " + str(len(files)))
        for file_f in files_failed:
            print("FAILED: " + file_f)
    else:
        print("===Check.py passed!=== " + str(len(files)) + " files checked")

if __name__ == '__main__':
    clang_format_all_files_run()
