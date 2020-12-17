#include <err.h>
#include <sys/prctl.h>
#include <unistd.h>

#include <iostream>

#include <capabilities/capabilities.hh>

namespace capabilities
{
    void lower_capabilites()
    {
        constexpr auto cap_to_remove = CAP_NET_RAW | CAP_DAC_OVERRIDE;
        if (prctl(PR_CAP_AMBIENT, PR_CAP_AMBIENT_LOWER, cap_to_remove, 0, 0) == -1)
        {
            throw CapabilitiesException("could not reduce capabilitites");
        }

        // print_capabilities();
    }

    CapabilitiesException::CapabilitiesException(const std::string msg)
        : msg_(msg)
    {}

    const char *CapabilitiesException::what()
    {
        return this->msg_.c_str();
    }
}
