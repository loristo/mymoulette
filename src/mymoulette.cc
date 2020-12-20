#include <err.h>

#include <iostream>
#include <memory>

#include <mymoulette.hh>
#include <options/options.hh>
#include <cgroups/cgroups.hh>
#include <capabilities/capabilities.hh>
#include <isolate/isolate.hh>
#include <isolate/Curl.hh>
#include <isolate/Tar.hh>

std::shared_ptr<isolate::Isolated> get_container(const options::Options& opt)
{
    if (opt.is_docker())
        return std::make_shared<isolate::IsolatedDocker>(opt.get_docker());
    else
        return std::make_shared<isolate::IsolatedRootfs>(opt.get_rootfs());
}

int main(int argc, char *argv[])
{
    options::Options opt(argc, argv);

    try
    {
        // auto cgroups = cgroups::create_cgroups();
        // capabilities::lower_capabilites();
        auto container = get_container(opt);
        container->run(opt.get_moulette_prog());
    }
    catch (cgroups::CgroupException& e)
    {
        warn(e.what());
        return ERR_CGROUPS;
    }
    catch (capabilities::CapabilitiesException& e)
    {
        warn(e.what());
        return ERR_CAPABILITIES;
    }
    catch (isolate::IsolatedException& e)
    {
        warn(e.what());
        return ERR_ISOLATED;
    }
    catch (isolate::CurlException& e)
    {
        warnx(e.what());
        return ERR_CURL;
    }
    catch (isolate::TarException& e)
    {
        warnx(e.what());
        return ERR_CURL;
    }
    catch (std::exception& e)
    {
        warnx(e.what());
        return ERR_UNKNOWN;
    }

    return 0;
}
