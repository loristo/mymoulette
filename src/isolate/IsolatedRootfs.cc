#include <curl/curl.h>

#include <isolate/isolate.hh>

namespace isolate
{
    IsolatedRootfs::IsolatedRootfs(const std::string& folder,
            const std::optional<std::string> student)
        : Isolated(folder, student)
    {}
}
