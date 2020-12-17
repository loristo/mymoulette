#pragma once

#include <exception>

#include <sys/capability.h>

namespace capabilities
{
    void lower_capabilites(void);
    inline void print_capabilities();

    class CapabilitiesException : public std::exception
    {
        public:
            CapabilitiesException(const std::string msg);

            const char *what();

        private:
            std::string msg_;
    };

}
