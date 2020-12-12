#include <err.h>

#include <iostream>

#include <mymoulette.hh>
#include <options/options.hh>
#include <cgroups/cgroups.hh>

int main(int argc, char *argv[])
{
    options::Options opt(argc, argv);

    try
    {
        cgroups::create_cgroups();
    }
    catch (cgroups::CgroupException& e)
    {
        err(ERR_CGROUPS, e.what());
    }
    return 0;
}
