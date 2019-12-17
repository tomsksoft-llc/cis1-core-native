#include "cis_version.h"

#include <iostream>

#include "version.h"

void print_version()
{
    std::cout << cis_current_version;

    if(git_retreived_state)
    {
        std::cout << " " << git_sha1
                  << (git_is_dirty ? " dirty" : "") << std::endl;
    }
}
