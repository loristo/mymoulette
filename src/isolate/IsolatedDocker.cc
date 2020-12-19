#include <err.h>

#include <nlohmann/json.hpp>

#include <exception>
#include <filesystem>
#include <iostream>
#include <sstream>

#include <isolate/isolate.hh>
#include <isolate/Curl.hh>

namespace isolate
{
    using json = nlohmann::json;
    static char dirname[] = "/tmp/mymoulette.XXXXXX";
    static char baseurl[] = "https://registry-1.docker.io/v2/";

    static void split_image(const std::string& docker, std::string& image, std::string& version)
    {
        size_t colon = docker.find(":");
        image = docker.substr(0, colon);
        version = docker.substr(colon + 1);
    }

    static std::string get_token(Curl& c, const std::string& image)
    {
        std::stringstream ss;
        ss
            << "https://auth.docker.io/token?service=registry.docker.io&scope=repository:"
            << image << ":pull";
        auto res = c.get(ss.str());
        auto j = json::parse(res);
        return j["token"];
    }

    static std::string get_conf(Curl& c, const std::string& image,
            const std::string& version, const std::string& field)
    {
        std::stringstream ss;
        ss << baseurl << image << "/manifests/" << version;
        auto resp = c.get(ss.str());
        auto j = json::parse(resp);
        for (auto man : j["manifests"])
        {
            const std::string os = man["platform"]["os"];
            const std::string arch = man["platform"]["architecture"];
            std::cout << os << " " << arch << '\n';
            if (os.compare(OS) == 0
                    && arch.compare(ARCH) == 0)
                return man[field];
        }
        return "";
    }

    static std::string get_digest(Curl& c, const std::string& image, const std::string& version)
    {
        return get_conf(c, image, version, "digest");
    }

    static std::string get_mediatype(Curl& c, const std::string& image, const std::string& version)
    {
        return get_conf(c, image, version, "mediaType");
    }

    IsolatedDocker::IsolatedDocker(const std::string& docker)
        : Isolated(mkdtemp(dirname))
    {
        std::string image, version;
        split_image(docker, image, version);
        Curl c;

        auto token = get_token(c, image);
        c.set_auth(token);
        auto digest = get_digest(c, image, version);
        auto mediatype = get_mediatype(c, image, version);
        std::cout << "digest: " << digest << ", mediatype: " << mediatype << '\n';
        c.set_auth(token, mediatype);
    }

    IsolatedDocker::~IsolatedDocker()
    {
        try
        {
            std::filesystem::remove_all(dirname);
        }
        catch (std::exception& e)
        {
            warnx(e.what());
        }
    }
}
