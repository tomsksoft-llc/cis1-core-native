# cis1 core development guide

## Common rules

The core consists of many executables described in [cis1-docs](https://github.com/tomsksoft-llc/cis1-docs). It can have additional executables if needed, for example `cis_cron_daemon` which is not described in docs and used only for service purpose.

Every executable should return exitcode zero in case of success and non-zero in other cases.

If execution fails, process should print a diagnostic message in stderr and also to one of logs, usually `cis_log`.

`cis_log` is for cis-wide events, like a start of a new session, some critical error.

`session_log` should include in itself all session events and errors. It should help in tracking errors in user scripts.

`webui_log` shouldn't be used separately in most cases. All writes to `cis_log` and `session_log` automatically sends data to `webui_log` too.

`tee_log` write to all logs.

## Repository structure

`include` - C++ header files.

`src` - C++ source files.

`cmake` - CMake scripts.

`test_package` - unit tests.

`docs` - additional documentation.

`version.txt` - package version.

## Backwards compatibility

Every executable must be backward compatible at least in one major version.

It’s about return codes and event stdout and stderr messages, which either can be used for user's diagnostic or parsed for some further processing.

## Contributing

Follow [the GitHub pull request workflow](https://guides.github.com/introduction/flow/): fork, branch, commit, pull request, automated tests, review, merge.

* Do PR only to dev branch.
* Every PR must have issue associated with it.
* Try to write smaller commits, that do exactly one thing.
* Commit message should briefly describe changes in code.
* Try to cover your code with unit tests.
* For extra code-health changes, either file a separate issue, or make it a separate PR that can be easily reviewed.
* Add reviewer to pull request.

Code style for C++ code can be found on [this](https://github.com/tomsksoft-llc/cis1-webui-native-srv-cpp/wiki/Codestyle "C++ CodeStyle") wiki-page.

## External libraries

External libraries usage is allowed, but you should check license compatibility.

## Documentation

Every class, function, method should be documented with doxygen.
