import os
import re
import subprocess

class CisCurrentBuild:
    def __init__(self):
        # set cis_base_dir

        cis_base_dir = os.getenv("cis_base_dir")

        assert cis_base_dir != None, (
            "Environment variable cis_base_dir is not defined")

        assert os.path.isdir(cis_base_dir), (
            "Cis base dir is not directory")

        self.cis_base_dir = cis_base_dir

        # set session owner

        session_opened_by_me = os.getenv("session_opened_by_me")

        assert cis_base_dir != None, (
            "Environment variable session_opened_by_me is not defined")

        assert session_opened_by_me in ['true', 'false'], (
            "session_opened_by_me is not bool")

        if session_opened_by_me == 'true':
            self.session_owner = True
        else:
            self.session_owner = False

        # set startjob exe

        startjob_exe = os.getenv("startjob")

        assert startjob_exe != None, (
            "Environment variable startjob is not defined")

        assert self.is_exe(startjob_exe), (
            "Startjob is not executable")

        self.startjob_exe = startjob_exe

        # set setvalue exe

        setvalue_exe = os.getenv("setvalue")

        assert setvalue_exe != None, (
            "Environment variable setvalue is not defined")

        assert self.is_exe(setvalue_exe), (
            "Setvalue is not executable")

        self.setvalue_exe = setvalue_exe

        # set getvalue exe

        getvalue_exe = os.getenv("getvalue")

        assert getvalue_exe != None, (
            "Environment variable getvalue is not defined")

        assert self.is_exe(getvalue_exe), (
            "Getvalue is not executable")

        self.getvalue_exe = getvalue_exe

        # set setparam exe

        setparam_exe = os.getenv("setparam")

        assert setparam_exe != None, (
            "Environment variable setparam is not defined")
        assert self.is_exe(setparam_exe), (
            "Setparam is not executable")

        self.setparam_exe = setparam_exe

        # set getparam exe

        getparam_exe = os.getenv("getparam")

        assert getparam_exe != None, (
            "Environment variable getparam is not defined")

        assert self.is_exe(getparam_exe), (
            "Getparam is not executable")

        self.getparam_exe = getparam_exe

        # set job_name

        job_name = os.getenv("job_name")

        assert job_name != None, (
            "Environment variable job_name is not defined")

        self.job_name = job_name

        # set build_number

        build_number = os.getenv("build_number")

        assert build_number != None, (
            "Environment variable build_number is not defined")

        assert re.match("^[0-9]{6}$", build_number), (
            "build_number should be string with six-digit integer")

        self.build_number = build_number

    def is_exe(self, path):
        path = os.path.join(self.cis_base_dir, "core", path)

        return os.path.isfile(path) and os.access(path, os.X_OK)

    def startjob(self, name):
        startjob_proc = subprocess.Popen(
            args = [
                os.path.join(self.cis_base_dir, "core", self.startjob_exe),
                name],
            stdout = subprocess.PIPE,
            universal_newlines = True)

        for stdout_line in iter(startjob_proc.stdout.readline, ""):
            print(stdout_line, end = '')

        startjob_proc.stdout.close()

        startjob_proc.wait()

        return startjob_proc.returncode

    def setvalue(self, name, value):
        setvalue_proc = subprocess.Popen(
            args = [
                os.path.join(self.cis_base_dir, "core", self.setvalue_exe),
                name,
                value])

        setvalue_proc.wait()

        return setvalue_proc.returncode

    def getvalue(self, name):
        getvalue_proc = subprocess.Popen(
            args = [
                os.path.join(self.cis_base_dir, "core", self.getvalue_exe),
                name],
            stdout = subprocess.PIPE)

        getvalue_result = getvalue_proc.stdout.read()

        getvalue_result = getvalue_result.decode("utf-8").partition('\n')[0]

        getvalue_proc.wait()

        return (getvalue_proc.returncode, getvalue_result)

    def setparam(self, name, value):
        setparam_proc = subprocess.Popen(
            args = [
                os.path.join(self.cis_base_dir, "core", self.setparam_exe),
                name,
                value])

        setparam_proc.wait()

        return setparam_proc.returncode

    def getparam(self, name):
        getparam_proc = subprocess.Popen(
            args = [
                os.path.join(self.cis_base_dir, "core", self.getparam_exe),
                name],
            stdout = subprocess.PIPE)

        getparam_result = getparam_proc.stdout.read()

        getparam_result = getparam_result.decode("utf-8").partition('\n')[0]

        getparam_proc.wait()

        return (getparam_proc.returncode, getparam_result)
