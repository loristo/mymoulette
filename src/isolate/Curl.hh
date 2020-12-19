#pragma once

#include <curl/curl.h>

#include <string>

namespace isolate
{
    class Curl
    {
        public:
            Curl();
            ~Curl();

            std::string get(const std::string url);
            void set_auth(const std::string& token);
            void set_auth(const std::string& token, const std::string& mediatype);

        private:
            CURL *conn_;
            struct curl_slist* headers_ = NULL;
    };

    class CurlException : public std::exception
    {
        public:
            CurlException(const std::string& msg);
            CurlException(const std::string& url, long code);

            const char* what();

        private:
            std::string msg_;
    };
}
