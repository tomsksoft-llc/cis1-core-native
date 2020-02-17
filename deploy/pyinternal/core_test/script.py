#!/usr/bin/env python
import sys
import os
import re

sys.path.append(
    os.path.join(
        os.path.dirname(
            os.path.dirname(
                os.path.dirname(
                    os.path.abspath(__file__)))), "shared_code"))

from cis import CisCurrentBuild

def root_job_owner_test(cis_current_build):
    print("Testing session owner...")

    assert cis_current_build.session_owner == True, (
        "Root job should own session")

    print("Test passed.")

def setvalue_getvalue_test(cis_current_build):
    print("Testing setvalue and getvalue...")

    getsetvalue_key = "test setvalue @"

    getsetvalue_val = "test set - ^ value";

    r = cis_current_build.setvalue(getsetvalue_key, getsetvalue_val)

    assert r == 0, ("Setvalue should return 0.")

    (r, value) = cis_current_build.getvalue(getsetvalue_key)

    assert r == 0, ("Getvalue should return 0.")

    assert value == getsetvalue_val, (
        "Value is {} (should be {})"
            .format(repr(value), repr(getsetvalue_val)))

    print("Test passed.")

def startjob_without_args_test(cis_current_build):
    print("Testing startjob without args...")

    (r, exit_code) = cis_current_build.startjob("pyinternal/core_test_job_without_args")

    assert r == 0, ("Startjob should return 0.")
    assert exit_code == 0, ("Job pyinternal/core_test_job_without_args failed.")

    (r, value) = cis_current_build.getvalue("last_job_name")

    assert r == 0, ("Getvalue should return 0.")
    assert value == "pyinternal/core_test_job_without_args", (
        "last_job_name is {} (should be {})"
            .format(repr(value), repr("pyinternal/core_test_job_without_args")))

    (r, value) = cis_current_build.getvalue("last_job_build_number")

    assert r  == 0, ("Getvalue should return 0.")
    assert re.match("^[0-9]{6}$", value), (
        "last_job_build_number is {} (should be string with six-digit integer)"
            .format(repr(value)))

    print("Test passed.")

def startjob_with_args_test(cis_current_build):
    print("Testing startjob with args...")

    prepared_prm2 = "set to the = non empty value"

    prepared_prm3 = "replace default param to FШ少:'@!#$%^&*()-_+=\\|/~{}[]`/?\"<>,"

    r = cis_current_build.setparam("prm2", prepared_prm2)

    assert r == 0, ("Setparam should return 0.")

    r = cis_current_build.setparam("prm3", prepared_prm3)

    assert r == 0, ("Setparam should return 0.")

    (r, exit_code) = cis_current_build.startjob("pyinternal/core_test_job_with_args")

    assert r == 0, ("Startjob should return 0.")
    assert exit_code == 0, ("Job pyinternal/core_test_job_with_args failed.")

    #assert params deletion

    (r, prm1) = cis_current_build.getvalue("prm1")

    assert r == 0, ("Getvalue should return 0.")

    (r, prm2) = cis_current_build.getvalue("prm2")

    assert r == 0, ("Getvalue should return 0.")

    (r, prm3) = cis_current_build.getvalue("prm3")

    assert r == 0, ("Getvalue should return 0.")

    assert (prm1, prm2, prm3) == ("\"why?\"", prepared_prm2, prepared_prm3), (
        "Job set incorrect values.")

    print("Test passed.")

def main(argv = None):
    print("Starting CIS Core test suite...")
    print("Root job started.")

    cis_current_build = CisCurrentBuild()

    root_job_owner_test(cis_current_build)

    setvalue_getvalue_test(cis_current_build)

    startjob_without_args_test(cis_current_build)

    startjob_with_args_test(cis_current_build)

if __name__ == "__main__":
    sys.exit(main())
