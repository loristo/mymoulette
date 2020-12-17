#include <curl/curl.h>

#include <isolate/isolate.hh>

namespace isolate
{
    IsolatedDocker::IsolatedDocker(const std::string image)
        : Isolated(".img")
    {
        auto curl = curl_easy_init();
        (void) image;
    }

    IsolatedDocker::~IsolatedDocker()
    {}
}
