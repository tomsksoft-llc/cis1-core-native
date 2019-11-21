#!/usr/bin/env python
import sys
import os

sys.path.append(
    os.path.join(
        os.path.dirname(
            os.path.dirname(
                os.path.dirname(
                    os.path.abspath(__file__)))), "shared_code"))

from cis import CisCurrentBuild

def main(argv = None):
    print("Job without args started.")

    cis_current_build = CisCurrentBuild()

    print("Testing child job session owner...")

    assert cis_current_build.session_owner == False, (
        "session_opened_by_me should be false.")

    print("Test passed.")

if __name__ == "__main__":
    sys.exit(main())
