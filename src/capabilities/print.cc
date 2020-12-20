#include <err.h>
#include <unistd.h>

#include <iostream>

#include <capabilities/capabilities.hh>

#define CAPABILITIES(field) \
    X(CAP_AUDIT_CONTROL, field) \
    X(CAP_AUDIT_READ, field) \
    X(CAP_AUDIT_WRITE, field) \
    X(CAP_BLOCK_SUSPEND, field) \
    X(CAP_BPF, field) \
    X(CAP_CHOWN, field) \
    X(CAP_DAC_OVERRIDE, field) \
    X(CAP_DAC_READ_SEARCH, field) \
    X(CAP_FOWNER, field) \
    X(CAP_FSETID, field) \
    X(CAP_IPC_LOCK, field) \
    X(CAP_IPC_OWNER, field) \
    X(CAP_KILL, field) \
    X(CAP_LAST_CAP, field) \
    X(CAP_LEASE, field) \
    X(CAP_LINUX_IMMUTABLE, field) \
    X(CAP_MAC_ADMIN, field) \
    X(CAP_MAC_OVERRIDE, field) \
    X(CAP_MKNOD, field) \
    X(CAP_NET_ADMIN, field) \
    X(CAP_NET_BIND_SERVICE, field) \
    X(CAP_NET_BROADCAST, field) \
    X(CAP_NET_RAW, field) \
    X(CAP_PERFMON, field) \
    X(CAP_SETFCAP, field) \
    X(CAP_SETGID, field) \
    X(CAP_SETPCAP, field) \
    X(CAP_SETUID, field) \
    X(CAP_SYSLOG, field) \
    X(CAP_SYS_ADMIN, field) \
    X(CAP_SYS_BOOT, field) \
    X(CAP_SYS_CHROOT, field) \
    X(CAP_SYS_MODULE, field) \
    X(CAP_SYS_NICE, field) \
    X(CAP_SYS_PACCT, field) \
    X(CAP_SYS_PTRACE, field) \
    X(CAP_SYS_RAWIO, field) \
    X(CAP_SYS_RESOURCE, field) \
    X(CAP_SYS_TIME, field) \
    X(CAP_SYS_TTY_CONFIG, field) \
    X(CAP_WAKE_ALARM, field)


namespace capabilities
{
    void print_capabilities()
    {
        struct __user_cap_header_struct _hdrp;
        struct __user_cap_data_struct _datap[_LINUX_CAPABILITY_U32S_2];
        cap_user_header_t hdrp = &_hdrp;
        cap_user_data_t datap = _datap;

        hdrp->version = _LINUX_CAPABILITY_VERSION_3;
        if (capget(hdrp, NULL))
            throw CapabilitiesException("could not get capabilites version");


        if (hdrp->version != _LINUX_CAPABILITY_VERSION_3)
            throw CapabilitiesException("wrong capabilities version");

        hdrp->pid = getpid();
        if (capget(hdrp, datap))
            throw CapabilitiesException("could not get capabilities");

        std::cout << "cap_user_header_t\n";
        std::cout << "-----------------\n";
        std::cout << "Version: " << hdrp->version << "\n";
        std::cout << "PID: " << hdrp->pid << "\n";
        std::cout << "\n";
        std::cout << "cap_user_data_t\n";
        std::cout << "---------------\n";

#define X(cap, field) \
        if (datap[cap / 32].field & CAP_TO_MASK(cap)) \
            std::cout << "    " #cap "\n";

#define PRINTER(field) \
        printf(#field ": 0x%lx\n", \
                ((uint64_t)datap[1].field << 32) | datap[0].field); \
        CAPABILITIES(field)

        PRINTER(effective)
        PRINTER(permitted)
        PRINTER(inheritable)

#undef X
#undef PRINTER
    }
}
