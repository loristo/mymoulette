#pragma once

#include <exception>
#include <functional>
#include <memory>
#include <string>
#include <utility>
#include <vector>

namespace cgroups
{
    constexpr char dirname[] = "mymoulette";

    class Cgroup
    {
        public:
            Cgroup(const std::string name);
            ~Cgroup();

        private:
            const std::string name_;
            std::string path_;
    };

    typedef std::unique_ptr<std::vector<Cgroup>> cgroup_list;
    typedef std::pair<const char *, std::function<void(const Cgroup&)>> cgroup_gen;

    class CgroupException : public std::exception
    {
        public:
            CgroupException(const std::string msg, const std::string cgroup);

            const char* what();

        private:
            std::string msg_;
    };

    cgroup_list create_cgroups(void);
}
