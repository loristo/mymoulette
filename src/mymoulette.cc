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
        return std::make_shared<isolate::IsolatedDocker>(
                    opt.get_docker(),
                    opt.get_student_dir()
                );
    else
        return std::make_shared<isolate::IsolatedRootfs>(
                    opt.get_rootfs(),
                    opt.get_student_dir()
                );
}

int main(int argc, char *argv[])
{
    int res = 0;
    options::Options opt(argc, argv);

    try
    {
        auto cgroups = cgroups::create_cgroups();
        auto container = get_container(opt);
        res = container->run(opt.get_moulette_prog());
    }
    catch (cgroups::CgroupException& e)
    {
        warn(e.what());
        return ERR_CGROUPS;
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

    return res;
}
