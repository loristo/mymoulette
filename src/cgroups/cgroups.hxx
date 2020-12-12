#pragma once

#include <cgroups/cgroups.hh>

#include <fstream>

namespace cgroups
{

    template <typename T>
    void Cgroup::set_value(const std::string file, const T value) const
    {
        std::stringstream ss;
        ss << this->path_ << "/" << file;
        std::ofstream fstream(ss.str());
        if (!fstream)
            throw CgroupException("opening fail", this->path_, file);
        fstream << value;
        fstream.close();
        if (!fstream)
            throw CgroupException("writing fail", this->path_, file);
    }

}
