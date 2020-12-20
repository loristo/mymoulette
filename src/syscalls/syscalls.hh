#pragma once

#include <exception>
#include <string>

namespace syscalls
{
    void filter_syscalls();

    class SyscallsException : public std::exception
    {
        public:
            SyscallsException(const std::string msg);

            const char *what();

        private:
            std::string msg_;
    };
}
