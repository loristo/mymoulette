#include <err.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>

#include <nlohmann/json.hpp>

#include <exception>
#include <filesystem>
#include <iostream>
#include <sstream>

#include <isolate/isolate.hh>
#include <isolate/Curl.hh>
#include <isolate/Tar.hh>

namespace isolate
{
    using json = nlohmann::json;
    static char dirname[] = ".mymoulette.XXXXXX";
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
        for (const auto& man : j["manifests"])
        {
            const std::string os = man["platform"]["os"];
            const std::string arch = man["platform"]["architecture"];
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

    static void get_layers(Curl& c, const std::string& folder, const std::string& image,
            const std::string& digest, const std::string& token)
    {
        Tar t(folder);
        std::stringstream ss;
        ss << baseurl << image << "/manifests/" << digest;
        auto resp = c.get(ss.str());
        auto j = json::parse(resp);
        c.set_auth_only(token);
        for (const auto& layer : j["layers"])
        {
            const std::string layer_digest = layer["digest"];
            std::stringstream ss;
            ss << baseurl << image << "/blobs/" << layer_digest;
            t.untar(c.get(ss.str()));
        }
    }

    IsolatedDocker::IsolatedDocker(const std::string& docker,
            const std::optional<std::string> student)
        : Isolated(mkdtemp(dirname), student)
    {
        Dir save(".");
        if (chdir(folder_.c_str()) == -1)
            throw IsolatedException("could not chdir");

        std::string image, version;
        split_image(docker, image, version);
        Curl c;

        auto token = get_token(c, image);
        c.set_auth(token);
        auto digest = get_digest(c, image, version);
        auto mediatype = get_mediatype(c, image, version);
        c.set_auth(token, mediatype);
        get_layers(c, folder_, image, digest, token);

        if (fchdir(save.fd_) == -1)
            throw IsolatedException("could not chdir");
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

    Dir::Dir(const std::string& dir)
    {
        fd_ = open(dir.c_str(), O_RDONLY);
        if (fd_ == -1)
            throw IsolatedException("could not open .");
    }

    Dir::~Dir()
    {
        close(fd_);
    }
}
