#!/usr/bin/env python
import sys
import os
import argparse
import subprocess
from shutil import copy
from shutil import copytree

def is_dir(path):
    return os.path.isdir(path)

def is_exe(path):
    return os.path.isfile(path) and os.access(path, os.X_OK)

def load_cis_config(path):
    if not os.path.isfile(path):
        return None

    cis_conf_file = open(path)

    cis_config = {}

    for line in cis_conf_file:
        name, var = line.partition("=")[::2]
        cis_config[name.strip()] = var.strip()

    return cis_config

def write_cis_config(path, config):
    cis_conf_file = open(path, 'w')

    for (executable, executable_name) in config.items():
        cis_conf_file.write(executable + "=" + executable_name + '\n')

def main(argv = None):
    parser = argparse.ArgumentParser(
        description = 'Deploy cis core to given directory.')

    parser.add_argument(
        '--execs_dir',
        type = str,
        required = True,
        help = 'directory to build executable_names.')

    parser.add_argument(
        '--deploy_dir',
        type = str,
        required = True,
        help = 'directory where cis_base_dir will be created.')

    args = parser.parse_args()

    execs_dir = os.path.abspath(args.execs_dir)

    deploy_dir = os.path.abspath(args.deploy_dir)

    if not is_dir(execs_dir):
        print("execs_dir should be directory.")

        return 1

    if not is_dir(os.path.dirname(deploy_dir)):
        print("deploy_dir parent directory should exists.")

        return 1

    if os.path.exists(deploy_dir):
        print("deploy_dir shouldn't exists.")

        return 1

    current_dir = os.path.dirname(
        os.path.abspath(__file__))

    cis_config = load_cis_config(
        os.path.join(current_dir, "cis.conf.example"))

    if cis_config == None or not is_dir(os.path.join(current_dir, "pyinternal")):
        print("Internal error.")

        return 1

    if os.name == 'nt':
        for (executable, executable_name) in cis_config.items():
            cis_config[executable] = executable_name + '.exe'

    for (executable, executable_name) in cis_config.items():
        if not is_exe(os.path.join(execs_dir, executable_name)):
            print("Incorrect executable name.")

            return 1

    os.mkdir(deploy_dir)

    os.mkdir(os.path.join(deploy_dir, "core"))
    os.mkdir(os.path.join(deploy_dir, "docs"))
    os.mkdir(os.path.join(deploy_dir, "jobs"))
    os.mkdir(os.path.join(deploy_dir, "logs"))
    os.mkdir(os.path.join(deploy_dir, "sessions"))

    for (executable, executable_name) in cis_config.items():
        copy(
            os.path.join(execs_dir, executable_name),
            os.path.join(deploy_dir, "core", executable_name))

    write_cis_config(
        os.path.join(deploy_dir, "core", "cis.conf"),
        cis_config)

    copytree(
        os.path.join(current_dir, "pyinternal"),
        os.path.join(deploy_dir, "jobs", "pyinternal"))

    penv = os.environ.copy()

    penv["cis_base_dir"] = deploy_dir

    startjob_proc = subprocess.Popen(
        args = [
            os.path.join(deploy_dir, "core", cis_config["startjob"]),
            "pyinternal/core_test"],
        stdout = subprocess.PIPE,
        universal_newlines = True,
        env = penv)

    for stdout_line in iter(startjob_proc.stdout.readline, ""):
        print(stdout_line, end = '')

    startjob_proc.stdout.close()

    startjob_proc.wait()

if __name__ == "__main__":
    sys.exit(main())
