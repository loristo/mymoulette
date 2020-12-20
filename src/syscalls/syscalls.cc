#define _DEFAULT_SOURCE

#include <stdio.h>
#include <stddef.h>
#include <errno.h>
#include <unistd.h>
#include <linux/seccomp.h>
#include <linux/filter.h>
#include <linux/audit.h>
#include <sys/ptrace.h>
#include <sys/syscall.h>
#include <sys/prctl.h>

#include <vector>

#include <syscalls/syscalls.hh>

#define ARRAY_SIZE(arr) (sizeof(arr) / sizeof((arr)[0]))

namespace syscalls
{
    static void filter_syscall(unsigned int syscall_nr)
    {
        unsigned int upper_nr_limit = __X32_SYSCALL_BIT - 1;

        struct sock_filter filter[] = {
            /* [0] Load architecture from 'seccomp_data' buffer into
            accumulator */
            BPF_STMT(BPF_LD | BPF_W | BPF_ABS,
                (offsetof(struct seccomp_data, arch))),

            /* [1] Jump forward 5 instructions if architecture does not
            match 't_arch' */
            BPF_JUMP(BPF_JMP | BPF_JEQ | BPF_K, AUDIT_ARCH_X86_64, 0, 5),

            /* [2] Load system call number from 'seccomp_data' buffer into
            accumulator */
            BPF_STMT(BPF_LD | BPF_W | BPF_ABS,
                (offsetof(struct seccomp_data, nr))),

            /* [3] Check ABI - only needed for x86-64 in deny-list use
            cases.  Use BPF_JGT instead of checking against the bit
            mask to avoid having to reload the syscall number. */
            BPF_JUMP(BPF_JMP | BPF_JGT | BPF_K, upper_nr_limit, 3, 0),

            /* [4] Jump forward 1 instruction if system call number
            does not match 'syscall_nr' */
            BPF_JUMP(BPF_JMP | BPF_JEQ | BPF_K, syscall_nr, 0, 1),

            /* [5] Matching architecture and system call: don't execute
            the system call, and return 'f_errno' in 'errno' */
            BPF_STMT(BPF_RET | BPF_K,
                SECCOMP_RET_ERRNO | (EPERM & SECCOMP_RET_DATA)),

            /* [6] Destination of system call number mismatch: allow other
            system calls */
            BPF_STMT(BPF_RET | BPF_K, SECCOMP_RET_ALLOW),

            /* [7] Destination of architecture mismatch: kill process */
            BPF_STMT(BPF_RET | BPF_K, SECCOMP_RET_KILL_PROCESS),
        };

        struct sock_fprog prog;
        prog.len = ARRAY_SIZE(filter);
        prog.filter = filter;

        if (syscall(SYS_seccomp, SECCOMP_SET_MODE_FILTER, 0, &prog))
            throw SyscallsException("could not seccomp");
    }

    void filter_syscalls()
    {
        if (prctl(PR_SET_NO_NEW_PRIVS, 1, 0, 0, 0))
            throw SyscallsException("could not prctl");

        const std::vector<unsigned int> syscalls_ =
        {
            SYS_clock_nanosleep,
            SYS_pivot_root,
            SYS_personality,
            SYS_nfsservctl,
            SYS_chroot,
            SYS_clock_nanosleep,
            SYS_sethostname
        };
        for (auto syscall : syscalls_)
            filter_syscall(syscall);
    }



    SyscallsException::SyscallsException(const std::string msg)
        : msg_(msg)
    {}

    const char *SyscallsException::what()
    {
        return msg_.c_str();
    }


}
