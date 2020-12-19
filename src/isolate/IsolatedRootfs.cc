#include <curl/curl.h>

#include <isolate/isolate.hh>

namespace isolate
{
    IsolatedRootfs::IsolatedRootfs(const std::string& dir)
        : Isolated(dir)
    {}
}
