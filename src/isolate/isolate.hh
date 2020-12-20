#pragma once

#include <string>
#include <exception>

namespace isolate
{
    class Isolated
    {
        public:
            ~Isolated();

            void run(char *argv[]);
            int child(char *argv[]);
            void pivot_root();
            void set_hostname();

        protected:
            Isolated(const std::string& folder);
            Isolated(std::nullptr_t null);
            const std::string folder_;
    };

    struct exec_data
    {
        exec_data(char **argv, Isolated *container);

        Isolated *container;
        char **argv;
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

    struct Dir
    {
        Dir(const std::string& dir);
        ~Dir();

        int fd_;
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

