#include <err.h>

#include <iostream>

#include <mymoulette.hh>
#include <options/options.hh>
#include <cgroups/cgroups.hh>
#include <capabilities/capabilities.hh>

int main(int argc, char *argv[])
{
    options::Options opt(argc, argv);

    try
    {
        auto cgroups = cgroups::create_cgroups();
        capabilities::lower_capabilites();
    }
    catch (cgroups::CgroupException& e)
    {
        err(ERR_CGROUPS, e.what());
    }
    catch (capabilities::CapabilitiesException& e)
    {
        err(ERR_CAPABILITIES, e.what());
    }
    catch (std::exception& e)
    {
        err(ERR_UNKNOWN, e.what());
    }

    return 0;
}
