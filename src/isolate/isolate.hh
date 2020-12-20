#pragma once

#include <string>
#include <exception>
#include <optional>

namespace isolate
{
    class Isolated
    {
        public:
            ~Isolated();

            int run(char *argv[]);
            int child(char *argv[]);

        protected:
            Isolated(const std::string& folder, const std::optional<std::string> student);
            Isolated(std::nullptr_t null);
            const std::string folder_;
            const std::optional<std::string> student_;

        private:
            void pivot_root();
            void set_hostname();
            void mount_student();
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
            IsolatedRootfs(const std::string& folder, const std::optional<std::string> student);
    };

    class IsolatedDocker : public Isolated
    {
        public:
            IsolatedDocker(const std::string& folder, const std::optional<std::string> student);
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

