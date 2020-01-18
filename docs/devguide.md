# cis1 core development guide

## Common rules

The core consists of many executables described in [cis1-docs](https://github.com/tomsksoft-llc/cis1-docs). It can have additional executables if needed, for example, `cis_cron_daemon` which is not described in docs and used only for service purposes.

Every executable should return exitcode zero in case of success and non-zero in other cases.

If execution fails, the process should write a diagnostic message into stderr and one of the logs, usually into `cis_log`.

`cis_log` is intended for the cis-wide events, like the start of a new session or some critical error.

`session_log` should contain all session events and errors. It should help with error tracking in user scripts.

In most cases, `webui_log` shouldn't be used on its own. Everything is written to `cis_log`, and additionally, `session_log` automatically sends data to `webui_log`.

`tee_log` writes to all logs.

## Repository structure

`include` - C++ header files.

`src` - C++ source files.

`cmake` - CMake scripts.

`test_package` - unit tests.

`docs` - additional documentation.

`version.txt` - package version.

## Backwards compatibility

Every executable must be backward compatible at least in one major version.

This applies to return codes and stdout and stderr messages related to the corresponding events, which then can either be used for diagnostics or parsed for some further processing.

## Contributing

Follow [the GitHub pull request workflow](https://guides.github.com/introduction/flow/): fork, branch, commit, pull request, automated tests, review, merge.

* Do PR to dev branch only.
* Every PR must have an issue associated with it.
* Try writing smaller commits that do one particular thing only.
* Commit message should briefly describe the code changes.
* Try covering your code with unit tests.
* For extra code-health changes, either submit a separate issue or make it a separate PR that can be easily reviewed.
* Add reviewer to the pull request.

The сode style for the C++ code can be found on [this](https://github.com/tomsksoft-llc/cis1-webui-native-srv-cpp/wiki/Codestyle "C++ CodeStyle") wiki-page.

## External libraries

Using external libraries is allowed, but you should check the license compatibility first.

## Documentation

Every class, function, method should be documented with doxygen.
