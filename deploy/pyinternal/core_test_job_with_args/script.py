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
    print("Job with args started.")

    cis_current_build = CisCurrentBuild()

    print("Testing child job is not owner...")

    assert cis_current_build.session_owner == False, (
        "session_opened_by_me should be false.")

    print ("Test passed.")

    # Bring params back as values

    (r, prm1) = cis_current_build.getparam("prm1")

    assert r == 0, ("Getparam should return 0.")

    (r, prm2) = cis_current_build.getparam("prm2")

    assert r == 0, ("Getparam should return 0.")

    (r, prm3) = cis_current_build.getparam("prm3")

    assert r == 0, ("Getparam should return 0.")

    r = cis_current_build.setvalue("prm1", prm1)

    assert r == 0, ("Setvalue should return 0.")

    r = cis_current_build.setvalue("prm2", prm2)

    assert r == 0, ("Setvalue should return 0.")

    r = cis_current_build.setvalue("prm3", prm3)

    assert r == 0, ("Setvalue should return 0.")

if __name__ == "__main__":
    sys.exit(main())
