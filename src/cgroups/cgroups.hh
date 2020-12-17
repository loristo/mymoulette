#pragma once

#include <exception>
#include <functional>
#include <memory>
#include <string>
#include <utility>
#include <list>

namespace cgroups
{
    constexpr char dirname[] = "mymoulette";

    class Cgroup
    {
        public:
            Cgroup(const std::string name);
            Cgroup(const Cgroup& cgroup) = delete;
            ~Cgroup();

            template <typename T>
            void set_value(const std::string file, const T value) const;

        private:
            const std::string name_;
            std::string path_;
    };

    typedef std::list<Cgroup> cgroup_list;
    typedef std::unique_ptr<cgroup_list> cgroups;
    typedef std::pair<const char *, std::function<void(const Cgroup&)>> cgroup_gen;

    class CgroupException : public std::exception
    {
        public:
            CgroupException(const std::string msg, const std::string cgroup);
            CgroupException(const std::string msg, const std::string cgroup, const std::string file);

            const char *what();

        private:
            std::string msg_;
    };

    cgroups create_cgroups();
}

#include <cgroups/cgroups.hxx>
