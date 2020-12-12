#include <err.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include <iostream>
#include <sstream>

#include <cgroups/cgroups.hh>

namespace cgroups
{
    static void memory(const Cgroup& Cgroup)
    {
        Cgroup.set_value("memory.max_usage_in_bytes", 1024 * 1024 * 1024);
    }

    static void cpuset(const Cgroup& Cgroup)
    {
        Cgroup.set_value("cpuset.cpus", 0);
    }

    static void pids(const Cgroup& Cgroup)
    {
        Cgroup.set_value("pids.max", "100");
    }

    cgroups create_cgroups(void)
    {
        auto cgroups = std::make_unique<cgroup_list>();

        const std::vector<cgroup_gen> schedule = {
            std::make_pair("memory", memory),
            std::make_pair("cpuset", cpuset),
            std::make_pair("pids", pids)
        };

        for (const auto& todo : schedule)
        {
            const auto& cgroup = cgroups->emplace_back(todo.first);
            todo.second(cgroup);
        }

        return cgroups;
    }

    // Cgroup member functions
    Cgroup::Cgroup(const std::string name)
        : name_(name)
    {
        std::ostringstream ss;
        ss << "/sys/fs/cgroup/" << name << "/" << dirname;
        path_ = ss.str();

        if (mkdir(path_.c_str(), 700) == -1)
            throw CgroupException("creation failed", name_);
    }

    Cgroup::~Cgroup()
    {
        try
        {
            if (rmdir(path_.c_str()) == -1)
                throw CgroupException("deletion failed", name_);
        }
        catch (CgroupException& e)
        {
            warn(e.what());
        }
    }

    // CgroupException member functions
    CgroupException::CgroupException(const std::string msg, const std::string cgroup)
    {
        std::ostringstream ss;
        ss << "cgroup `" << cgroup << "': " << msg;
        msg_ = ss.str();
    }

    CgroupException::CgroupException(const std::string msg, const std::string cgroup,
            const std::string file)
    {
        std::ostringstream ss;
        ss << "cgroup `" << cgroup << "': " << msg << " for file " << file;
        msg_ = ss.str();
    }

    const char* CgroupException::what()
    {
        return msg_.c_str();
    }

}
