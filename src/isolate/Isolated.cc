#include <curl/curl.h>

#include <iostream>

#include <isolate/isolate.hh>

namespace isolate
{
    void Isolated::run(const char *argv[]) const
    {
        (void) argv;
    }

    Isolated::Isolated(const std::string& folder)
        : folder_(folder)
    {}

    Isolated::Isolated(std::nullptr_t null)
        : folder_("")
    {
        (void) null;
        throw IsolatedException("could not create temp directory");
    }

}
