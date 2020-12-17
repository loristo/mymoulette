#include <curl/curl.h>

#include <isolate/isolate.hh>

namespace isolate
{
    void Isolated::run(const char *argv[]) const
    {
        (void) argv;
    }

    Isolated::Isolated(const std::string folder)
        : folder_(folder)
    {}
}
