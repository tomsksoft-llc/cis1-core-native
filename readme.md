# Cis1 core native

> Continuous integration system core written in C++

[![License](http://img.shields.io/:license-mit-blue.svg?style=flat-square)](http://badges.mit-license.org)

This repository contains a core of continuous integration system cis1 written in C++.
This component follows the rules described in the [cis1-docs](https://github.com/tomsksoft-llc/cis1-docs/ "cis1 documentation") repository.
It's compatible with OS Linux; the support for OS Windows is not guaranteed, but we are working to add it.

## Description

Full system description, terms and entity definitions can be found on [this](https://github.com/tomsksoft-llc/cis1-docs/blob/master/overview.md "cis1 overview") document page.

## Usage

Set `cis_base_dir` environment variable to the cis1 installation directory path.

Create job. (see [cis1-docs](https://github.com/tomsksoft-llc/cis1-docs/ "cis1 documentation"))

Run the job with the following command

```console
$ ${cis_base_dir}/core/$startjob internal/core_test
```

After the command is successfully executed, the exitcode of the job and some other parameters will be displayed in the console.

## Usage with webui

Install [webui](https://github.com/tomsksoft-llc/cis1-webui-native-srv-cpp).
After successful installation, you can manage the cis core via web interface.

## Compiling

### Linux

Build requirements:
> Conan, gcc 8.3, CMake 3.9+

Add the `tomsksoft` and `bincrafters` requirements to conan

```console
$ conan remote add bincrafters https://api.bintray.com/conan/bincrafters/public-conan
$ conan remote add tomsksoft https://api.bintray.com/conan/tomsksoft/cis1
```

Switch to the build directory and install build dependencies

```console
$ conan install ${PATH_TO_SRC} --profile -s build_type=Release --build=missing
```

Generate build scripts with CMake

```console
$ cmake ${PATH_TO_SRC} -DCMAKE_BUILD_TYPE=Release
```

You can set the BUILD_DOC and BUILD_TESTING CMake variables.
The default values are:

```
BUILD_DOC     ON
BUILD_TESTING OFF
```

Run build

```console
$ cmake --build .
```

When the build is completed, the executables will appear in:

```console
${PATH_TO_BUILD_DIR}/bin
```

## Installation

Run `deploy/deploy.py`

```console
$ ./deploy.py --execs_dir ${PATH_TO_BUILT_EXECUTABLES} --deploy_dir ${PATH_WHERE_CIS_WILL_BE_DEPLOYED}
```
