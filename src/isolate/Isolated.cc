#include <unistd.h>
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
    {
        if (chdir(folder_.c_str()) == -1)
            throw IsolatedException("could not chdir");
    }

    Isolated::Isolated(std::nullptr_t null)
        : folder_(".")
    {
        (void) null;
        throw IsolatedException("could not create temp directory");
    }

}
