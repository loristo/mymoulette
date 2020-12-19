#include <curl/curl.h>

#include <isolate/isolate.hh>

namespace isolate
{
    IsolatedException::IsolatedException(const std::string& msg)
        : msg_(msg)
    {}

    const char* IsolatedException::what()
    {
        return msg_.c_str();
    }
}
