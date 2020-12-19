#include <iostream>
#include <fstream>
#include <sstream>

#include <isolate/Curl.hh>
#include <isolate/isolate.hh>

namespace isolate
{
    static size_t write_function(void *ptr, size_t size, size_t nmemb, std::string* data)
    {
        data->append((char *) ptr, size * nmemb);
        return size * nmemb;
    }

    static size_t write_to_file_function(void *ptr, size_t size, size_t nmemb, std::ofstream* file)
    {
        file->write((char *)ptr, size * nmemb);
        return size * nmemb;
    }

    Curl::Curl()
        : conn_(curl_easy_init())
    {
        if (!conn_)
            throw CurlException("could not create curl object");
        curl_easy_setopt(this->conn_, CURLOPT_FOLLOWLOCATION, 1L);
    }

    std::string Curl::get(const std::string url)
    {
        std::string res;
        long response_code;

        curl_easy_setopt(this->conn_, CURLOPT_WRITEFUNCTION, write_function);
        curl_easy_setopt(this->conn_, CURLOPT_WRITEDATA, &res);
        curl_easy_setopt(this->conn_, CURLOPT_URL, url.c_str());

        curl_easy_setopt(this->conn_, CURLOPT_HTTPHEADER, this->headers_);
        curl_easy_perform(this->conn_);

        curl_easy_getinfo(this->conn_, CURLINFO_RESPONSE_CODE, &response_code);

        if (response_code != 200)
            throw CurlException(url, response_code);

        return res;
    }

    void Curl::wget(const std::string& url, const std::string& path)
    {
        std::ofstream f(path);
        long response_code;

        curl_easy_setopt(this->conn_, CURLOPT_WRITEFUNCTION, write_to_file_function);
        curl_easy_setopt(this->conn_, CURLOPT_WRITEDATA, &f);
        curl_easy_setopt(this->conn_, CURLOPT_URL, url.c_str());

        curl_easy_setopt(this->conn_, CURLOPT_HTTPHEADER, this->headers_);
        curl_easy_perform(this->conn_);

        curl_easy_getinfo(this->conn_, CURLINFO_RESPONSE_CODE, &response_code);

        if (response_code != 200)
            throw CurlException(url, response_code);
    }

    void Curl::set_auth(const std::string& token)
    {
        this->set_auth(token,
                "Accept: application/vnd.docker.distribution.manifest.list.v2+json");
    }

    void Curl::set_auth(const std::string& token, const std::string& mediatype)
    {
        this->set_auth_only(token);
        headers_ = curl_slist_append(headers_, mediatype.c_str());
    }

    void Curl::set_auth_only(const std::string& token)
    {
        curl_slist_free_all(headers_);
        headers_ = NULL;
        std::string auth = "Authorization: Bearer ";
        auth.append(token);
        headers_ = curl_slist_append(headers_, auth.c_str());
    }


    Curl::~Curl()
    {
        curl_slist_free_all(headers_);
        curl_easy_cleanup(this->conn_);
    }


    CurlException::CurlException(const std::string& msg)
        : msg_(msg)
    {}

    CurlException::CurlException(const std::string& url, long code)
    {
        std::stringstream ss;
        ss << "could not curl `" << url << "': error code " << code;
        msg_ = ss.str();
    }

    const char* CurlException::what()
    {
        return msg_.c_str();
    }

}
