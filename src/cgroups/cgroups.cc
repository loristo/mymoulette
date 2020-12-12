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
        (void) Cgroup;
    }

    cgroup_list create_cgroups(void)
    {
        auto cgroups = std::make_unique<std::vector<Cgroup>>();

        const std::vector<cgroup_gen> schedule = {
            std::make_pair("memory", memory)
        };

        for (auto& todo : schedule)
        {
            auto& cgroup = cgroups->emplace_back(todo.first);
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
            throw CgroupException("creation", name_);
    }

    Cgroup::~Cgroup()
    {
        try
        {
            if (rmdir(path_.c_str()) == -1)
                throw CgroupException("deletion", name_);
        }
        catch (cgroups::CgroupException& e)
        {
            warn(e.what());
        }
    }

    // CgroupException member functions
    CgroupException::CgroupException(const std::string msg, const std::string cgroup)
    {
        std::ostringstream ss;
        ss << "cgroup " << cgroup << " " << msg << " failed";
        msg_ = ss.str();
    }

    const char* CgroupException::what()
    {
        return msg_.c_str();
    }


}
