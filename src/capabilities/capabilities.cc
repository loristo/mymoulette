#include <err.h>
#include <sys/prctl.h>
#include <unistd.h>

#include <iostream>

#include <capabilities/capabilities.hh>

namespace capabilities
{
    void lower_capabilites()
    {
        constexpr auto cap_to_remove = CAP_DAC_OVERRIDE
            | CAP_AUDIT_WRITE
            | CAP_CHOWN
            | CAP_FOWNER
            | CAP_FSETID
            | CAP_KILL
            | CAP_MKNOD
            | CAP_NET_BIND_SERVICE
            | CAP_NET_RAW
            | CAP_SETFCAP
            | CAP_SETGID
            | CAP_SETPCAP
            | CAP_SETUID
            | CAP_SYS_ADMIN
            | CAP_SYS_CHROOT;

        if (prctl(PR_CAPBSET_DROP, cap_to_remove, 0, 0, 0, 0) == -1)
            throw CapabilitiesException("could not reduce capabilitites");
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
