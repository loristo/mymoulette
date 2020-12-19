#pragma once

#include <string>
#include <exception>

namespace isolate
{
    class Isolated
    {
        public:
            ~Isolated() = default;

            void run(const char *argv[]) const;

        protected:
            Isolated(const std::string& folder);
            Isolated(std::nullptr_t null);
            const std::string folder_;
    };

    class IsolatedRootfs : public Isolated
    {
        public:
            IsolatedRootfs(const std::string& dir);
    };

    class IsolatedDocker : public Isolated
    {
        public:
            IsolatedDocker(const std::string& docker);
            ~IsolatedDocker();
    };

    class IsolatedException : public std::exception
    {
        public:
            IsolatedException(const std::string& msg);

            const char* what();

        private:
            std::string msg_;
    };
}

